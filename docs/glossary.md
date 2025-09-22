# Glossary

This glossary collects important terms, acronyms, and concepts used throughout the RatOS project. The goal is to maintain a **single source of truth** for terminology so that documentation, discussions, and code remain consistent.

---

## 📖 How to Use

* When introducing a new term or acronym in documentation, ensure it is defined here.
* Keep definitions **clear, concise, and beginner-friendly**.
* Link back to this document where appropriate.

---

## 📚 Terms

### API

**Application Programming Interface** — A defined set of functions and conventions that applications can use to interact with RatOS system services.

### ADR

**Architectural Decision Record** — A short document that captures an important architectural or design decision, along with its context and consequences.

### RatAPI

The **Win32-esque API layer** unique to RatOS, providing a simplified and consistent developer interface across system services.

### Kernel

The **Linux kernel**, which RatOS builds upon, providing low-level hardware interaction, process management, and system resources.

### POSIX

**Portable Operating System Interface** — A family of standards specified by the IEEE to maintain compatibility between operating systems.

### SDK

**Software Development Kit** - A bundle of software tools, libraries, documentation, and sample code that allows developers to create applications for a specific platform, operating system, or programming language. By providing these pre-built components and resources, an SDK streamlines the development process, enabling developers to focus on unique features rather than reinventing standard functionalities. RatOS will offer an SDK that allows developers to build apps agains RatAPI.

### Syscall

A **system call**, i.e., the mechanism through which user applications request services from the operating system kernel.

---

## 📌 Contribution Notes

* New terms should be added in **alphabetical order**.
* If a term overlaps with existing definitions (e.g., POSIX, syscall), clarify how it applies specifically in RatOS.

---

*RatOS Glossary — version 0.1-draft*