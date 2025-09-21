# RatOS Specification: Asynchronous I/O (Async I/O)

## Overview

This document specifies RatOS's asynchronous I/O subsystem and the **completion-queue** model exposed to applications. It defines semantics for submission, completion, cancellation, ordering, resource limits, backend mappings (io\_uring vs epoll/threadpool), and required safety guarantees (buffer lifetime, thread-safety).

Async I/O is a first-class capability in RatOS intended for servers, high-performance apps, and GUI systems that need low-latency, scalable I/O without blocking threads.

---

## Core Concepts

* **Submission**: A request issued by a process to perform I/O (read, write, accept, connect, etc.). Submissions are associated with a `userTag` supplied by the caller.

* **Completion**: A record indicating the outcome of a submission. Completions are delivered through a **completion queue** (`HCOMPQ`) and contain a status, bytes transferred, and the original `userTag`.

* **Completion Queue (`HCOMPQ`)**: An OS-managed object that aggregates completions. Applications create and poll/wait on completion queues. A completion queue may be used for I/O on many handles.

* **Backend**: The kernel mechanism used to implement async behavior. Primary backend is `io_uring` where available; fallback is `epoll` (or kqueue on BSD/macOS) plus a worker threadpool for operations that cannot be made non-blocking.

---

## Public Types

```c
typedef uint64_t HCOMPQ;

typedef enum {
  RAT_OK = 0,
  RAT_ERR_UNKNOWN = 1,
  RAT_ERR_CANCELLED = 2,
  RAT_ERR_NO_RESOURCES = 3,
  RAT_ERR_IO = 4,
  RAT_ERR_TIMEOUT = 5,
  RAT_ERR_INVALID_ARG = 6,
  RAT_ERR_NOT_APPLICABLE = 7,
} RAT_STATUS;

typedef struct {
  uint64_t userTag;        // user-supplied identifier
  RAT_STATUS status;       // logical status of the operation
  size_t bytesTransferred; // number of bytes read/written, 0 if N/A
  uint64_t misc;           // backend-specific or op-specific value (e.g., new client handle)
} RatCompletion;
```

---

## Core API

```c
// Completion queue lifecycle
RAT_STATUS RatCreateCompletionQueue(size_t depth, HCOMPQ* out);
RAT_STATUS RatCloseCompletionQueue(HCOMPQ q);

// Submissions (examples)
RAT_STATUS RatQueueRead(HCOMPQ q, HFILE file, void* buf, size_t len, uint64_t userTag);
RAT_STATUS RatQueueWrite(HCOMPQ q, HFILE file, const void* buf, size_t len, uint64_t userTag);
RAT_STATUS RatQueueAccept(HCOMPQ q, HSOCKET listener, uint64_t userTag);
RAT_STATUS RatQueueConnect(HCOMPQ q, HSOCKET sock, const RatSockAddr* addr, uint64_t userTag);

// Cancellation
RAT_STATUS RatCancelSubmission(HCOMPQ q, uint64_t userTag);

// Completion retrieval
RAT_STATUS RatGetCompletion(HCOMPQ q, RatCompletion* out, int32_t timeout_ms);

// Query queue state
RAT_STATUS RatQueryCompletionQueue(HCOMPQ q, size_t* out_capacity, size_t* out_outstanding);
```

Notes:

* Submissions return quickly; they may fail synchronously for invalid arguments or resource exhaustion.
* `userTag` is opaque to the kernel; RatOS returns it with the completion so callers can correlate events.

---

## Semantics & Guarantees

### Ordering

* **Per-handle ordering**: For submissions targeting the same handle, operations that logically serialize (e.g., reads/writes using the file offset) are **observed in submission order** unless explicit offsets are provided by the caller that alter ordering.
* **Cross-handle ordering**: No ordering guarantees between submissions targeting different handles.
* **Completion ordering**: Completions may be delivered out of order relative to submission across multiple handles and even across multiple submissions to the same handle when the backend executes operations concurrently. Callers must use `userTag` to correlate completions.

### Buffer lifetime

* Callers **must** ensure the memory buffer passed to a submission remains allocated and unchanged until the corresponding completion has been retrieved. Use of stack-allocated buffers that go out of scope before completion leads to undefined behavior.

### Cancellation

* `RatCancelSubmission` is **best-effort**. Possible outcomes:

  * If the submission is still pending (not yet dispatched to kernel or worker), it will be cancelled and a completion with `RAT_ERR_CANCELLED` will be posted.
  * If the submission is already in-flight and the backend/kernel supports cancellation, the operation may be cancelled and completion with `RAT_ERR_CANCELLED` posted.
  * If the submission cannot be cancelled (already completed or kernel does not support cancellation), the eventual completion reflects the real outcome.
* `RatCancelSubmission` returns `RAT_OK` if the cancel request was accepted (even if cancellation later fails); it returns `RAT_ERR_INVALID_ARG` if the tag was not found on the queue.

### Timeouts

* `RatGetCompletion` accepts a timeout in milliseconds. Negative timeout means infinite wait.
* Submissions themselves do not have an intrinsic timeout unless the API provides timed variants (e.g., `RatQueueReadTimed`). Implementations should not implicitly timeout submissions.

### Partial completions

* If an operation completes partially (e.g., short read), the completion should indicate `RAT_OK` with `bytesTransferred` set to the actual bytes read. If a partial transfer occurred due to an error, `status` should reflect the error.

### Resource limits

* `depth` in `RatCreateCompletionQueue` is a hint for expected outstanding submissions. Implementations may enforce limits and cause submissions to fail with `RAT_ERR_NO_RESOURCES`.
* `RatQueryCompletionQueue` allows callers to observe capacity and outstanding counts.

---

## Backend mapping

### Primary: `io_uring`

* When available, map submissions to `io_uring` SQEs and completions to CQEs.
* Use appropriate ops: `IORING_OP_READV`, `IORING_OP_WRITEV`, `IORING_OP_ACCEPT`, `IORING_OP_CONNECT`, etc.
* Translate `io_uring` result codes to `RAT_STATUS`.

### Fallback: `epoll` + worker threadpool

* For sockets: use non-blocking sockets with `epoll` readiness notifications; perform non-blocking `read`/`write` when ready.
* For regular files: use a small worker threadpool that performs blocking `pread`/`pwrite` calls and posts completions to the queue.
* The worker threadpool should respect queue affinity and fairness to avoid starvation.

### Other platforms

* On platforms without `epoll` (e.g., BSD/macOS), use the native readiness API (`kqueue`) with similar semantics.

---

## Error mapping

Convert common kernel/errno values to `RAT_STATUS`:

* `ENOENT` -> `RAT_ERR_INVALID_ARG` or `RAT_ERR_NOT_FOUND` (context-specific)
* `EACCES` -> `RAT_ERR_ACCESS_DENIED`
* `EINTR` -> `RAT_ERR_IO` (or a specific interrupted status)
* `ETIMEDOUT` -> `RAT_ERR_TIMEOUT`
* `ECONNREFUSED`, `ECONNRESET` -> `RAT_ERR_IO` (with extended codes available via `RatGetLastErrorDetail`)
* `ENOMEM` -> `RAT_ERR_NO_RESOURCES`

Implementations should preserve the underlying errno for diagnostic purposes via `RatGetLastErrno()` or an extended error detail API.

---

## Security considerations

* Completions or submission metadata must not leak kernel pointers or sensitive kernel memory.
* Access control: Queues and submissions respect process credentials. For cross-process completion queues (if supported), only authorized processes can create/read completions.
* Buffer contents passed to I/O should be validated by privileged subsystems when crossing privilege boundaries.

---

## Edge Cases & Notes

* **Duplicate userTag values**: Using the same `userTag` for multiple in-flight submissions is allowed but makes correlation ambiguous. Callers should prefer unique tags per submission.
* **Queue closure with outstanding submissions**: Closing a queue with outstanding submissions should attempt to cancel them and post completion entries marked as failed (e.g., `RAT_ERR_CANCELLED` or `RAT_ERR_IO`). `RatCloseCompletionQueue` may block until outstanding completions are posted or return an error (implementation choice — document behaviour).
* **Scatter/gather I/O**: Supported via vectorized submission APIs (`RatQueueReadV`, `RatQueueWriteV`) where the backend supports it.

---

## Examples

### Simple async read

```c
HCOMPQ q; RatCreateCompletionQueue(64, &q);
HFILE f = RatOpenFile("/var/log/big.log", RAT_FILE_READ, 0, NULL);
char* buf = malloc(65536);
RatQueueRead(q, f, buf, 65536, 0x1001);

RatCompletion c;
RatGetCompletion(q, &c, 5000);
if (c.status == RAT_OK) {
  // use buf[0..c.bytesTransferred-1]
}
```

### Accept on a listening socket

```c
HCOMPQ q; RatCreateCompletionQueue(256, &q);
HSOCKET listener = RatSocket(RAT_AF_INET, RAT_SOCK_STREAM, 0);
RatBind(listener, &addr);
RatListen(listener, 128);
RatQueueAccept(q, listener, 0xBEEF);

RatCompletion c;
RatGetCompletion(q, &c, -1);
if (c.status == RAT_OK) {
  HSOCKET client = (HSOCKET)c.misc; // new client handle returned in misc
}
```

---

## Open Questions / Future Work

* Should RatOS support shared completion queues across processes? If so, define precise security and ownership semantics.
* Decide whether `RatCloseCompletionQueue` blocks until outstanding submissions complete or returns immediately with a forced cancellation behavior.
* Design extended error detail API (`RatGetLastErrorDetail`) for richer diagnostics.

---

*RatOS Async I/O Specification — version 0.1-draft*