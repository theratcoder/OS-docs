# ADR-0012: SDK and Development Workflow

## Status
Proposed

---

## Context

For RatOS to gain adoption, developers must have a reliable way to write and build applications against RatAPI. This requires a Software Development Kit (SDK) that defines how developers interact with the OS, both during compilation and runtime.

Key questions include:

- What should the SDK contain?

- Should developers cross-compile from existing platforms, or develop natively within RatOS?

- How should tooling and IDE support evolve?

- How do we ensure ABI/API stability over time?

## Options Considered

### A: Native Development First

Port GCC/Clang and related toolchains to RatOS immediately.

Developers build apps inside RatOS directly.

Pro: Purist/self-hosting approach.

Con: Requires significant work upfront, slows developer adoption, fragile in early stages.

### B: Cross-Compilation First

Provide RatAPI headers, libraries, and toolchain files so developers can build RatOS apps from Linux/Mac/Windows.

RatOS remains the runtime environment, not the primary dev environment.

Pro: Easiest for early adoption, leverages existing compilers and IDEs.

Con: Delays self-hosting, developers depend on external systems.

### C: Hybrid (Cross-Compile First, Native Later)

Start with cross-compilation as the main workflow.

Gradually port GCC/Clang and build tools into RatOS, enabling native builds later.

Pro: Fast adoption, but allows eventual self-hosting.

Con: Requires roadmap discipline, potential duplication of effort.

---

## Decision

RatOS will adopt Option C (Hybrid Model).

### Initial Phase (Cross-Compilation)

Provide:

- RatAPI headers (ratapi.h, etc.)

- Shared libraries (libratapi.so, libratcontrols.so, etc.)

- Toolchain definitions (CMake files, pkg-config, sysroot).

- Distribute SDK packages for Linux, macOS, and Windows (via MinGW/Clang).

- Offer sample projects and integration with VSCode and CMake.

### Growth Phase (Native Development)

- Port GCC/Clang and binutils to RatOS.

- Provide rat-sdk package inside RatOS (headers, libs, build tools).

- Allow developers to compile apps natively inside RatOS.

### Long-Term (Enhanced Tooling)

- Offer IDE integrations (VSCode extensions, CLion support).

- Explore a dedicated RatOS IDE with a Visual Studio–like experience.

- Maintain ABI/API stability using versioned shared libraries (e.g. libratapi.so.1).

## Rationale

Cross-compilation ensures developers can start building apps early without RatOS needing to be fully self-hosting.

Native development is a milestone of OS maturity, and essential for long-term credibility.

Leveraging existing compilers avoids unnecessary reinvention while allowing RatOS to focus on the SDK and developer experience.

Clear versioning and stable ABI guarantees developer trust.

## Consequences

Developers can start experimenting with RatAPI apps almost immediately using existing platforms.

RatOS will need to maintain SDK distribution packages for multiple host platforms.

Native self-hosting will take longer but will mark a key milestone in RatOS maturity.

Additional work will be needed to provide IDE integration and documentation.