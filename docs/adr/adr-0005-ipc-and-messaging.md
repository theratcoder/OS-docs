# ADR-0005: Inter-Process Communication and Messaging in RatAPI

---

## Status

Proposed

---

## Context

Applications on RatOS need mechanisms to communicate with each other and with system services. Linux already provides a variety of IPC primitives (pipes, sockets, shared memory, signals), but RatAPI must decide what abstractions to expose. The goal is to make RatOS approachable for both Linux developers (who are familiar with POSIX APIs) and Windows-style developers (who may expect a message-loop model).

---

## Decision

RatAPI will adopt a hybrid model for IPC and messaging:

* Expose low-level primitives in RatAPI:

* Pipes (unstructured byte streams).

* Shared memory (fast large-data sharing).

* Events (synchronization + notifications).

* Local and network sockets (general-purpose communication).

* Provide a message-loop abstraction, inspired by Win32:

* Each process may create a message queue and run a loop.

* Messages can be posted to other processes or windows (if GUI).

Core APIs will be provided:

```c
RatStatus RatGetMessage(RatMessage* outMessage, uint32_t timeout);
RatStatus RatPeekMessage(RatMessage* outMessage);
RatStatus RatPostMessage(RatTarget target, uint32_t msg, uintptr_t wparam, intptr_t lparam);
```


Message queues integrate with RatAPI handles and can be waited on like other synchronization objects.

**Future-proofing:** RatAPI may later add higher-level abstractions (structured RPC, publish/subscribe) but this is explicitly deferred until after the core primitives + message loop are stable.

---

## Consequences

RatOS developers get the best of both worlds:

* POSIX-minded developers can use pipes, sockets, and shared memory.

* Application developers can rely on a unified message loop and avoid dealing with raw primitives.

This design introduces a moderate implementation burden (since RatAPI must wrap both Linux primitives and its own message queue system).

Consistency is maintained: all IPC objects are represented as RatAPI handles and follow the naming/error-handling conventions.