# ADR-0007: Networking API Strategy
## Status

Proposed

---

## Context

Networking is a core requirement for RatOS. We must provide developers with a familiar, reliable, and extensible API while avoiding unnecessary complexity or abstraction.

Several design options were considered:

**Option A:** Direct BSD/POSIX Sockets

* Pros: Minimal, portable, well-understood.

* Cons: Feels lazy, forces developers into low-level details, no RatOS identity.

**Option B:** BSD Sockets with WinSock-Style API (Hybrid)

* Pros: Familiar to both Windows and Unix developers; clean abstraction; provides RatOS identity; extensible.

* Cons: Requires designing and maintaining RatAPI wrappers.

**Option C:** Pure Modern Async Abstractions

* Pros: Modern and clean for async-first applications.

* Cons: Can become messy, opinionated, or one-size-fits-all.

**Option D:** Generic Stream Abstractions

* Pros: Consistency with file I/O and IPC.

* Cons: Violates RatAPI’s principle of “one function = one purpose”; over-generalization.

**Option E:** High-Level Networking (HTTP, etc.)

* Pros: Convenient, powerful, opinionated.

* Cons: Too heavy for the kernel/API baseline; risks bloat.

The team prefers a clean but not overengineered approach. A good balance is to start with sockets as the foundation, but wrap them in a consistent RatAPI style.

---

## Decision

RatOS will adopt Option B: BSD Sockets with WinSock-Style API as the baseline networking API.

Provide the traditional socket functions: create, bind, listen, connect, accept, send, receive.

Expose these with RatAPI-style naming conventions (e.g., RatSocketCreate, RatSocketConnect, RatSocketSend).

Integrate sockets with RatOS’s event loop and messaging system (e.g., RatSocketAttachToEventLoop with WM_SOCKET-style messages).

Keep the API thin enough that developers familiar with sockets can use it directly, while offering cleaner naming and structure.

Defer high-level protocols (HTTP, FTP, etc.) to libraries outside of RatAPI.

This strategy allows RatOS to remain compatible with existing paradigms, while introducing its own consistent API identity and preparing for future extensions.

---

## Consequences

Developers can easily port code from Windows or POSIX.

RatOS gains an immediately useful, standard networking layer.

Future work can add RatAPI-native async wrappers (RatNetAsync or similar).

Avoids premature complexity and scope creep.

---

## References

* WinSock
* BSD sockets