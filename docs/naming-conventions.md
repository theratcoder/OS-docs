# RatAPI Naming Conventions

This document defines naming conventions for RatAPI. Consistency in naming ensures that the API is predictable, easy to read, and avoids clashes with the C standard library or system headers.

---

## 1. Functions

Format: `RatVerbNoun` (PascalCase, prefixed with Rat).

Examples:

* RatCreateThread()

* RatLockMutex()

* RatReadFileAsync()

This style mirrors Win32 while avoiding name collisions and providing a clear RatAPI "namespace."

---

## 2. Types

Format: `RatTypeName` (PascalCase, prefixed with Rat).

Examples:

* RatThread

* RatMutex

* RatFile

* RatStatus

We avoid Hungarian notation (e.g., `HTHREAD`, `HMUTEX`), preferring a modern, consistent approach.

---

## 3. Constants and Enums

Format: RAT_CONSTANT_NAME (ALL_CAPS, prefixed with RAT_).

Examples:

* RAT_OK

* RAT_ERROR_INVALID_PARAMETER

* RAT_WAIT_TIMEOUT

Enums should use PascalCase for type names, but enum values follow this constant style.

---

## 4. Internal Implementation Symbols

**Format:** snake_case.

Examples:

* rat_thread_table

* rat_handle_entry

This clearly separates internal/private identifiers from the public API.

---

## 5. Header Guards

* Use `#pragma once` rather than traditional include guards.

* Cleaner, modern, and supported by all compilers targeted by RatOS.

---

## 6. Booleans

* Use <stdbool.h> from C11.

* Boolean values: true and false.

---

## 7. Null Pointers

* Use NULL consistently.

* RatAPI will be usable from both C and C++, so NULL provides maximum compatibility.

---

## 8. Error Handling

* All API functions that can fail return a RatStatus value.

* RAT_OK indicates success; other values indicate failure or specific error conditions.

* APIs that produce a result additionally return it via output parameters.

* Async APIs follow the same convention with completion callbacks or queues.

---

## 9. Asynchronous Naming

Asynchronous functions use the Async suffix.

Examples:

* `RatReadFileAsync()`

* `RatWriteFileAsync()`

* `RatQueueCompletion()`

---

## 10. General Guidelines

* Do not redefine features already provided by C11 or glibc.

* Keep the RatAPI surface consistent and minimal, avoiding duplicate abstractions.

* When in doubt, prefer clarity over brevity.

---

*RatOS Naming Conventions — version 0.1-draft*