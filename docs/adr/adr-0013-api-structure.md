# ADR-0013: RatAPI Structure and SDK Layout

## Status
Proposed

---

## Context

RatOS needs a consistent and discoverable application programming interface (RatAPI) to enable developers to write applications. The structure of the API and SDK will heavily influence developer experience, ease of learning, and maintainability. Historically, systems such as Win32 (windows.h) and POSIX (many headers) have taken different approaches. We must decide how RatAPI will be organized, named, and exposed to developers.

---

## Decision

RatAPI will be structured around functional domains, with one header file per domain, plus a unified umbrella header for convenience. The SDK will also provide dynamic/shared libraries corresponding to these domains.

### 1. Naming Convention

**Functions:** PascalCase, with Rat prefix (e.g. `RatCreateWindow`, `RatReadFile`, `RatCreateThread`).

**Types:** PascalCase, with Rat prefix (e.g. `RatHandle`, `RatStatus`, `RatString`).

**Constants** and Enums: Uppercase with RAT_ prefix (e.g. RAT_OK, RAT_ERR_ACCESS).

This ensures clarity, consistency, and strong association with RatOS.

### 2. Header Structure

- `ratcore.h` — fundamental types, error handling, memory management.

- `ratio.h` — file I/O, async I/O, networking.

- `ratgui.h` — windowing, drawing primitives, common controls.

- `ratproc.h` — processes, threads, synchronization, IPC.

- `ratsec.h` — security, authentication, cryptography.

- `ratinit.h` — init system interaction, system services.

- `rat.h` — umbrella header that includes all of the above.

Developers may include just what they need or use the single umbrella header.

3. Library Structure

Each header corresponds to a shared library:

- libratcore.so / libratcore.dll

- libratio.so / libratio.dll

- libratgui.so / libratgui.dll

- libratproc.so / libratproc.dll

- libratsec.so / libratsec.dll

- libratinit.so / libratinit.dll

And optionally a monolithic library (librat.so / librat.dll) that links them together for convenience.

4. SDK Layout

The SDK provided to developers will include:

- include/ — all RatAPI headers.

- lib/ — precompiled libraries.

- bin/ — tools such as the RatOS compiler toolchain wrappers, resource compilers, and debuggers.

- docs/ — reference documentation and samples.

### 5. Error Handling

All functions return a RatStatus (success/error code), with output values provided via pointers. This aligns with ADR-0010.

### 6. Future-Proofing

The structure allows:

Adding new domains via additional headers/libraries.

Backward compatibility via the umbrella header.

Optional high-level frameworks (e.g., GUI toolkit) without polluting RatAPI.

---

## Consequences

Developers can choose fine-grained or broad includes.

Consistent naming makes RatAPI self-descriptive.

RatOS presents itself as a modern, cohesive OS API, not just a patchwork of POSIX/Win32 calls.

Slightly higher maintenance burden, as headers and libraries must stay in sync.