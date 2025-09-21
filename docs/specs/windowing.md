# RatOS Specification: Windowing & Graphics (Windowing)

## Overview

This specification defines RatOS's native windowing and graphics subsystem as set out in ADR-0006. The subsystem provides:

* A **native RatOS compositor and protocol** (Rat-native), designed for retained-surface rendering and modern GPU-friendly workflows.
* A **Win32-style message loop + WndProc** compatibility model for application programmers.
* A **modern async/event** model (completion-queue friendly) for reactive applications and toolkits.
* Well-defined primitives for surfaces, input, event delivery, and presentation.

This document describes the public RatAPI surface, lifecycle rules, rendering models (software & GPU), input semantics, security considerations, and an incremental implementation plan.

---

## Goals

* **Cohesive developer experience:** A single, coherent API for application windows, events, and rendering.
* **Performance and safety:** Use retained surfaces and GPU acceleration where possible.
* **Extensibility:** Allow RatOS-specific features (effects, composition policies) to be exposed cleanly.
* **Compatibility:** Provide a Wayland-host compatibility mode to run existing Wayland applications later.

---

## Naming & Conventions

Public API names follow project conventions (see `naming-conventions.md`):

* Functions and types: **PascalCase** with `Rat` prefix (e.g., `RatCreateSurface`, `RatMessageQueue`).
* Error enums: `RAT_*` ALL\_CAPS.
* Internal symbols: `snake_case`.

All API return `RatStatus` unless otherwise noted. Asynchronous completion uses `HCOMPQ` completion queues as defined in `specs/async-io.md`.

---

## Core Types

```c
typedef uint64_t RatSurface;        // opaque surface handle
typedef uint64_t RatWindow;         // alias to RatSurface for windows
typedef uint64_t RatMessageQueue;   // per-process or per-window message queue

typedef enum {
  RAT_OK = 0,
  RAT_ERR_UNKNOWN = 1,
  RAT_ERR_INVALID_ARG = 2,
  RAT_ERR_NO_RESOURCES = 3,
  RAT_ERR_ACCESS_DENIED = 4,
  RAT_ERR_NOT_SUPPORTED = 5,
  RAT_ERR_TIMED_OUT = 6,
} RatStatus;

// Pixel formats (expand as needed)
typedef enum {
  RAT_PIXEL_FMT_ARGB32,   // 32-bit, 8 bits per channel, premultiplied alpha
  RAT_PIXEL_FMT_XRGB32,   // 32-bit, no alpha
  RAT_PIXEL_FMT_RGBA16F,  // half-float 16-bit per channel
  RAT_PIXEL_FMT_RGB10_A2, // 10 bits RGB + 2 bits alpha
} RatPixelFormat;
```

---

## Surface Lifecycle & APIs

Surfaces are the fundamental drawable objects. A `RatSurface` represents a buffer, its metadata, and associated state (size, scale, transform).

### Creation & Destruction

```c
RatStatus RatCreateSurface(uint32_t width, uint32_t height, RatPixelFormat fmt, uint32_t flags, RatSurface* out);
RatStatus RatDestroySurface(RatSurface s);
```

* `flags` may include hints (GPU\_BACKED, SHARED, IMMUTABLE\_CONTENT, etc.).
* `RatSurface` may be used as a top-level window (`RatWindow`) or as an offscreen buffer.

### Mapping & Software Rendering

```c
RatStatus RatMapSurface(RatSurface s, void** outPtr, size_t* outStride);
RatStatus RatUnmapSurface(RatSurface s);
```

* `RatMapSurface` returns a pointer to the surface pixels in client address space for CPU drawing. The memory is valid until `RatUnmapSurface`.
* For GPU-backed surfaces, `RatMapSurface` may fail and require the app to use GPU APIs.

### GPU Integration

```c
// Get a native handle for EGL/GL/Vulkan integration (opaque pointer type)
RatStatus RatGetSurfaceNativeHandle(RatSurface s, void** outNativeHandle);

// Post a GPU-sourced image (platform-specific wrapper)
RatStatus RatAttachGpuImage(RatSurface s, const RatGpuImageDescriptor* desc);
```

* Describe supported backends in implementation notes (EGL/GBM, Vulkan, Metal on other ports).

### Commit/Present

```c
RatStatus RatCommitSurface(RatSurface s); // present latest attached buffer
RatStatus RatDamageSurface(RatSurface s, int x, int y, int w, int h); // optional
```

* `RatCommitSurface` signals the compositor to composite the surface. The compositor decides when the frame reaches the display.
* `RatDamageSurface` informs compositor of changed region (optimisation hint).

### Frame Feedback

* When a frame is presented, the compositor posts a `FRAME_DONE` event (via message queue or completion queue) to the surface owner, including timestamps and presentation metadata.

---

## Windows, Owners & Message Queues

A **window** is a surface that is managed by the window manager (has decorations, position, input routing). Each process can create a message queue for handling events.

### Window Creation

```c
RatStatus RatCreateWindow(RatSurface surface, const RatWindowOptions* opts, RatWindow* outWindow);
RatStatus RatDestroyWindow(RatWindow w);
```

* `RatWindowOptions` includes title, initial position, size, window type (normal, popup, tooltip), and flags (resizable, decorated).
* Creating a window wraps the provided `RatSurface` as a top-level surface and registers it with compositor.

### Message Queue APIs (Win32-style)

```c
typedef struct { uint32_t message; uintptr_t wParam; intptr_t lParam; RatHandle target; } RatMessage;
RatStatus RatCreateMessageQueue(RatMessageQueue* out);
RatStatus RatDestroyMessageQueue(RatMessageQueue q);
RatStatus RatPostMessage(RatMessageQueue q, const RatMessage* msg);
RatStatus RatSendMessage(RatMessageQueue q, const RatMessage* msg, RatMessage* outReply, int32_t timeout_ms);
RatStatus RatGetMessage(RatMessageQueue q, RatMessage* outMsg, int32_t timeout_ms);
RatStatus RatPeekMessage(RatMessageQueue q, RatMessage* outMsg);
```

* Message queues integrate with `RatWaitForObject` (threads can wait on queues) and with `HCOMPQ` for async models.
* `RatSendMessage` allows synchronous RPC-like messaging (e.g., request/reply). The compositor enforces authorization when messages cross process boundaries.

### WndProc and Dispatch

* Developers implement a `WndProc(RatMessage* msg)` callback that inspects `msg->message` and handles messages via a switch statement.
* `RatDispatchMessage(const RatMessage* msg)` is a helper that invokes the WndProc for the target window.

---

## Event Model (Modern, Async)

For event-driven apps and toolkits that prefer async flows, RatOS supports registering surfaces/windows with completion queues or event callbacks.

```c
RatStatus RatSubscribeSurfaceEvents(RatSurface s, HCOMPQ q, uint64_t userTag);
```

* Events posted to the completion queue contain structured `RatCompletion` entries with event codes (INPUT\_EVENT, FRAME\_DONE, FOCUS\_CHANGE, RESIZE).
* Event payloads reference stable memory (or provide handles to shared buffers) to avoid copying large data.

---

## Input & Focus Semantics

* Compositor owns global input configuration and routing.
* Focus may be per-surface or per-window, and the compositor sends `FOCUS_GAIN`/`FOCUS_LOST` events.
* Raw input (pointer motion, key codes) is delivered via `INPUT_EVENT` messages. The spec exposes both high-level events (click, keypress) and raw events for games.
* Pointer capture and grab APIs:

```c
RatStatus RatCapturePointer(RatSurface s);
RatStatus RatReleasePointer(RatSurface s);
```

---

## Compositor Responsibilities & Policies

The compositor is responsible for:

* Window stacking, compositing, clipping, and effects.
* Input routing and focus policies.
* Enforcing security boundaries between clients.
* Managing presentation timing and vsync.
* Providing a Wayland-host adapter (see compatibility notes).

Compositors may implement platform-specific policies: animation, transitions, power-aware composition, and multi-GPU support.

---

## Buffer Formats & Performance

* Support for a minimal set of pixel formats is required (ARGB32, XRGB32). Implementations may add higher-precision formats.
* Surfaces should support damage regions, partial updates, and batch commits for performance.
* GPU path should support texture import/export to avoid CPU copies.

---

## Security & Access Control

* Each client may only map/modify surfaces it owns. Shared surfaces require explicit `RatShareSurface` semantics and ACLs.
* Message passing across processes is mediated by the compositor: `RatSendMessage` across process boundaries is checked against ACLs.
* The compositor should run with limited privileges and drop capabilities where possible. Surface memory must avoid exposing kernel pointers or secrets.

---

## Wayland Compatibility & Migration Strategy

We will **not** depend on Wayland for core protocol but will provide a **Wayland-host adapter** in the compositor:

* Implement a translator that accepts Wayland clients and maps their `wl_surface` buffers to Rat surfaces.
* Provide a compatibility mode that ensures common Wayland clients render correctly.
* Over time, maintain a small compatibility shim rather than a forked Wayland tree to reduce maintenance burden.

---

## Error Handling & Diagnostics

* Use `RatStatus` return codes. Provide `RatGetLastErrorDetail()` for extended diagnostics (timestamps, compositor logs, native error codes).
* Compose debugging tools: a `ratctl` utility to inspect surfaces, clients, and compositor state. Logging channels should include security-sensitive redaction.

---

## Examples

### Simple window with software rendering

```c
RatSurface s; RatCreateSurface(800, 600, RAT_PIXEL_FMT_ARGB32, 0, &s);
RatWindow w; RatCreateWindow(s, NULL, &w);
RatMessageQueue q; RatCreateMessageQueue(&q);

for (;;) {
  RatMessage m;
  RatGetMessage(q, &m, -1);
  switch (m.message) {
    case RAT_MSG_PAINT: {
      void* pixels; size_t stride;
      RatMapSurface(s, &pixels, &stride);
      // draw into pixels
      RatUnmapSurface(s);
      RatCommitSurface(s);
      break;
    }
    // handle input/events
  }
}
```

### GPU-backed rendering (sketch)

```c
RatSurface s; RatCreateSurface(1024, 768, RAT_PIXEL_FMT_RGBA16F, RAT_SURF_FLAG_GPU_BACKED, &s);
void* native; RatGetSurfaceNativeHandle(s, &native);
// create EGL/Vulkan image bound to native
// render into image and attach
RatAttachGpuImage(s, &desc);
RatCommitSurface(s);
```

---

## Incremental Implementation Plan

**Phase 0 — Prototype compositor (software)**

* Implement simple compositor using DRM/KMS (or an X11 window for development). Accept shared-memory surfaces, do software composition.
* Provide `RatCreateSurface`, `RatCommitSurface`, and a simple message queue + input routing.

**Phase 1 — GPU & polish**

* Add EGL/GBM or Vulkan GPU paths. Implement damage regions, vsync, and frame feedback.
* Add RatGDI-lite helper library for common 2D operations.

**Phase 2 — Wayland-host adapter**

* Implement a Wayland server adapter accepting `wl_surface` clients and mapping them to Rat surfaces.
* Test common Wayland clients for compatibility.

**Phase 3 — Toolkits & ecosystem**

* Provide toolkit bindings, examples, and migration guides for porting GTK/Qt apps to RatAPI if desired.

---

## Open Questions

* Do we expose direct access to GPU acceleration APIs (EGL/Vulkan) as part of RatAPI or keep them private and provide higher-level bindings? (default: expose native handle, but keep API minimal)
* What is the precise security model for handle transfer across processes? (Capability tokens vs ACL-only)
* Should we provide first-class compositing effects APIs (shadows, blur) or keep them compositor-internal? (default: compositor-internal; expose selective APIs later)

---

*RatOS Windowing & Graphics Specification — version 0.1-draft*