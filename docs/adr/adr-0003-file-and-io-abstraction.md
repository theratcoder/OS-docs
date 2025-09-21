# ADR-0003: File and I/O Abstraction

## Status

Accepted

## Context

RatOS requires a unified, consistent API for handling all forms of input and output, ranging from files and devices to sockets and pipes. The design should:

* Support **Linux compatibility** internally, ensuring existing software can be ported with minimal effort.
* Provide a **simplified, Win32-style abstraction** via handles, so developers only need to learn one system API (RatAPI).
* Accommodate advanced features like asynchronous I/O, event notifications, and a virtual file system (VFS).

Three primary options were considered:

1. **POSIX passthrough** — expose file descriptors directly via RatAPI.
2. **Pure Win32-style handles** — all I/O operations use opaque handles, with no direct POSIX compatibility.
3. **Hybrid system** — expose a Win32-style handle-based API, while internally mapping to Linux file descriptors or other system primitives.

## Decision

RatOS will adopt a **hybrid handle-based I/O system**:

* Every file, device, socket, or pipe will be represented by a **RatOS handle type**, e.g. `HFILE`, `HSOCKET`, `HPIPE`.
* Internally, these map to Linux **file descriptors** or other kernel-level primitives, ensuring compatibility.
* RatAPI will provide a consistent set of functions for opening, reading, writing, seeking, and closing handles.
* Extended functionality (async I/O, event-driven I/O, etc.) will be exposed via RatAPI extensions.
* A virtual file system layer will allow RatOS-specific resources (e.g. registry-like configuration storage, pseudo-devices) to be exposed uniformly.

### Example API Surface

```c
// Open a file and return a handle
HFILE RatCreateFile(const char* path, RatAccess access, RatShareMode share);

// Read and write data
bool RatReadFile(HFILE file, void* buffer, size_t size, size_t* bytesRead);
bool RatWriteFile(HFILE file, const void* buffer, size_t size, size_t* bytesWritten);

// Close a file
bool RatCloseHandle(HFILE file);

// Asynchronous variant with callback/event support
bool RatReadFileAsync(HFILE file, void* buffer, size_t size, RatIOCompletion* completion);
```

### Device Abstraction

* **Sockets** → `HSOCKET` with wrappers for TCP/UDP.
* **Pipes** → `HPIPE` for IPC and anonymous pipes.
* **Devices** → Exposed as `HDEVICE`, unified under the RatAPI model.

## Alternatives

1. **POSIX passthrough**: Simple, but breaks the RatAPI design goal of providing a single unified abstraction.
2. **Pure Win32-style**: Maximizes abstraction, but risks poor Linux compatibility and higher implementation complexity.

## Consequences

* **Positive**:

  * Provides a unified, developer-friendly I/O model.
  * Ensures Linux compatibility internally.
  * Allows RatAPI to evolve beyond POSIX limitations (async I/O, completion ports, etc.).
* **Negative**:

  * Adds an abstraction layer, with potential performance overhead.
  * Requires careful mapping between RatAPI handles and Linux descriptors.

## References

* ADR-0001: Handle System for RatOS
* ADR-0002: Memory Management Model