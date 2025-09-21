# RatOS Documentation

Welcome to the **RatOS documentation repository**. RatOS is a Linux-based operating system that exposes its own **Win32-inspired API** (RatAPI) to simplify application development. This documentation is the canonical source for architecture decisions, specifications, API references, and examples.

---

## 📖 What is RatOS?

RatOS is built on the Linux kernel but diverges from traditional UNIX/POSIX in that it:

* Provides a consistent, handle-based API surface for developers.
* Simplifies common system tasks (process management, I/O, IPC, windowing).
* Offers a developer-friendly interface, inspired by the clarity of Win32, while remaining modern and extensible.

This project is in the design and prototyping phase. The documentation here evolves alongside the OS.

---

## 📂 Repository Layout

```
/docs/
  README.md                # This file (index)
  overview.md              # High-level philosophy & goals
  roadmap.md               # Milestones & planned features
  glossary.md              # Terminology
  adr/                     # Architectural Decision Records
  specs/                   # Subsystem specifications
  templates/               # Templates for specs, ADRs, PRs
  examples/                # Example code using RatAPI
```

Additional repo-level files:

* **CONTRIBUTING.md** — guidelines for contributing to docs and proposals.
* **CODE\_OF\_CONDUCT.md** — community standards.
* **CHANGELOG.md** — tracks changes to documentation & design.

---

## 🚀 Getting Started

1. **Read the [overview.md](overview.md)** to understand RatOS goals and architecture.
2. **Check the [roadmap.md](roadmap.md)** for upcoming milestones.
3. **Explore [specs/](specs/)** for details of subsystems (process/thread, memory, file I/O, networking, IPC, windowing, etc.).
4. **Look at [adr/](adr/)** to see architectural decisions and their rationale.
5. **Browse [examples/](examples/)** for API usage samples.

---

## 🧩 How to Contribute

* Proposals for major changes should be written as **ADRs** (see `adr/README.md`).
* Follow the templates in `templates/` when writing specs or RFCs.
* Open pull requests to suggest edits, improvements, or new documentation.
* All contributions are welcome: design ideas, API feedback, diagrams, or examples.

---

## 📌 Status

RatOS is in **early design**. Many decisions (API conventions, window server, security model, packaging format) are under discussion. See the [decision backlog in the skeleton doc](../RatOS%20Design%20Docs%20-%20Skeleton) for open questions.

---

## 🔗 Quick Links

* [Overview](overview.md)
* [Roadmap](roadmap.md)
* [Glossary](glossary.md)
* [ADR index](adr/README.md)
* [Subsystem specs](specs/)
* [Examples](examples/)

---

*RatOS Documentation — version 0.1-draft*