# ADR-0010: Error Handling Policy

## Status

Accepted

---

## Context

Error handling in operating systems and system libraries is notoriously inconsistent.

Windows (Win32) uses return values for success/failure and a thread-local GetLastError() for details.

POSIX/Linux APIs typically return -1/NULL and set the global errno.

Many higher-level systems use exceptions or structured error objects.

Each model has trade-offs, but RatOS must prioritize clarity, consistency, and safety in its core API.

---

## Decision

RatOS will adopt a pure return code model for error handling:

- Every function that can fail must return a RatStatus (enum).

- A return of RAT_OK indicates success.

- Any other value indicates a specific failure.

- If additional detail is required (e.g. bytes written, pointer results), it must be returned via output parameters.

- No global state (errno, GetLastError, thread-local storage) will be used.

---

## Example
```c
RatStatus RatOpenFile(const char* path, RatFile** out_file);

RatFile* file;
RatStatus st = RatOpenFile("data.txt", &file);
if (st != RAT_OK) {
    // handle error directly
}
```

---

## Consequences

Pros:

- Explicit and unambiguous error handling.

- Thread-safe without extra complexity.

- Consistent across all RatAPI functions.

- Easier to reason about in large systems.

Cons:

- More verbose than implicit error models.

- Requires developers to check every return value explicitly.

- Less “convenient” for quick prototyping.

---

## Alternatives Considered

- Windows-style last error (GetLastError): Rejected due to hidden state, thread-local complexity, and bug-prone nature.

- POSIX errno: Rejected for similar reasons.

- Hybrid (return + last error): Rejected for inconsistency and confusion.

- Exceptions (language-level): Rejected as RatAPI must remain C-compatible.

- Explicit error objects: Rejected as too verbose for low-level system APIs.

---

## Final Choice

RatOS will use explicit return codes (RatStatus) as the sole error handling mechanism.