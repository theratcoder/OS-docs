# RatOS SDK Specification

## Overview

The RatOS Software Development Kit (SDK) provides the tools, headers, libraries, and documentation required to develop applications for RatOS. Its design balances simplicity, consistency, and extensibility. The SDK allows developers to build applications natively on RatOS or cross-compile from other platforms.

RatAPI, the core application programming interface, is organized into functional domains with consistent naming conventions and modular headers. The SDK also provides build tools, resource compilers, and debugging utilities.

---

## Naming Conventions

Functions: PascalCase, prefixed with Rat (e.g., `RatCreateWindow`, `RatOpenFile`).

Types: PascalCase, prefixed with Rat (e.g., `RatHandle`, `RatStatus`).

Constants and Enums: Uppercase with RAT_ prefix (e.g., `RAT_OK`, `RAT_ERR_INVALIDARG`).

This ensures a clear and discoverable API surface.

---

## Header Structure

Headers are organized by functional domain.

- ratcore.h — fundamental types, error handling, memory management.

- ratio.h — file I/O, async I/O, networking.

- ratgui.h — windowing, drawing primitives, common controls.

- ratproc.h — processes, threads, synchronization, IPC.

- ratsec.h — security, authentication, cryptography.

- ratinit.h — init system interaction, system services.

- rat.h — umbrella header that includes all of the above.

- Developers may include individual headers for minimal builds or rat.h for simplicity.

---

## Library Structure

Each header corresponds to a shared library:

- libratcore

- libratio

- libratgui

- libratproc

- libratsec

- libratinit

For convenience, RatOS also provides:

- librat — a monolithic library linking all modules together.

Dynamic linking is encouraged, but static libraries (.a/.lib) will also be available for special cases.

5. SDK Layout

A typical SDK installation will have the following structure:
```
/ratos-sdk/
  ├── include/
  │    ├── ratcore.h
  │    ├── ratio.h
  │    ├── ratgui.h
  │    ├── ratproc.h
  │    ├── ratsec.h
  │    ├── ratinit.h
  │    └── rat.h
  ├── lib/
  │    ├── libratcore.so/.dll
  │    ├── libratio.so/.dll
  │    ├── libratgui.so/.dll
  │    ├── libratproc.so/.dll
  │    ├── libratsec.so/.dll
  │    ├── libratinit.so/.dll
  │    └── librat.so/.dll
  ├── bin/
  │    ├── ratcc   (compiler frontend / wrapper)
  │    ├── ratld   (linker wrapper)
  │    ├── ratres  (resource compiler)
  │    ├── ratdbg  (debugger)
  │    └── ratsdk  (metadata / SDK management tool)
  └── docs/
       ├── api/
       ├── tutorials/
       └── samples/
```

---

## Toolchain

The SDK provides wrappers around existing compiler and linker toolchains, abstracting platform differences.

- **Compiler (ratcc):** front-end driver (may wrap GCC/Clang initially).

- **Linker (ratld):** links RatAPI libraries automatically.

- **Resource Compiler (ratres):** handles GUI resource definitions (dialogs, menus, icons).

- **Debugger (ratdbg):** RatOS debugging front-end (may wrap GDB or LLDB).

Over time, RatOS may provide a dedicated compiler suite for deeper integration.

## Error Handling

All RatAPI functions return a RatStatus indicating success or error.

Output values are written to pointers supplied by the caller.

RAT_OK is used to indicate success.

This policy ensures clarity and avoids ambiguity (see ADR-0010).

## Cross-Compilation

Cross-compilation will be supported from Linux, macOS, and Windows.

The SDK provides cross-compiler toolchains where feasible.

Developers can build RatOS applications before a native development environment is fully mature.

## Documentation

The SDK includes:

- **API Reference:** auto-generated from RatAPI headers.

- **Guides:** tutorials on building, packaging, and distributing apps.

- **Samples:** minimal working examples for core RatAPI domains.

Documentation will be distributed both online and offline.

---

## Future Extensions

**IDE Integration:** Official RatOS IDE in the style of Visual Studio.

**Package Templates:** Standardized project structures.

**High-Level Frameworks:** Optional GUI toolkit or application frameworks.

---

*RatOS SDK — version 0.1-draft*