# Architecture

This document provides a **high-level view of RatOS architecture**, describing the major layers, their responsibilities, and how they interact. The goal is to give contributors and developers a shared understanding of how the system fits together.

---

## 🏛 Design Principles

* **Simplicity for developers** — The RatAPI provides a friendlier, higher-level interface than raw POSIX calls.
* **Compatibility with Linux** — Built on the Linux kernel, ensuring hardware support and stability.
* **Separation of concerns** — Clear boundaries between kernel, RatOS core services, and user applications.
* **Extensibility** — Designed to evolve without breaking existing APIs.

---

## 📚 Layered Architecture

### 1. Hardware Layer

* Physical CPU, memory, storage, and I/O devices.
* Supported via Linux kernel drivers.

### 2. Kernel Layer (Linux)

* Process scheduling and management
* Memory management
* Device drivers
* Filesystem and networking primitives

### 3. RatOS Core Services

* Runs in **user space**, providing the RatAPI abstraction layer.
* Components include:

  * **Process & Thread Manager** — Higher-level API for process/thread creation, handles, and synchronization.
  * **File & I/O API** — Uniform interface for files, sockets, and devices.
  * **Memory API** — Simplified memory allocation and mapping.
  * **IPC (Inter-Process Communication)** — RatAPI-level messaging and shared memory abstractions.
  * **UI/Graphics Subsystem (future)** — Optional higher-level API for windows, events, and drawing.

### 4. RatAPI Layer

* The Win32-esque interface for developers.
* Provides functions like `RatCreateProcess`, `RatOpenFile`, `RatSendMessage`.
* Backed by RatOS Core Services, which in turn call Linux syscalls.

### 5. User Applications

* Applications developed directly against RatAPI.
* Can also run **POSIX-compliant applications** (via compatibility layer) to ease porting.

---

## 🔀 Data & Control Flow

1. **Application** calls a RatAPI function (e.g., open a file).
2. **RatAPI library** translates this into a request to RatOS Core Services.
3. **Core Service** either:

   * Handles it internally (e.g., handle bookkeeping), or
   * Passes it down to the **Linux kernel** via syscalls.
4. **Kernel** interacts with hardware or system resources.
5. Results propagate back up the stack to the application.

---

## 🧩 Key Differentiators vs Linux Distros

* RatAPI sits as a **stable, simplified abstraction layer** above Linux.
* Encourages **new software development** instead of direct reliance on POSIX.
* Long-term plan to add **first-class GUI and service APIs**.

---

*RatOS Architecture — version 0.1-draft*