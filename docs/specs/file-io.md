# specs/file-io.md

## Purpose & scope

This spec describes the RatOS File & I/O subsystem API surface, types, flags, lifecycle rules, error mapping, and examples. It covers synchronous and asynchronous file I/O, basic metadata operations, file locking, and interop helpers. The asynchronous I/O design builds on the **completion-queue model** defined in `specs/async-io.md`.

---

## Terminology

* **RatHandle / HFILE / HSOCKET:** typed opaque handles returned by RatAPI for resources.
* **Completion queue (HCOMPQ):** handle representing a submission/completion queue for async I/O.
* **Submission:** request to perform I/O (read, write, accept, connect).
* **Completion:** outcome of a submission, with status and user tag.

---

## Public Types & Enums

```c
typedef uint64_t HFILE;
typedef uint64_t HSOCKET;
typedef uint64_t HCOMPQ;

typedef enum {
  RAT_OK = 0,
  RAT_ERR_UNKNOWN = 1,
  RAT_ERR_NOT_FOUND = 2,
  RAT_ERR_ACCESS_DENIED = 3,
  RAT_ERR_NO_RESOURCES = 4,
  RAT_ERR_NOT_APPLICABLE = 5,
  RAT_ERR_INVALID_ARG = 6,
  RAT_ERR_IO = 7,
  RAT_ERR_TIMEOUT = 8,
  RAT_ERR_CANCELLED = 9,
  // extend as needed
} RAT_STATUS;

typedef enum {
  RAT_FILE_READ = 1 << 0,
  RAT_FILE_WRITE = 1 << 1,
  RAT_FILE_CREATE = 1 << 2,
  RAT_FILE_TRUNCATE = 1 << 3,
} RatFileAccess;

typedef enum {
  RAT_SEEK_SET = 0,
  RAT_SEEK_CUR = 1,
  RAT_SEEK_END = 2,
} RatSeekOrigin;
```

---

## Core API (synchronous)

```c
// Open/Create/Close
RAT_STATUS RatOpenFile(const char* path, uint32_t access_flags, uint32_t create_flags, HFILE* out);
RAT_STATUS RatCloseFile(HFILE file);

// Read/Write (blocking)
RAT_STATUS RatRead(HFILE file, void* buf, size_t req, size_t* read);
RAT_STATUS RatWrite(HFILE file, const void* buf, size_t len, size_t* written);

// Seek / metadata
RAT_STATUS RatSeek(HFILE file, int64_t offset, RatSeekOrigin origin, uint64_t* newPos);
RAT_STATUS RatStat(HFILE file, RatFileStat* outStat);

// Convenience
RAT_STATUS RatFlush(HFILE file);

// Interop / escape hatches
RAT_STATUS RatGetUnderlyingFD(HFILE file, int* out_fd);
RAT_STATUS RatOpenFromFD(int fd, HFILE* out);
```

`RatFileStat` is a small struct mirroring `stat` information (size, mode, uid/gid, timestamps). Implementors should keep it minimal.

---

## Core API (asynchronous)

The async API is defined in detail in [`specs/async-io.md`](./async-io.md). File I/O integrates with this system by supporting queued read/write submissions.

```c
// Completion queue lifecycle
RAT_STATUS RatCreateCompletionQueue(size_t depth, HCOMPQ* out);
RAT_STATUS RatCloseCompletionQueue(HCOMPQ q);

// Queueing operations for files
RAT_STATUS RatQueueRead(HCOMPQ q, HFILE file, void* buf, size_t len, uint64_t userTag);
RAT_STATUS RatQueueWrite(HCOMPQ q, HFILE file, const void* buf, size_t len, uint64_t userTag);

// Retrieve completion (see async-io spec for details)
typedef struct {
  uint64_t userTag;
  RAT_STATUS status;
  size_t bytesTransferred;
  uint64_t misc; // unused for file ops
} RatCompletion;

RAT_STATUS RatGetCompletion(HCOMPQ q, RatCompletion* out, int32_t timeout_ms);
```

**Notes:**

* Ordering, cancellation, buffer lifetime, and error semantics are governed by `specs/async-io.md`.
* For regular files, async operations may be serviced by a worker threadpool on kernels without `io_uring`.

---

## Lifecycle & Ownership

* Handles are reference-counted. `RatCloseFile` and `RatCloseCompletionQueue` decrement references and eventually free resources.
* `RatOpenFromFD` optionally *takes ownership* of the FD (documented in the API flag). An alternate API `RatOpenFromFDCopy` duplicates the FD.

---

## Error mapping

Map common `errno` values to `RAT_STATUS`:

* `ENOENT` -> `RAT_ERR_NOT_FOUND`
* `EACCES` -> `RAT_ERR_ACCESS_DENIED`
* `ENOSPC` -> `RAT_ERR_NO_RESOURCES`
* `EIO` -> `RAT_ERR_IO`
* `ETIMEDOUT` -> `RAT_ERR_TIMEOUT`
* Cancelled async operations -> `RAT_ERR_CANCELLED`

Always retain underlying `errno` where useful for debugging; provide `RatGetLastErrno()` for low-level diagnostics if necessary.

---

## File locking & atomic operations

* Provide `RatLockFile(HFILE file, int exclusive)` and `RatUnlockFile(HFILE file)` mapping to `fcntl`/`flock` (advisory) — clearly document semantics.
* Atomic rename uses underlying `rename(2)`.

---

## Security considerations

* Filesystem permissions follow POSIX. Rat-specific IPC objects should have ACLs enforced at creation time.
* Escape-hatch FDs expose kernel-level access; use with caution. Document best practices.

---

## Examples

**Synchronous read**

```c
HFILE f;
if (RatOpenFile("/etc/hosts", RAT_FILE_READ, 0, &f) != RAT_OK) return;
char buf[1024]; size_t got;
if (RatRead(f, buf, sizeof(buf), &got) == RAT_OK) {
  // use data
}
RatCloseFile(f);
```

**Async read (completion-queue)**

```c
HCOMPQ q; RatCreateCompletionQueue(64, &q);
HFILE f; RatOpenFile("big.dat", RAT_FILE_READ, 0, &f);
RatQueueRead(q, f, bigbuf, 65536, 0x1);
RatCompletion c;
RatGetCompletion(q, &c, 5000);
if (c.status == RAT_OK) printf("read %zu bytes", c.bytesTransferred);
RatCloseFile(f);
RatCloseCompletionQueue(q);
```

---

*RatOS File I/O Specification — version 0.1-draft*