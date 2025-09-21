# RatOS Memory Management Specification
## Overview

RatOS memory management builds on the Linux kernel’s virtual memory subsystem while exposing a consistent RatAPI for developers. The goal is to make memory allocation, protection, and mapping safe, predictable, and unified, whether applications are allocating simple buffers or working with shared memory for IPC.

---

## Design Principles

Consistency: Unified RatAPI naming and behavior across all allocation functions.

Safety: Default to zero-initialized memory and enforce clear error codes.

Flexibility: Support traditional heap allocations and advanced features like memory mapping and shared memory.

Interop: Keep compatibility with POSIX APIs where possible while offering higher-level RatOS semantics.

---

## Core Concepts
### Address Space

Each process gets a virtual address space, managed by the Linux kernel.

RatAPI abstracts over mmap, munmap, brk, and related syscalls.

### Allocation Modes

Heap Allocation: For general-purpose memory.

Virtual Allocation: Explicit control over page mapping, alignment, and protections.

Shared Memory: Named regions for IPC between processes.

### Protection Flags

RatAPI supports memory protection flags, mapped to Linux’s PROT_* flags:
```c
RAT_MEM_READ

RAT_MEM_WRITE

RAT_MEM_EXECUTE

RAT_MEM_NONE
```

---

## API Overview
### Types
```c
typedef void* RatMemPtr;

typedef enum {
    RAT_STATUS_OK,
    RAT_STATUS_ERROR,
    RAT_STATUS_INVALID_ARG,
    RAT_STATUS_NO_MEMORY,
    RAT_STATUS_PERMISSION
} RatStatus;

typedef enum {
    RAT_MEM_COMMIT,
    RAT_MEM_RESERVE
} RatMemAllocType;

typedef enum {
    RAT_MEM_PRIVATE,
    RAT_MEM_SHARED
} RatMemMapType;
```

---

### Heap Allocation

Simple malloc/free equivalents:
```c
RatMemPtr RatMemAlloc(size_t size);
RatMemPtr RatMemCalloc(size_t count, size_t size);
RatMemPtr RatMemRealloc(RatMemPtr ptr, size_t newSize);
RatStatus RatMemFree(RatMemPtr ptr);
```

### Virtual Memory Allocation

Explicit page-based allocations (Win32 VirtualAlloc style):
```c
RatMemPtr RatMemReserve(size_t size);
RatStatus RatMemCommit(RatMemPtr ptr, size_t size, int protectionFlags);
RatStatus RatMemProtect(RatMemPtr ptr, size_t size, int protectionFlags);
RatStatus RatMemRelease(RatMemPtr ptr, size_t size);
```

### Shared Memory

Named memory regions for IPC:
```c
RatMemPtr RatSharedMemCreate(const char* name, size_t size);
RatMemPtr RatSharedMemOpen(const char* name);
RatStatus RatSharedMemClose(RatMemPtr ptr);
RatStatus RatSharedMemUnlink(const char* name);
```

---

## Error Handling

All functions return either a RatMemPtr (null on error) or RatStatus.

Detailed error codes provided through RatGetLastError() (optional) or direct return values.

---

## Integration with Other Systems

IPC: Shared memory integrates with RatOS’s messaging system.

File I/O: Memory-mapped files (RatFileMapView) reuse the same protection and allocation logic.

Threading: Synchronization primitives can be placed in shared memory for cross-process locks.

---

## Security Considerations

All memory is zero-initialized on allocation to avoid data leaks.

Memory protections enforced (no execute + write at same time).

Shared memory regions require appropriate permissions (tied to RatOS security model).

---

## Future Extensions

- Huge pages support for performance-critical applications.

- NUMA-aware allocation strategies.

- Sandboxed memory quotas for app isolation.

- Integration with RatOS debugging/profiling tools (heap dump, leak detection).

---

*RatOS Memory Management — version 0.1-draft*