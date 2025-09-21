# Contributing to RatOS

Thank you for your interest in contributing to **RatOS**! 🎉
This document explains how to propose changes, write documentation, and participate in discussions.

---

## 📖 Ways to Contribute

* **Documentation**: improve design docs, specs, examples, or glossary entries.
* **Design**: propose new features or refinements via ADRs (Architectural Decision Records).
* **Code**: contribute prototypes, examples, or tooling once RatAPI is stabilized.
* **Feedback**: review proposals, open issues, or suggest improvements.

---

## 📝 Workflow

1. **Fork and branch**

   * Fork the repo and create a feature branch: `git checkout -b feature/my-change`.

2. **Make your changes**

   * Follow the style guidelines below.
   * Keep PRs focused on a single topic.

3. **Propose your change**

   * Open a Pull Request (PR).
   * If it’s a design decision, add an ADR in `docs/adr/`.
   * Use the templates in `docs/templates/` (ADR, spec, PR).

4. **Discussion & review**

   * Maintainers and community will review.
   * Expect feedback and iteration before merging.

---

## 📂 Documentation Standards

* Use **Markdown** with clear headings and code fences.
* Line length: \~80–100 chars where possible.
* Use fenced code blocks with language tags:
  \`\`\`c
  RAT\_STATUS RatCreateProcess(...);
  \`\`\`
* Diagrams: ASCII art is fine, link to images/SVGs if needed.
* Keep language concise and neutral.

---

## 🧩 ADRs (Architectural Decision Records)

* Required for major changes (API conventions, packaging format, security model, etc.).
* Each ADR should:

  * Live in `docs/adr/`.
  * Use the `adr` template from `docs/templates/`.
  * Be numbered sequentially (`0001-...`, `0002-...`).
  * Status field: Proposed, Accepted, Rejected, Superseded.

---

## ✅ Pull Requests

* Use the PR template in `docs/templates/pr-template.md`.
* Reference related ADRs or issues.
* Ensure Markdown lints cleanly (`markdownlint`).
* Check spelling (`codespell` or similar) before submitting.

---

## 🤝 Community Guidelines

* Be respectful and constructive in all discussions.
* Follow the [Code of Conduct](CODE_OF_CONDUCT.md).
* Assume good intent; clarify misunderstandings before escalating.

---

## 🔍 Tips for Contributors

* Read `docs/overview.md` and `docs/roadmap.md` before proposing changes.
* Check the **decision backlog** in `docs/` for open questions.
* Keep specs modular: one subsystem per document.
* Use examples liberally — they help clarify API intent.

---

*Contributing guide — version 0.1-draft*