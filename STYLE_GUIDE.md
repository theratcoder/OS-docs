# Style Guide

This document defines the **coding and documentation conventions** for RatOS. Following these guidelines ensures consistency, readability, and maintainability across the project.

---

## 📝 Documentation Style

* Use **Markdown** for all documentation.
* Keep line length under **100 characters** where possible.
* Use `CONSTANT_CASE` for standard document file names (e.g., `CODE_OF_CONDUCT.md`), except for `README.md`
* Use `kebab-case` for other document file names
* Write in clear, concise English.
* Use gender-neutral language.
* Define all acronyms on first use (see [glossary.md](./docs/glossary.md)).

### Formatting

* Headings: `# H1`, `## H2`, `### H3` (no deeper unless necessary).
* Use bullet points (`-`) for lists.
* Use fenced code blocks with language hints (e.g., `c`, `bash`).
* Prefer relative links (`./file-name.md`) for cross-references.

---

## 💻 Code Style

### Languages

* **C** for low-level RatOS Core Services.
* **C11 standard** (with GNU extensions allowed where necessary).
* **POSIX-compliant** code where possible, unless overridden by RatAPI abstractions.

### Formatting

* Indentation: tabs.
* Braces: [Stroustrup style](https://en.wikipedia.org/wiki/Indentation_style#:~:text=Allman%20style.-,Stroustrup,-%5Bedit%5D)

  ```c
  if (condition) {
      do_something();
  }
  else {
      do_something_else();
  }
  ```
* Limit functions to a single responsibility where possible.

### Naming Conventions

* **Functions**: `RatFunctionName()` for public RatAPI, `rat_internal_function()` for internal.
* **Variables**: `snake_case` for locals, `g_global_variable` for globals.
* **Constants / Macros**: `ALL_CAPS_WITH_UNDERSCORES`.
* **Types / Structs**: `RatTypeName`.

### Comments

* Use `//` for single-line comments, `/* ... */` for multi-line.
* Keep comments **focused on intent**, not just describing the code.
* Document all public RatAPI functions with:

  * Purpose
  * Parameters
  * Return values
  * Error cases

---

## 🔧 Git & Workflow

* Write **clear, imperative commit messages**:

  * ✅ `Add process manager subsystem`
  * ❌ `Added process manager subsystem`
* Reference issues/PRs where applicable.
* Keep PRs focused and atomic.
* Document major decisions in an **ADR**.

---

*RatOS Style Guide — version 0.1-draft*