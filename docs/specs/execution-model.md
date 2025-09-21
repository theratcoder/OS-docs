# RatOS Execution Model Specification

## Overview
The RatOS execution model defines how programs create, manage, and coordinate **processes**, **threads**, and **synchronization objects**. It establishes a unified abstraction for **waiting** on and **signaling** events, whether they originate from processes, threads, I/O, or IPC.  

The model is designed with these goals:
- **Consistency**: all resources are handles (`HPROCESS`, `HTHREAD`, `HEVENT`, etc.), and all handles can participate in synchronous or asynchronous waits.
- **Flexibility**: supports both synchronous waits and asynchronous integration via completion queues (`HCOMPQ`).
- **Familiarity**: provides a Win32-style message loop for developers who prefer that model, while remaining compatible with Linux primitives under the hood.

---

## Core Concepts

### Processes
- A **process** is an execution container with its own address space, handle table, and security context.
- Identified by a handle (`HPROCESS`).
- Created using:
  ```c
  RatStatus RatCreateProcess(
      const char* path,
      const char* const* argv,
      const RatProcessAttributes* attrs,
      HPROCESS* outProcess
  );
  ```

**Lifecycle:**

Create → Run → Exit (exit code retrievable).

Must be closed with `RatCloseHandle`.

### Threads
A thread is a unit of execution within a process.

Identified by a handle (`RatThreadHandle`).

Created using:

```c
RatStatus RatCreateThread(
    RatThreadEntry entry,
    void* arg,
    const RatThreadAttributes* attrs,
    HTHREAD* outThread
);
```
**Lifecycle:**

Create → Run → Exit (exit code retrievable).

Must be closed with `RatCloseHandle`.

### Synchronization Objects
- **Events:** signaled/non-signaled, used for signaling between threads or processes.

- **Mutexes:** mutual exclusion locks, recursive or non-recursive.

- **Semaphores:** counters for controlling access to shared resources.

All are handles (`RatEventHandle`, `RatMutexHandle`, `RatSemaphoreHandle`) and follow the same lifetime rules.

### Unified Waiting
**Synchronous wait:**
Any waitable handle can be waited on with a blocking call:

```c
Copy code
RatStatus RatWaitForHandle(HANDLE h, uint32_t timeoutMs);
RatStatus RatWaitForMultipleHandles(HANDLE* handles, size_t count, bool waitAll, uint32_t timeoutMs, size_t* outIndex);
```
Examples of waitable handles:

- `RatProcessHandle` → wait for process exit.

- `RatThreadHandle` → wait for thread completion.

- `RatEventHandle` → wait for event signal.

- `RatFileHandle` → wait for async I/O completion (when combined with a completion queue).

- `RatMsgQueueHandle` → wait for messages.

**Asynchronous wait:**
For integration with event-driven systems, waits can be redirected into a completion queue (`RatCompQueueHamdle`):

```c
RatStatus RatWaitProcessAsync(RatProcessHandle h, RatCompQueueHandle q, uintptr_t userData);
RatStatus RatWaitThreadAsync(RatThreadHandle h, RatCompQueueHandle q, uintptr_t userData);
RatStatus RatWaitEventAsync(RatEventHandle h, RatCompQueueHandle q, uintptr_t userData);
```
When the event completes, the completion queue receives an entry with the user-provided context.

---

## IPC Integration
Message queues (HMSGQUEUE) integrate with both synchronous waits and async queues.

Processes can post and receive messages (RatPostMessage, RatGetMessage).

Windowing and GUI subsystems also build on this model (see windowing.md).

---

## Message Loop Model
Applications may adopt a Win32-style message loop for convenience:

```c
RatMsg msg;
while (RatGetMessage(&msg, NULL)) {
    RatDispatchMessage(&msg);
}
```
This is especially useful for GUI applications. Internally, `RatGetMessage` may wait on multiple sources: process signals, thread completions, I/O, and GUI/windowing messages.

---

## Error Handling
All functions return a RatStatus code (see `error-handling.md`).

No reliance on global or thread-local error state.

Consistent codes across process, thread, sync, and IPC APIs (`RAT_SUCCESS`, `RAT_ERR_TIMEOUT`, `RAT_ERR_INVALID_HANDLE`, etc.).

---

## Examples
**Example 1:** Spawn Process and Wait
```c
RatProcessHandle child;
RatStatus st = RatCreateProcess("/bin/ls", (const char*[]){"ls", "-l", NULL}, NULL, &child);

if (st == RAT_SUCCESS) {
    st = RatWaitForHandle(child, RAT_INFINITE);
    RatCloseHandle(child);
}
```
**Example 2:** Async Thread Wait
```c
RatThreadHandle thread;
RatCreateThread(MyThreadFn, NULL, NULL, &thread);

RatCompQueueHandle q;
RatCreateCompletionQueue(&q);

RatWaitThreadAsync(thread, q, 1234); // userData = 1234

// Later, event appears in queue
RAT_COMPLETION c;
if (RatGetNextCompletion(q, &c)) {
    // c.userData == 1234
}
```
**Example 3:** Message Loop with Event
```c
RatEventHandle evt;
RatCreateEvent(false, false, &evt); // manualReset=false, initialState=false

while (RatGetMessage(&msg, NULL)) {
    if (msg.type == RATMSG_EVENT_SIGNALED && msg.hEvent == evt) {
        // handle event
    }
    RatDispatchMessage(&msg);
}
```

---

## Future Extensions
- Fibers / user-mode scheduling.

- Job objects (manage groups of processes).

- Asynchronous process creation (spawn + immediate async event).

- Cross-process shared handle inheritance rules.

---

## References
- threading-and-sync.md

- ipc.md

- async-io.md

- error-handling.md

- windowing.md

---

*RatOS Execution Model Specification — version 0.1-draft*