# RatOS Init System Specification
## Overview

The init system is the first userspace process in RatOS (PID 1). It is responsible for bringing up the system, supervising services, and managing shutdown/reboot. Init is minimal by design, but provides enough flexibility to support both simple single-user setups and a full multi-service environment.

---

## Goals

Provide a clear and predictable boot sequence.

Supervise long-lived system services (daemons).

Support extensibility without requiring kernel changes.

Keep early implementations small and auditable.

Responsibilities of Init

Mount essential filesystems: /proc, /sys, /dev.

Start services (e.g. networking, login shells, desktop/session managers).

Reap orphaned/zombie processes.

Handle shutdown/reboot signals.

---

## Init Implementation Strategy
### v0: Static Init

A hardcoded binary (/sbin/init or /init).

Mounts essential filesystems.

Spawns a single shell or test process (e.g. /bin/sh).

Reaps zombies in a simple loop.

### v1: Config-Driven Init (where we are now)

Reads /etc/init.conf (INI/TOML/YAML format).

Config entries define services, restart policies, and dependencies.

Example:
```ini
[service]
name=getty
exec=/bin/getty ttyS0
restart=always
```
```ini
[service]
name=network
exec=/bin/netd
restart=on-failure
```

Provides a simple service supervision loop.

### v2 (Future): Service Manager

Support parallel startup and dependency resolution.

Unified logging.

Optional socket activation.

Isolation via namespaces or cgroups (if supported in kernel).

Relationship with RatAPI

Bootstrapping: Init MAY use direct syscalls in early implementations.

Consistency: Init SHOULD migrate to RatAPI once stable, to act as a reference implementation of process management and event handling.

Service Supervision: Init uses RatAPI primitives such as RatCreateProcess, RatWaitForHandle, and events (HEVENT) to manage services.

---

## Example Init Loop (Pseudocode)
```c
mount("proc", "/proc", "proc", 0, NULL);
mount("sysfs", "/sys", "sysfs", 0, NULL);

HPROCESS shell;
RatCreateProcess("/bin/sh", NULL, NULL, &shell);

while (true) {
    RatHandle handles[MAX_SERVICES];
    size_t count = CollectAllServiceHandles(handles);

    size_t index;
    if (RatWaitForMultipleHandles(handles, count, false, RAT_INFINITE, &index) == RAT_SUCCESS) {
        HandleServiceExit(handles[index]);
        RestartServiceIfNeeded(handles[index]);
    }
}
```

---

## Security Considerations

Init must run as PID 1 and cannot be killed.

Init should drop privileges when spawning services, where applicable.

Init must validate configs strictly to avoid boot loops or privilege escalation.

---

## Roadmap

1. Bootstrap phase: static init with shell.

2. System usable: config-driven init with restart policies. **(We are here)**

3. Full-featured: modular service manager with optional advanced features.

---

*RatOS Init Specification — version 0.1-draft*