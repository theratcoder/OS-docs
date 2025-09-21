# RatOS Overview

## 🎯 Philosophy & Goals

RatOS aims to simplify application development on a Linux foundation by exposing a **Win32-inspired API** (RatAPI). Instead of requiring developers to directly work with POSIX and Linux-specific quirks, RatOS offers a **consistent, handle-based API** that balances ease-of-use with power.

**Core goals:**

* **Developer-friendly**: provide a single, well-documented API surface.
* **Consistent abstractions**: hide underlying Linux inconsistencies.
* **Modern, but pragmatic**: inspired by Win32 simplicity, improved with modern design.
* **Stable ABI**: allow RatOS applications to remain compatible across OS versions.
* **Extensible**: language-neutral base API (C) with bindings for Rust, Go, and others.

---

## 👥 Target Audience

* **Hobbyist OS developers** who want to build apps without reinventing the stack.
* **App developers** seeking a friendlier interface than raw POSIX.
* **Game developers** who want predictable performance and straightforward window/input APIs.
* **Systems programmers** looking for a clean, documented environment to experiment in.

---

## 🏗️ High-Level Architecture

RatOS builds on Linux but provides its own user-facing API and runtime environment:

```
+-------------------------+
|     Applications        |  <-- Developers target RatAPI
+-------------------------+
|         RatAPI          |  <-- Win32-inspired system API (C-based)
+-------------------------+
|   RatOS Services/Daemons|  <-- Window server, IPC, security, etc.
+-------------------------+
|       Linux Kernel      |
+-------------------------+
```

* **Linux Kernel**: Handles low-level resource management.
* **RatOS Services**: user-space daemons for windowing, IPC, networking, packaging.
* **RatAPI Runtime**: library exposing consistent, handle-based APIs.
* **Applications**: written against RatAPI, portable across RatOS versions.

---

## ✨ Key Differentiators

* **Handle-based API** instead of raw file descriptors.
* **Unified error model** (`RAT_STATUS` codes instead of `errno`).
* **Stable headers & ABI versioning** for long-term compatibility.
* **Optional windowing system** with RatOS-native event loop & GUI primitives.
* **Simplified IPC** that hides Linux’s multiple mechanisms (sockets, pipes, shared memory) behind one API.

---

## 📚 Glossary (short list)

* **RatAPI**: RatOS’s user-facing system API.
* **Handle**: opaque identifier for OS-managed resources (process, thread, file, socket, etc.).
* **ADR**: Architectural Decision Record, documents major design choices.
* **Spec**: A detailed description of a subsystem’s design and API.

---

## 🔮 Future Directions

* **GUI Toolkit**: RatOS-native windowing system for modern UIs.
* **RatPkg**: package format for distributing RatOS apps.
* **Security Model**: simplified but powerful permissions (inspired by ACLs).
* **Developer Tools**: build system, debugging, profiling integrated with RatAPI.

---

*RatOS Overview — version 0.1-draft*