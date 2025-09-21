# RatOS Specification: Inter-Process Communication (IPC) & Messaging

## Overview

This specification defines RatOS IPC primitives and the **message-loop** abstraction inspired by Win32. RatAPI exposes low-level primitives (pipes, sockets, shared memory, events) and a higher-level message queue / message-loop API that app developers can use to write event-driven programs.

Design goals:

* Provide primitives familiar to POSIX/Linux developers.
* Provide a friendly message-loop model for application developers.
* Keep all IPC objects consistent with RatAPI handle and error conventions.
* Ensure security, ownership, and lifetime semantics are clearly defined.

---

## Key Types

```c
typedef uint64_t RatHandle;          // generic opaque handle
typedef RatHandle RatMessageQueue;   // message-queue handle
typedef RatHandle RatSharedMemory;   // shared-memory handle
typedef RatHandle RatPipe;           // pipe handle
typedef RatHandle RatSocket;         // socket handle
typedef RatHandle RatEvent;          // event handle

typedef enum {
  RAT_IPC_OK = 0,
  RAT_IPC_ERR_UNKNOWN = 1,
  RAT_IPC_ERR_NOT_FOUND = 2,
  RAT_IPC_ERR_ACCESS_DENIED = 3,
  RAT_IPC_ERR_NO_RESOURCES = 4,
  RAT_IPC_ERR_INVALID_ARG = 5,
  RAT_IPC_ERR_ALREADY_EXISTS = 6,
  RAT_IPC_ERR_NOT_CONNECTED = 7,
  RAT_IPC_ERR_BUFFER_TOO_SMALL = 8,
  RAT_IPC_ERR_TIMEOUT = 9,
} RatIpcStatus;

// Basic message structure used by the message-loop API
typedef struct {
  uint32_t message;     // message id
  uintptr_t wparam;     // optional word-sized parameter
  intptr_t lparam;      // optional word-sized parameter
  RatHandle sender;     // handle of sender (may be RAT_INVALID_HANDLE)
  uint32_t size;        // size of payload in bytes (0 if none)
} RatMessageHeader;
```

A message may optionally carry a payload (binary blob). For small control messages, `wparam/lparam` suffice. `RatHandle` is opaque and is compatible with other RatAPI handle types.

---

## Design Layers & Implementation Notes

* **Local primitives** (pipes, sockets, shared memory, events) are thin wrappers around kernel primitives (AF\_UNIX sockets, `shm_open`/`mmap`, `pipe()`, futex/condvar) or user-space implementations when necessary.
* **Message queues** are a RatAPI concept. They may be implemented in-process (fast, in-memory) or system-wide for named queues (backed by an agent/daemon, UNIX domain sockets, or POSIX message queues). The spec is intentionally **implementation-agnostic**; the API guarantees behavior and security semantics rather than a specific kernel primitive.
* **Cross-process messaging** will be supported for named queues and for window/message-target semantics (see Message Loop section).

---

## Primitives API (short)

### Pipes (byte-stream)

```c
RatIpcStatus RatCreatePipe(RatPipe* outRead, RatPipe* outWrite); // local anonymous pipe
RatIpcStatus RatReadPipe(RatPipe pipe, void* buf, size_t size, size_t* read);
RatIpcStatus RatWritePipe(RatPipe pipe, const void* buf, size_t size, size_t* written);
RatIpcStatus RatClosePipe(RatPipe pipe);
```

Notes:

* Implemented on top of `pipe()`/`socketpair()`; supports blocking and non-blocking modes via flags on creation or `RatSetHandleFlags`.

### Sockets (local/network)

```c
RatIpcStatus RatCreateSocket(int domain, int type, RatSocket* out);
RatIpcStatus RatBindSocket(RatSocket sock, const void* addr, size_t addrlen);
RatIpcStatus RatListenSocket(RatSocket sock, int backlog);
RatIpcStatus RatAcceptSocket(RatSocket sock, RatSocket* outClient);
RatIpcStatus RatConnectSocket(RatSocket sock, const void* addr, size_t addrlen);
RatIpcStatus RatSendSocket(RatSocket sock, const void* buf, size_t len, size_t* sent);
RatIpcStatus RatRecvSocket(RatSocket sock, void* buf, size_t len, size_t* recvd);
RatIpcStatus RatCloseSocket(RatSocket sock);
```

Notes:

* `domain`/`type` mirror POSIX (`AF_UNIX`, `AF_INET`, `SOCK_STREAM`, `SOCK_DGRAM`) to ease porting.
* Sockets integrate with the async completion-queue API (`specs/async-io.md`) for non-blocking I/O.

### Shared Memory

```c
RatIpcStatus RatCreateSharedMemory(const char* name, size_t size, RatSharedMemory* out);
RatIpcStatus RatOpenSharedMemory(const char* name, RatSharedMemory* out);
RatIpcStatus RatMapSharedMemory(RatSharedMemory shm, void** outAddr, size_t* outSize);
RatIpcStatus RatUnmapSharedMemory(void* addr, size_t size);
RatIpcStatus RatCloseSharedMemory(RatSharedMemory shm);
```

Notes:

* `RatCreateSharedMemory` creates a named shared object; `RatOpenSharedMemory` connects to an existing one.
* Mapping yields a pointer in the caller’s address space; synchronization is the caller’s responsibility (use `RatMutex` / `RatEvent`).

### Events (notifications)

```c
RatIpcStatus RatCreateEvent(bool manualReset, bool initialState, RatEvent* out);
RatIpcStatus RatSetEvent(RatEvent ev);
RatIpcStatus RatResetEvent(RatEvent ev);
RatIpcStatus RatCloseEvent(RatEvent ev);
```

Notes:

* Events are waitable via the standard `RatWaitForObject` APIs defined in `specs/threading-and-sync.md`.

---

## Message-Queue / Message-Loop API

This is the higher-level, Win32-style API intended for application message loops.

### Creating & Opening Queues

```c
// Create an unnamed in-process queue
RatIpcStatus RatCreateMessageQueue(size_t capacity, RatMessageQueue* out);

// Create or open a named system queue (visible to other processes)
RatIpcStatus RatCreateNamedMessageQueue(const char* name, size_t capacity, RatMessageQueue* out);
RatIpcStatus RatOpenNamedMessageQueue(const char* name, RatMessageQueue* out);

RatIpcStatus RatCloseMessageQueue(RatMessageQueue q);
```

Notes:

* Named queues are subject to RatOS ACLs and user permissions; the creator may specify access rights on creation.
* Unnamed queues are local to the creating process and are the fastest option for GUI/event loops.

### Message Structure & Payloads

```c
typedef struct {
  RatMessageHeader header;      // header (id, wparam, lparam, sender, size)
  void* payload;                // optional pointer to payload (opaque to API)
} RatMessage;
```

* If `header.size == 0`, `payload` is ignored.
* For large payloads across processes, recommend using shared memory handles and passing the handle/offset in `wparam/lparam` or in the payload header.

### Sending & Posting Messages

```c
RatIpcStatus RatPostMessage(RatMessageQueue target, const RatMessage* msg);
RatIpcStatus RatSendMessage(RatMessageQueue target, const RatMessage* msg, RatMessage* outReply, uint32_t timeout_ms);
```

* `RatPostMessage` is asynchronous — it enqueues the message and returns quickly. The sender does not wait for delivery.
* `RatSendMessage` is synchronous — it blocks or waits until a reply is posted (useful for RPC-style interaction). `outReply` may be NULL if sender does not expect a reply.

### Retrieving Messages (message-loop)

```c
RatIpcStatus RatGetMessage(RatMessageQueue q, RatMessage* outMsg, int32_t timeout_ms);
RatIpcStatus RatPeekMessage(RatMessageQueue q, RatMessage* outMsg); // non-blocking
RatIpcStatus RatDispatchMessage(const RatMessage* msg); // helper to dispatch to window/proc
```

* `RatGetMessage` blocks up to `timeout_ms` milliseconds; negative timeout means infinite wait.
* Message queues integrate with the RatWait APIs; `RatWaitForObject` on a message queue handle succeeds when messages are available.

### Integrating with Windowing

* Window handles (if/when implemented) are valid message targets. Posting to a window routes the message to the owning process’s queue and also supplies window-specific routing information.

---

## Security & Access Control

* Named IPC objects (named message queues, named shared memory) are created with **ACLs** describing which UIDs/GIDs or capabilities may open/read/write.
* By default, named objects are created private to the creating user unless an explicit ACL grants broader access.
* Message payloads are not implicitly trusted — receivers must validate data. Passing handles (e.g., shared memory handles) across processes should verify that the receiving process is authorized.

---

## Lifetime & Ownership

* IPC handles are reference-counted. `RatClose*` APIs decrement the reference and free resources when no references remain.
* Named objects persist until explicitly unlinked or until the last reference is dropped and the creator decides lifecycle policies (configurable at creation).
* Shared memory should be explicitly unlinked by the owner when appropriate to avoid resource leakage.

---

## Error Handling

* IPC APIs return `RatIpcStatus` values (or the common `RatStatus` alias if preferred). Map platform `errno` values to `RatIpcStatus` for consistent behavior.
* For message-oriented APIs, common errors include `RAT_IPC_ERR_BUFFER_TOO_SMALL`, `RAT_IPC_ERR_NO_RESOURCES`, and `RAT_IPC_ERR_ACCESS_DENIED`.

---

## Examples

### Simple message loop (in-process)

```c
RatMessageQueue mq;
RatCreateMessageQueue(256, &mq);

for (;;) {
  RatMessage msg;
  if (RatGetMessage(mq, &msg, -1) != RAT_IPC_OK) break;
  switch (msg.header.message) {
    case 1:
      // handle
      break;
    case 2:
      // handle
      break;
  }
}
RatCloseMessageQueue(mq);
```

### Cross-process post with shared memory payload

```c
// Sender
RatSharedMemory shm; void* addr; size_t size = 1 << 20;
RatCreateSharedMemory("/rat_large", size, &shm);
RatMapSharedMemory(shm, &addr, &size);
// fill data in addr
RatMessage msg = { .header = { .message = 100, .wparam = (uintptr_t)shm, .lparam = 0, .size = 0 }, .payload = NULL };
RatPostMessage(targetQueue, &msg);
RatUnmapSharedMemory(addr, size);
RatCloseSharedMemory(shm);
```

---

## Integration Points & Future Work

* **Integration with async I/O:** posting/receiving messages should be usable from completion queues and integrate with the RatOS event loop model.
* **RPC framework:** build higher-level RPC semantics on top of `RatSendMessage` (deferred).
* **Message filtering/priorities:** consider priority queues or filtering in the future.
* **Cross-origin handle transfer:** define secure ways to transfer handles between processes (capability tokens, explicit accept).

---

*RatOS IPC & Messaging Specification — version 0.1-draft*