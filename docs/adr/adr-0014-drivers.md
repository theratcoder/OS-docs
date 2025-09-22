# ADR-0015: Driver Strategy

## Status
Proposed

---

## Context

RatOS currently runs on top of a stock Linux kernel with its full set of in-tree drivers. Linux provides wide-ranging hardware support across storage, networking, display, sound, input devices, and more. Writing and maintaining drivers from scratch is a major undertaking, and not feasible at the current stage of RatOS development.

However, RatOS aims to grow into a distinct operating system, and its eventual kernel model may diverge from Linux. This raises the question of how drivers will be handled both now and in the future.

---

## Decision

We will adopt a phased driver strategy:

### Phase 0 (Current)

Use the stock Linux kernel with its existing in-tree drivers.

No new driver development required.

RatOS gains “for free” the same hardware compatibility as modern Linux distributions.

### Phase 1 (Short-to-Mid Term)

Continue to ship RatOS with Linux kernel modules enabled for maximum compatibility.

Consider trimming unused drivers for performance and footprint improvements, but maintain overall reliance on Linux’s driver ecosystem.

RatAPI remains independent of the driver model, ensuring flexibility.

### Phase 2 (Long Term / Kernel Divergence)

If and when RatOS introduces a distinct kernel or diverges significantly from Linux internals, evaluate options:

- Linux driver compatibility layer (similar to FreeBSD’s LinuxKPI).

- Selective reimplementation of critical drivers (e.g., display, storage, networking).

- New driver framework aligned with RatOS design principles (possibly moving some drivers to user space).

At no point will RatOS commit to wholesale reimplementation of all drivers. Instead, Linux’s extensive hardware support will be leveraged as long as possible.

---

## Consequences

- **Short-term benefit:** Immediate hardware support with no development cost.

- **Flexibility:** RatAPI is not tied to any single driver model, allowing us to switch later.

- **Risk:** Long-term reliance on Linux could limit independence if RatOS diverges too far.

- **Mitigation:** Future-proof design by keeping RatAPI above the driver layer and documenting driver/kernel dependencies.

## Alternatives Considered

- **Write new drivers from scratch:** Rejected due to high cost and lack of expertise.

- **Fork Linux drivers early:** Rejected as premature; would increase maintenance burden.

- **Adopt a microkernel driver model now:** Rejected as over-engineering at this stage.

---

## Decision

Use Linux in-tree drivers for now, defer custom driver work until RatOS diverges at the kernel level.