# ADR-0006: Windowing and Graphics System

---

## Status

Accepted

---

## Context

RatOS requires a unified, coherent, and flexible windowing and graphics system to support GUI application development. The system must:

* Provide a cohesive API (RatAPI) for creating, rendering, and interacting with windows.

* Be compatible with modern applications, while still offering familiar models (e.g., message loops like Win32).

* Support extensibility for advanced GUI features (compositing, effects, GPU acceleration, etc.).

* Maintain a developer experience that balances consistency (like Win32) and modernity (like Wayland).

Several options were considered:

* Custom Window Server (fork or reimplementation based on Wayland concepts).

* Wrapper over existing toolkits (GTK/Qt, etc.).

* Abstraction layer over Wayland (keeping RatAPI portable).

* Low-level GPU/Compositor API (like Win32 device contexts, but modernized).

---

## Decision

RatOS will implement a custom window server forked from Wayland, heavily adapted to RatOS requirements.

### Key design points

**Restrained Surfaces:** Each window is represented as a surface, managed via opaque handles or strongly-typed object references. These surfaces enforce security and resource ownership.

Message Loops and Async Events:

* Classic Win32-style message loops and WndProc functions (with switch-style message handling) will be supported for familiarity.

* A modern async event model will also be provided, for developers who prefer event-driven APIs without a central loop.

**No Device Contexts:** Unlike Win32, rendering will be performed through modern APIs (e.g., shared buffers, GPU-backed surfaces). Device contexts are avoided due to complexity and legacy limitations.

**Wayland Compatibility:** Forking Wayland ensures strong compatibility with existing Linux applications. However, RatOS will diverge significantly in order to provide a cohesive RatAPI rather than a thin wrapper.

**X11 Support:** Not included initially. May be added later via compatibility layers if needed.

This approach provides the control and extensibility necessary for RatOS to innovate while maintaining enough compatibility to support common Linux GUI applications.

---

## Alternatives Considered

* Wrapper over GTK/Qt: Rejected because it would make RatAPI feel like “just another toolkit,” without a unified system-level API.

* Pure Wayland Abstraction Layer: Rejected because it limits RatOS to Wayland’s design choices, constraining innovation.

* Device Contexts (Win32 model): Rejected due to unnecessary complexity, legacy baggage, and mismatch with modern GPU-based rendering pipelines.

* X11-based design: Rejected due to X11’s legacy issues and lack of modern compositing features.

---

## Consequences

### Positive:

* Unified API that balances familiarity (Win32) and modern design (Wayland).

* Long-term flexibility to introduce advanced GUI features unique to RatOS.

* Clean developer experience with both message-loop and async models.

### Negative:

* Requires significant development effort to fork and extend Wayland.

* Short-term risk of diverging too far from upstream Wayland, complicating maintenance.

* No immediate compatibility with legacy X11 applications.

---

## References

* Win32 API
* Wayland
* XWayland