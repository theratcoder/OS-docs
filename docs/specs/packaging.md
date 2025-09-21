# RatOS Packaging Specification (.ratpkg)
## Overview

RatOS introduces a native application package format (.ratpkg) to unify three installation paths:

- App Store (GUI discovery/installation).

- Package Manager (ratpkg CLI).

- Standalone Installer (double-click .ratpkg).

All three use the same underlying format and lifecycle, ensuring consistency and reducing fragmentation.

---

## Goals

- Provide a unified identity for RatOS applications.

- Support secure installation via signatures.

- Include metadata (permissions, icons, descriptions).

- Auto-clean packages after installation to avoid clutter.

- Leverage proven Linux tools (dpkg) for dependency resolution.

---

## File Format

A .ratpkg is a compressed archive (ZIP initially, later potentially Rat-native container) with the following structure:
```
myapp.ratpkg
 ├── manifest.json         # Required metadata
 ├── signature             # Cryptographic signature
 ├── payload/              # App payload (usually a .deb)
 │    └── myapp.deb
 ├── icons/                # Optional app icons (various sizes)
 └── resources/            # Optional, e.g. screenshots, docs
```

---

## Manifest Specification (manifest.json)

**Example:**

```json
{
  "name": "MyApp",
  "id": "com.example.myapp",
  "version": "1.2.3",
  "architecture": "amd64",
  "description": "A sample RatOS application.",
  "author": "Example Corp",
  "license": "MIT",
  "homepage": "https://example.com",
  "permissions": [
    "network",
    "filesystem",
    "camera"
  ],
  "entrypoint": "/usr/bin/myapp",
  "icons": {
    "48x48": "icons/icon-48.png",
    "128x128": "icons/icon-128.png"
  }
}
```
**Required fields:**

- `name`: Human-readable app name.

- `id`: Globally unique identifier (reverse domain style).

- `version`: Semantic version.

- `architecture`: Target CPU architecture (amd64, arm64, etc.).

- `description`: Short description of the app.

- `author`: Name of app publisher.

- `entrypoint`: Executable path after install.

**Optional fields:**

- `license`, `homepage`.

- `permissions`: List of required capabilities.

- `icons`: Maps icon sizes to files.

- `resources`: Additional assets (docs/screenshots).

**Signature:**

- `signature` file contains cryptographic signature of the archive.

- Signed with the publisher’s private key.

- Verified at install time against trusted certificate store.

- If verification fails, installation is aborted.

---

## Installation Lifecycle

1. Verify signature: Ensure .ratpkg is signed by a trusted key.

2. Read manifest: Collect metadata, permissions, and entrypoint.

3. Check dependencies: Pass payload (.deb) to dpkg for resolution.

4. Install payload: Use system package manager backend.

**Register app:**

- Add entry to RatOS app registry.

- Store metadata (icons, description) for App Store and menu.

**Cleanup:** Delete .ratpkg by default after successful install (overridable by flag).

**CLI Tool:** ratpkg

**Examples:**
```bash
ratpkg install myapp.ratpkg
ratpkg verify myapp.ratpkg
ratpkg info myapp.ratpkg
ratpkg remove com.example.myapp
```

---

## Integration Points

- App Store: GUI frontend → calls ratpkg install.

- Standalone Installer: Double-click → GUI installer → runs ratpkg.

- Package Manager: CLI tool for devs and power users.

---

## Future Extensions

- Support for Flatpak/AppImage payloads in addition to .deb.

- Sandboxing/runtime permission enforcement.

- Delta updates (smaller patches instead of full installs).

- Multiple architectures in one .ratpkg.

- App Store metadata sync (ratings, reviews, categories).

---

*RatOS Packaging — version 0.1-draft*