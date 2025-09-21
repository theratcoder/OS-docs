# ADR-0001: Handle Types vs File Descriptors

## Status

Accepted

## Context

RatOS aims to provide a unified, developer-friendly API (RatAPI) for system services, similar to the Win32 API. A key question is how to represent system objects (files, threads, sockets, windows, etc.) in the API. Options include:

1. **Win32-like handles:** Opaque RatHandle for all objects.
2. **Linux-like primitives:** File descriptors, pthreads, POSIX primitives.
3. **Hybrid approach:** Externally expose handles to developers; internally use POSIX primitives for Linux compatibility.

Goals:

* Provide a consistent, easy-to-learn API for developers.
* Maintain compatibility with Linux for existing applications.
* Allow opaque, abstracted system objects that can cover a variety of subsystems.

## Decision

We will adopt the **hybrid approach**, with the following characteristics:

* **Developer-facing API:** Different handle types for different object categories (inspired by Win32):

  * `HWND` for windows
  * `HINSTANCE` for processes or apps
  * `HTHREAD` for threads
  * `HFILE` for files
  * `HMQ` for message queues / IPC
* **Internal implementation:** Map each handle type to an appropriate Linux primitive (e.g., FDs, pthread\_t, mqd\_t, internal pointers).
* **Opaque handles:** Developers will not see the underlying type, ensuring abstraction and flexibility for future subsystems.

### Example Usage

```c
HFILE file = RatOpenFile("data.txt", RAT_FILE_READ);
HTHREAD thread = RatCreateThread(MyThreadFunc, arg);
HMQ mq = RatOpenMessageQueue("myqueue");
HWND window = RatCreateWindow("Title", 800, 600);
```

Internally, the runtime maps each handle to the corresponding Linux construct.

## Alternatives

### 1. Pure Win32-like (`RatHandle` for everything)

* Pros: Consistent API, simpler externally.
* Cons: Internally less compatible with Linux; harder to port existing software.

### 2. Pure Linux-like (file descriptors, pthreads, etc.)

* Pros: Directly compatible with Linux, minimal runtime layer.
* Cons: Inconsistent API; not as friendly for developers; harder to unify across subsystems.

## Consequences

* **Positive:**

  * Unified RatAPI for developers, consistent naming and usage.
  * Internals remain Linux-compatible, easing porting of existing applications.
  * Future subsystems (graphics, networking, IPC) can use the same handle-based pattern.
* **Negative:**

  * Requires a lightweight mapping layer between handles and internal primitives.
  * Some complexity in managing distinct handle types.

## References

* Win32 API handle conventions
* Linux POSIX APIs (file descriptors, pthreads, IPC)