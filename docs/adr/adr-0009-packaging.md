# ADR-0009: Application Packaging Strategy

## Status

Proposed

---

## Context

One of RatOS’s core goals is to provide a modern, cohesive application distribution experience, while also supporting multiple user expectations:

- App Store — A GUI for discovering and installing apps.

- Package Manager — A CLI tool for developers and power users, comparable to apt.

- Standalone Installer — A Windows-like, double-clickable file that contains a whole app installer.

Existing models (e.g. .deb for Debian/Ubuntu, .rpm for Red Hat, Flatpak/Snap for cross-distro packaging) each solve parts of the problem, but none alone deliver the RatOS vision. We need:

- Unified UX: All three distribution modes should use the same underlying package format.

- Security: Packages must be signed and verified before installation.

- Metadata & Permissions: Apps should declare requirements (permissions, APIs, icons, descriptions) in a manifest.

- Cleanup: Installers should auto-delete by default, avoiding leftover clutter.

---

## Decision

RatOS will adopt a hybrid approach, layering a RatOS-native package format (.ratpkg) over a proven backend (dpkg/apt).

**Canonical format:** .ratpkg

Contains:

- manifest.json (app metadata, permissions, RatAPI integration).

- signature (cryptographic verification).

- Payload (initially .deb; may expand to Flatpak/AppImage or native Rat formats).

### Installation lifecycle:

1. Verify package signature.

2. Read manifest and enforce declared permissions.

3. Install payload using dpkg (or later Rat-native installer).

4. Auto-delete package file after successful install (unless overridden).

Frontends:

- App Store (GUI) → wraps ratpkg commands.

- CLI Package Manager (ratpkg) → user/developer facing tool.

- Standalone Installer (double-click .ratpkg) → launches GUI installer and follows the same lifecycle.

This ensures consistency across all user flows, while leveraging mature Linux tooling to handle dependency resolution, versioning, and upgrades.

## Alternatives Considered

**Option A:** Use .deb directly.

- Pros: Simple, widely supported, low effort.

- Cons: Lacks unified RatOS identity, no manifest or signing integration, inconsistent UX across store/CLI/installer.

**Option B:** Reinvent packaging system from scratch.

- Pros: Full control, tailor-made for RatOS.

- Cons: Reinventing dependency management is error-prone, huge time investment, slows adoption.

**Option C:** Adopt Flatpak or Snap wholesale.

- Pros: Modern, sandboxed, manifest-driven.

- Cons: Over-abstraction, dependency bloat, diverges from RatOS’s “clean API” philosophy.

## Consequences

Pros:

- Unified RatOS-native identity for apps.

- Clean UX across store, package manager, and standalone installers.

- Strong security via signing and manifest verification.

- Reuse of robust, battle-tested Linux packaging tools.

Cons:

- Requires RatOS tooling (ratpkg) to wrap around dpkg.

- Adds complexity in maintaining manifest/signature standards.

- Developers must package apps into .ratpkg format (extra step on top of .deb).

### Status & Next Steps

Accepted in principle: .ratpkg will be the canonical format.

Next steps:

- Define manifest.json schema.

- Define cryptographic signing and verification model.

- Implement ratpkg tool to manage .ratpkg lifecycle.

- Create app store frontend and GUI installer to call ratpkg.