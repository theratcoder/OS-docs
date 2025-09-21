# ADR-0004: Threading & Synchronization

## Status

Accepted

## Context

RatOS requires a unified threading and synchronization model as part of RatAPI. While Linux already provides robust threading (via pthreads and clone()), RatAPI is designed to present a Win32-style, handle-based API that is consistent across all system services.

The design must balance:

Linux compatibility (so existing native apps still run)

Win32 familiarity (to meet the goal of a friendly API)

Future extensibility (e.g., support for fibers or more advanced primitives)

## Decision

RatOS will implement a hybrid approach:

### Threads:

* Exposed as `RatThread` handles.

* Created via `RatCreateThread`.

* Backed internally by `pthread_create` or `clone()`.

* Can be joined, suspended (if supported), or waited on via `RatWaitForObject`.

### Synchronization Objects:

Exposed as distinct handle types:

* `RatMutex` (maps to `pthread_mutex_t`)

* `RatSemaphore` (maps to `sem_t`)

* `RatEvent` (maps to condition variables / futexes)

* `RatRWLock` (maps to `pthread_rwlock_t`)

All can be used with `RatWaitForObject` and related APIs.

Handle-based model provides consistency with the rest of RatAPI.

### Error Handling:

* Thread and sync functions return bool for success/failure.

* Detailed error codes retrieved via `RatGetLastError` (thread-local).

### Future Extensibility:

* User-mode scheduling (fibers) may be added later as an optional subsystem.

* Does not affect existing RatAPI contracts.

Consequences
**Positive**

* ✅ RatAPI remains consistent and familiar to Win32 developers.

* ✅ Existing Linux-native applications remain compatible, as pthreads and related primitives are still supported internally.

* ✅ Provides a foundation for async/event-driven programming.

**Negative**

* ❌ Slight runtime overhead from handle indirection.

* ❌ More code required to wrap pthreads into RatAPI handles.

## Alternatives Rejected

1. Pure POSIX passthrough — too inconsistent with RatAPI’s design philosophy.

2. Pure user-mode threading (fibers) — too complex for early versions of RatOS, but may be revisited later.

## References

* ADR-0001: Handle-Based API Model

* ADR-0002: Memory Management Model

* ADR-0003: File and I/O Abstraction