# RatOS Security Specification

## Overview
This document defines the RatAPI-level security model. While RatOS relies on the Linux kernel and LSMs for core enforcement, RatAPI introduces a consistent abstraction for managing object permissions, transferring capabilities, and handling application permissions.

The goal is to:
- Provide a familiar, friendly API for developers.
- Ensure secure default behavior for all RatAPI objects.
- Allow future extensibility without breaking applications.

---

## Core Concepts

### Access Control Lists (ACLs)
- Every named RatAPI object (message queues, shared memory, windows, devices) has an associated ACL.
- ACLs define which UIDs, GIDs, or capability tokens may access the object, and with what rights (read, write, execute, control).

### Capability Tokens
- A **capability token** is an unforgeable, opaque identifier bound to a handle.
- Passing a handle across processes transfers the capability to use that object.
- Capabilities can be limited (e.g., read-only view of a shared memory region).

### Application Permissions
- Applications must declare required permissions in their package manifest.
- RatOS enforces these permissions at install time, with optional runtime checks for sensitive actions (e.g., camera, microphone).
- Permissions map onto kernel primitives (namespaces, seccomp, AppArmor profiles).

### Auditing
- Security-sensitive actions (denials, capability grants, permission requests) are logged.
- Admins can use `ratctl security` to inspect active ACLs, permissions, and capabilities.

---

## API Surface

### Object ACL Management

```c
typedef struct RatAclEntry {
    uid_t user;
    gid_t group;
    uint32_t rights; // bitmask of RAT_RIGHT_READ, RAT_RIGHT_WRITE, etc.
} RatAclEntry;

typedef struct RatAcl {
    size_t entryCount;
    RatAclEntry* entries;
} RatAcl;
```

- `RatStatus RatGetObjectAcl(HOBJECT hObject, RatAcl* pAcl);`retrieves the current ACL for an object.

- `RatStatus RatSetObjectAcl(HOBJECT hObject, const RatAcl* pAcl);` replaces the ACL for an object. Only the owner or a privileged process may call this.

### Capability Tokens
- `RatStatus RatGrantCapability(HOBJECT hObject, uint32_t rights, HCAPABILITY* phCap);` creates a capability token with limited rights for an object.

- `RatStatus RatUseCapability(HCAPABILITY hCap, HOBJECT* phObject);` resolves a capability token into a usable handle.

- `RatStatus RatRevokeCapability(HCAPABILITY hCap);` revokes a previously granted capability.

### Application Permissions
- `RatStatus RatRequestPermission(const char* permissionName);` requests a runtime permission (e.g., "network.access", "camera.use").

If not previously granted, the system broker may prompt the user or deny automatically based on policy.

- `bool RatCheckPermission(const char* permissionName);` checks if the calling process currently holds a permission.

### Auditing & Logging
- `RatStatus RatGetSecurityLog(RatSecurityLogEntry* buffer, size_t bufferSize, size_t* pCount);` retrieves recent security log entries (denials, grants, revocations).

- `RatStatus RatDescribeCapability(HCAPABILITY hCap, RatCapabilityInfo* pInfo);` returns metadata about a capability (issuer, target object, granted rights).

### Rights Constants
```c
#define RAT_RIGHT_READ      0x1
#define RAT_RIGHT_WRITE     0x2
#define RAT_RIGHT_EXECUTE   0x4
#define RAT_RIGHT_CONTROL   0x8
```

### Examples
**Example 1:** Creating a restricted shared memory region
```c
RatShmHandle hShared;
RatCreateSharedMemory("MyRegion", 4096, &hShared);

RatAcl acl = {
    .entryCount = 1,
    .entries = (RatAclEntry[]) {
        { .user = 1001, .rights = RAT_RIGHT_READ }
    }
};

RatSetObjectAcl(hShared, &acl);
```

**Example 2:** Granting a read-only capability
```c
RatCapablityHandle hCap;
RatGrantCapability(hShared, RAT_RIGHT_READ, &hCap);

// Pass hCap to another process via IPC
```
**Example 3:** Requesting camera access
```c
if (!RatCheckPermission("camera.use")) {
    if (RatRequestPermission("camera.use") != RAT_SUCCESS) {
        printf("Camera access denied!\n");
        return;
    }
}

// Safe to open camera device
```
---

## Implementation Notes
Internally, ACLs and tokens are managed by a privileged security broker daemon.

Enforcement uses Linux namespaces, AppArmor profiles, and seccomp filters.

RatAPI objects must be created with default-deny policies unless explicitly opened.

---

## Future Extensions
- Role-based access control (RBAC) for enterprise environments.

- Time-limited or one-shot capabilities.

- Distributed capabilities for networked systems.

- Graphical UI for permission prompts.

---

*RatOS Security — version 0.1-draft*