# ADR-0002: Memory Management Model

## Status

Accepted

## Context

Memory management is a foundational subsystem for RatOS, affecting processes, threads, file mapping, IPC, and potentially graphics. RatOS aims to provide a **consistent, developer-friendly API** (RatAPI) that abstracts system services while maintaining compatibility with Linux.

We need to decide how memory allocations, virtual memory, and shared memory are exposed to developers. The API should align with the handle-based hybrid model established in ADR-0001, where developers work with **typed handles** rather than raw pointers where possible.

## Decision

We will adopt a **handle-based memory model** for RatAPI:

* **Developer-facing API:**

  * Use `HMEM` as the primary handle for memory regions.
  * Optional function to map a handle to a pointer for direct access.
  * Provide functions for allocation, deallocation, mapping, and unmapping.

### Example Usage

```c
HMEM region = RatAllocMemory(4096);   // Allocate 4 KB
void* ptr = RatMapMemory(region);     // Map handle to address for access
RatFreeMemory(region);                // Free memory handle
```

* **Internal implementation:**

  * Map `HMEM` to Linux primitives such as `mmap`/`munmap` for private regions.
  * Use `shm_open`/`mmap` for shared memory or memory-mapped files.
  * Reference counting ensures safe cleanup.
  * Pointer mapping is ephemeral; handles remain valid even if virtual address changes.

* **Optional future enhancements:**

  * Separate handle types for virtual memory, shared memory, or file-mapped memory if needed.
  * Access permissions and protections enforced at the handle level.

## Alternatives

### 1. Direct pointer exposure

* Pros: simple, minimal overhead.
* Cons: inconsistent with handle-based API, harder to enforce access control, unsafe across processes.

### 2. Pure handle abstraction (without mapping function)

* Pros: fully consistent, safe.
* Cons: inconvenient for developers needing pointer access for performance.

### 3. Segmented memory handles by type (`HVMEM`, `HSHM`, `HMAPPED`)

* Pros: explicit, safe, clear boundaries.
* Cons: more handle types to manage, slightly higher complexity for developers.

## Consequences

* **Positive:**

  * Maintains RatAPI consistency with hybrid handle model.
  * Supports safe cross-process memory management.
  * Linux compatibility retained internally.
  * Future-proof for subsystems like graphics or shared memory.

* **Negative:**

  * Slight overhead for mapping/unmapping handles.
  * Developers must understand handle vs pointer mapping.
  * Runtime layer complexity to manage handles internally.

## References

* Win32 memory APIs: `VirtualAlloc`, `VirtualFree`, `MapViewOfFile`
* Linux memory primitives: `mmap`, `munmap`, `shm_open`
* ADR-0001: Handle Types vs File Descriptors