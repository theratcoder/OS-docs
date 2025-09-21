# API Reference Template

This document is a **template** for writing RatAPI reference documentation. Use it when documenting new or existing subsystems of RatOS.

---

## 📚 How to Use

* Group functions by **subsystem** (e.g., Process/Thread, File I/O, Memory, IPC).
* For each function, include: description, prototype, parameters, return value, and errors.
* Keep formatting consistent across all subsystems.

---

## 🧵 Subsystem Name (e.g., Process & Thread Management)

### `RatFunctionName`

**Description:** A short explanation of what the function does.

**Prototype:**

```c
ReturnType RatFunctionName(ParamType param1, ParamType param2);
```

**Parameters:**

* `param1`: Explanation of the parameter.
* `param2`: Explanation of the parameter.

**Returns:**

* Success return value.
* Failure return value.

**Errors:**

* `RAT_ERR_EXAMPLE` — Explanation of the error.

---

### `RatAnotherFunction`

**Description:** Another example function.

**Prototype:**

```c
ReturnType RatAnotherFunction(void);
```

**Parameters:**

* None.

**Returns:**

* Success return value.

**Errors:**

* `RAT_ERR_EXAMPLE` — Explanation of the error.

---

## 📂 Additional Subsystems

* Repeat the above structure for File & I/O, Memory Management, IPC, UI/Graphics, etc.

---

## ❌ Error Codes

List all subsystem-specific error codes here:

* `RAT_ERR_EXAMPLE` — Example error definition.

---

*RatOS API Reference Template — version 0.1*