# RatOS Specification: Threading & Synchronization

## Overview

This specification defines the RatOS threading and synchronization subsystem. It formalizes the API surface described by ADR-0004: handle-based thread objects and handle-based synchronization primitives, together with semantics, error mapping, ownership rules, and examples.

RatOS exposes a Win32-like, handle-centric interface to developers while mapping these handle objects to POSIX primitives internally (pthreads, futexes, semaphores). The goal is to be familiar to Win32 developers, safe for multithreaded programs, and compatible with the underlying Linux kernel.

---

## Key Concepts

* **RatThread / HTHREAD**: opaque handle representing a thread of execution.
* **RatMutex / HMUTEX**: mutual-exclusion object.
* **RatSemaphore / HSEMAPHORE**: counting semaphore.
* **RatEvent / HEVENT**: manual/auto-reset event (signalling primitive).
* **RatRWLock / HRWLOCK**: read-write lock.
* **RatWaitable**: any handle that can be waited on via `RatWaitForObject` APIs (threads and sync objects are waitable).
* **RatWaitResult**: enumeration for wait outcomes (signalled, timeout, abandoned, error).

---

## Public Types & Enums

```c
typedef uint64_t HTHREAD;
typedef uint64_t HMUTEX;
typedef uint64_t HSEMAPHORE;
typedef uint64_t HEVENT;
typedef uint64_t HRWLOCK;

typedef enum {
  RAT_WAIT_OK = 0,
  RAT_WAIT_TIMEOUT = 1,
  RAT_WAIT_FAILED = 2,
  RAT_WAIT_ABANDONED = 3, // if we support abandoned mutex semantics
} RatWaitResult;

typedef enum {
  RAT_THREAD_CREATE_DETACHED = 0x1,
  RAT_THREAD_CREATE_SUSPENDED = 0x2, // optional: create suspended
} RatThreadCreateFlags;
```

---

## Error Handling

* APIs return `RAT_STATUS` or `bool` per subsystem convention. For consistency with other RatAPI specs, thread and sync calls return `RAT_STATUS`.
* Detailed error information is available via `RatGetLastError()` which is **thread-local**.

---

## Core Thread APIs

```c
// Thread creation and lifecycle
RAT_STATUS RatCreateThread(void (*start_routine)(void*), void* arg, size_t stack_size, uint32_t flags, HTHREAD* out);
RAT_STATUS RatCloseThread(HTHREAD thread); // decrement handle ref; thread keeps running if not joined/detached
RAT_STATUS RatJoinThread(HTHREAD thread, uint64_t timeout_ms); // wait for termination
RAT_STATUS RatTerminateThread(HTHREAD thread, int exit_code); // forceful termination (discouraged)

// Thread properties
RAT_STATUS RatSetThreadPriority(HTHREAD thread, int priority);
RAT_STATUS RatGetThreadPriority(HTHREAD thread, int* out_priority);
RAT_STATUS RatSetAffinity(HTHREAD thread, uint64_t cpu_mask);

// TLS (thread-local storage) helpers
RAT_STATUS RatTlsAlloc(uint32_t* out_index);
RAT_STATUS RatTlsFree(uint32_t index);
RAT_STATUS RatTlsSetValue(uint32_t index, void* value);
RAT_STATUS RatTlsGetValue(uint32_t index, void** out_value);
```

**Notes:**

* `stack_size` of 0 uses a reasonable default (platform-dependent).
* `RAT_THREAD_CREATE_SUSPENDED` requires `RatResumeThread` to start execution. If not supported, document as optional.
* `RatTerminateThread` is provided as an escape hatch; prefer cooperative cancellation.

---

## Synchronization Object APIs

### Mutex

```c
RAT_STATUS RatCreateMutex(bool initiallyOwned, HMUTEX* out);
RAT_STATUS RatLockMutex(HMUTEX mutex, uint64_t timeout_ms);
RAT_STATUS RatUnlockMutex(HMUTEX mutex);
RAT_STATUS RatCloseMutex(HMUTEX mutex);
```

**Semantics:** maps to `pthread_mutex_t` or futex-based implementation; `Lock` blocks until acquired or timeout.

### Semaphore

```c
RAT_STATUS RatCreateSemaphore(int32_t initialCount, int32_t maxCount, HSEMAPHORE* out);
RAT_STATUS RatWaitSemaphore(HSEMAPHORE sem, uint64_t timeout_ms);
RAT_STATUS RatReleaseSemaphore(HSEMAPHORE sem, int32_t releaseCount, int32_t* previousCount);
RAT_STATUS RatCloseSemaphore(HSEMAPHORE sem);
```

### Event (auto/manual reset)

```c
typedef enum { RAT_EVENT_AUTO_RESET = 0, RAT_EVENT_MANUAL_RESET = 1 } RatEventType;
RAT_STATUS RatCreateEvent(RatEventType type, bool initialState, HEVENT* out);
RAT_STATUS RatSetEvent(HEVENT ev);
RAT_STATUS RatResetEvent(HEVENT ev);
RAT_STATUS RatCloseEvent(HEVENT ev);
```

### Read-Write Lock

```c
RAT_STATUS RatCreateRWLock(HRWLOCK* out);
RAT_STATUS RatAcquireRead(HRWLOCK lock, uint64_t timeout_ms);
RAT_STATUS RatAcquireWrite(HRWLOCK lock, uint64_t timeout_ms);
RAT_STATUS RatReleaseRWLock(HRWLOCK lock);
RAT_STATUS RatCloseRWLock(HRWLOCK lock);
```

---

## Waiting APIs

```c
// Wait for single object
RatWaitResult RatWaitForObject(uint64_t handle, uint64_t timeout_ms);

// Wait for multiple objects
// waitAll==true: wait until all signalled or timeout; waitAll==false: wait for any
RatWaitResult RatWaitForMultipleObjects(uint64_t* handles, size_t count, bool waitAll, uint64_t timeout_ms, size_t* signalledIndex);
```

**Semantics & Guarantees**

* Waiting on a terminated thread returns `RAT_WAIT_OK` immediately (thread signalled).
* `RatWaitForMultipleObjects` should be efficient; internally it can use `futex`/`poll`/`epoll` or an internal waiter list.
* Abandoned mutex: if a thread exits while owning a mutex, behavior is implementation-defined; consider returning `RAT_WAIT_ABANDONED` for the waiter.

---

## Ownership & Lifecycle

* All handle creations return a reference-counted handle. `RatClose*` releases the caller's reference.
* For threads: if created non-detached, joining is required to release OS resources; `RatCloseThread` decrements handle reference but may not reap the thread.
* For sync objects: closing a handle while other waiters exist should not cause undefined behaviour; waiting threads should receive an error or the object may remain alive until last reference is closed.

---

## Mapping to POSIX Internals

* HTHREAD → `pthread_t` (plus metadata: detach state, exit code, joiners list).
* HMUTEX → `pthread_mutex_t` or futex-backed mutex.
* HSEMAPHORE → POSIX semaphores (`sem_t`) or futex-based counters.
* HEVENT → Condition variables + associated mutex or futex + state word.
* HRWLOCK → `pthread_rwlock_t`.

Implementations should provide a central handle table mapping handles to internal objects and ensure operations are thread-safe.

---

## Cancellation & Cooperative Shutdown

* Prefer cooperative cancellation: provide `RatThreadRequestCancel(HTHREAD thread)` and `RatThreadTestCancel()` that threads can poll to exit cleanly.
* `RatTerminateThread` remains available but is strongly discouraged due to resource leaks and undefined state.

---

## Thread-Local Storage & GetLastError

* `RatGetLastError()` must be implemented using thread-local storage to avoid race conditions.
* TLS helper APIs (`RatTlsAlloc`, etc.) provide a platform-neutral way to store per-thread values.

---

## Performance & Tuning

* Default stack size should be conservative (e.g., 8 MiB) but configurable at thread creation.
* Provide a pool allocator for frequently-created short-lived threads if benchmarks indicate overhead.

---

## Interop & Porting

* Provide helper APIs to convert between Rat handles and native POSIX objects for advanced users: e.g., `RatGetNativeThread(HTHREAD)` may return a `pthread_t` when applicable.
* Document that using native pthread APIs on threads created by RatAPI is supported but discouraged; better to stick to RatAPI for portability.

---

## Examples

### Basic thread

```c
void worker(void* arg) {
  // do work
}

HTHREAD t;
RatCreateThread(worker, NULL, 0, 0, &t);
RatJoinThread(t, UINT64_MAX);
RatCloseThread(t);
```

### Mutex & wait

```c
HMUTEX m; RatCreateMutex(false, &m);
RatLockMutex(m, 1000); // wait up to 1s
// critical section
RatUnlockMutex(m);
RatCloseMutex(m);
```

### Wait for multiple

```c
uint64_t handles[2] = { (uint64_t)threadHandle, (uint64_t)eventHandle };
size_t idx;
RatWaitForMultipleObjects(handles, 2, false, 5000, &idx);
// idx indicates which handle signalled
```

---

## Future Work

* Define user-mode fiber API (`RatFiber`) and a scheduler that can coexist with kernel threads.
* Define priority / QoS semantics and mapping to Linux `nice` and real-time scheduling policies.
* Define clear abandoned-mutex semantics and possible recovery mechanisms.

---

*RatOS Threading & Synchronization Specification — version 0.1-draft*