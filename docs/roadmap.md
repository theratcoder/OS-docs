# RatOS Roadmap

This roadmap outlines short-term, medium-term, and long-term goals for RatOS. It is a **living document**, updated as design decisions are made and milestones are reached.

---

## 🐀 Versioning Plan

* **v0.1 (Prototype)**: minimal API + hello world apps.
* **v0.5 (Developer Preview)**: usable RatAPI, basic IPC, experimental windowing.
* **v1.0 (Stable)**: production-ready release with ABI stability guarantees.

---

## 🎯 Short-Term Goals (v0.1)

* [ ] Establish **API conventions** (handles, error model, naming).
* [ ] Implement **basic process & thread management** (`RatCreateProcess`, `RatCreateThread`, `RatWaitForSingleObject`).
* [ ] Provide **file I/O wrapper API** (`RatOpenFile`, `RatReadFile`, `RatWriteFile`).
* [ ] Define **error/status codes** (`RAT_STATUS`).
* [ ] Draft **ADR 0001: Handles vs File Descriptors**.
* [ ] Example programs: `hello_world.c`, file I/O demo.

---

## ⚙️ Medium-Term Goals (v0.5)

* [ ] Expand process/thread APIs: synchronization primitives (mutexes, semaphores, events).
* [ ] Introduce **IPC model** (message queues, shared memory).
* [ ] Introduce **networking API** (RatSocket-like abstraction or POSIX wrapper).
* [ ] Design and prototype **window server**:

  * Decision: build on Wayland vs. custom compositor.
  * Implement **basic event loop + window creation API**.
* [ ] Packaging format draft (`.ratpkg`).
* [ ] Provide **language bindings** (Rust, Go experimental).

---

## 🚀 Long-Term Goals (v1.0 and beyond)

* [ ] Finalize **windowing system** with UI toolkit.
* [ ] Stabilize **RatPkg package system** with signing & permissions model.
* [ ] Define and enforce **security model** (capabilities, ACL-like permissions).
* [ ] RatOS **App Store** concept (distribution + sandboxing).
* [ ] Developer tooling: debugger, profiler, RatAPI-aware IDE plugins.
* [ ] Long-term **binary compatibility promise** across RatOS versions.

---

## 🔄 Process

* All major decisions tracked in **ADRs** (`/docs/adr/`).
* Roadmap items may shift depending on ADR outcomes.
* Community proposals welcome via PRs to this document.

---

*RatOS Roadmap — version 0.1-draft*