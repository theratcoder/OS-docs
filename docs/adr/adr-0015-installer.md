# ADR-0016: Installer Strategy
## Status

Proposed

---

## Context

RatOS requires a way to be installed onto user systems beyond manual copying or running under QEMU. Providing a structured installation process will improve accessibility and adoption, even during the early stages of development.

There are several possible installer strategies:

Script-based (minimal, Arch-like): simple shell scripts that perform partitioning, copying of system files, and bootloader installation.

TUI installer (ncurses-based, Debian/FreeBSD-like): a text-based guided installer that makes the process easier for less experienced users.

GUI installer (Windows/Ubiquity-like): a polished graphical installer running in a RatOS live environment, suitable for mainstream adoption.

A phased approach allows RatOS to provide installation capabilities early while leaving room for more advanced installers later.

---

## Decision

RatOS will use a phased installer strategy:

### Phase 1: Script-Based Installer

Build on the existing QEMU installation script.

Distribute as part of a bootable live ISO.

Supports partitioning, rootfs copying, and bootloader setup.

### Phase 2: TUI Installer

Implement a guided, ncurses-based installer.

Handles disk partitioning, package selection, user creation, and configuration.

Provides a balance between flexibility and accessibility.

### Phase 3: GUI Installer

Once RatAPI GUI stack and basic controls are available, implement a graphical installer.

Doubles as a live demo environment.

Provides a polished, professional user experience.

---

## Bootloader

- Use GRUB as the default bootloader for maximum hardware and filesystem support.

- Evaluate simpler alternatives (e.g., systemd-boot) in future.

---

## Distribution format

- Primary: bootable ISO images.

- Optional: support netboot (PXE) once networking stack matures.

---

## Consequences

Early users will need to be comfortable with shell scripts for installation.

Provides a clear upgrade path for improving user experience over time.

Ensures RatOS has an installer solution in every stage of its maturity.

Locks RatOS into supporting GRUB initially, though this remains flexible.