# RatOS API Specification: Error Handling

## Overview

RatOS uses a **single, unified error handling model** across all system APIs. Instead of hidden global state (`errno`, `GetLastError`, thread-local storage), RatOS adopts **explicit return codes** to ensure clarity, safety, and consistency.

This policy applies to **all RatAPI functions**.

---

## Error Codes

* All functions that can fail must return a `RatStatus`.
* `RatStatus` is an enum that encodes all possible outcomes:

  * `RAT_OK` — success.
  * Other values — specific failure modes (e.g. `RAT_ERR_NOT_FOUND`, `RAT_ERR_ACCESS_DENIED`).
* The exact set of error codes will be standardized across modules (file I/O, networking, threading, etc.) with extensions possible per subsystem.

---

## Function Signatures

Functions return a `RatStatus`. Additional data must be returned via output parameters.

### Example

```c
RatStatus RatOpenFile(const char* path, RatFile** out_file);

RatFile* file;
RatStatus st = RatOpenFile("data.txt", &file);
if (st != RAT_OK) {
    // handle error directly, e.g. log or exit
}
```

---

## Design Principles

* **Explicitness:** Errors are visible at call sites, not hidden in thread-local state.
* **Thread-Safety:** No global or implicit state avoids race conditions.
* **Consistency:** One model across the entire API surface.
* **Simplicity:** Easy to reason about and teach.

---

## Consequences

### Advantages

* Developers always know where errors come from.
* No hidden coupling between calls (unlike `GetLastError` or `errno`).
* Safer in multithreaded environments.
* Easier for static analysis tools to detect unchecked errors.

### Trade-offs

* Slightly more verbose code (developers must explicitly check return codes).
* Less convenient than implicit error models for small scripts or prototypes.

---

## Summary

RatOS uses **explicit return codes (`RatStatus`) as the sole error handling mechanism**. No implicit error state will be maintained, ensuring predictable and thread-safe APIs.

---

*RatOS Error Handling Specification — version 0.1-draft*