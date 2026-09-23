# Memory

This document describes the memory and reference-counting functions from [`mem.h`](/src/c/cdk/mem.h).

## Reference Counting

Q objects are reference counted. Each `QObj` has a `refs` field that counts its *additional* references:

- a newly created object has `refs == 0` (a single owner),
- `incRef` adds a reference, and
- `decRef` removes one. When `decRef` is called on an object whose count is `0`, the object is freed, and any objects it contains (list items, dictionary keys and values, table columns) are released in turn.

Whoever holds a reference is responsible for releasing it exactly once. Releasing an object too many times, or using it after its last reference has been released, is undefined behaviour and typically crashes.

### `incRef`

```c
void incRef(QObj *obj);
```

Increment the reference count of `obj`.

### `decRef`

```c
void decRef(QObj *obj);
```

Decrement the reference count of `obj`, freeing it if the count was `0`.

## Ownership

Functions either **take ownership** of an argument or **borrow** it:

- A function that takes ownership (for example, `qNewDict` takes ownership of its keys and values) becomes responsible for releasing the argument. The caller must not use or release that argument afterwards. The functions in this library that take ownership release the argument even if they fail.
- A function that borrows an argument (for example, the type checking functions and accessors) does not change its reference count.
- Accessors that return a `QObj *` (for example, `qGetDictKeys`) return a borrowed pointer to an object inside the containing object. Do not release it.

To keep using an object after giving ownership of it away, take an extra reference first:

```c
QObj *keys = qNewSymbolList(symbols, 3);
incRef(keys);                          // keep a reference for ourselves
QObj *dict = qNewDict(keys, values);   // dict takes ownership of one reference

// ... keys is still valid here ...

decRef(keys);                          // release our reference
decRef(dict);                          // release dict (and the reference it holds to keys)
```

The documentation for each function states whether it takes ownership of its arguments.

## Other Functions

### `freePool`

```c
void freePool();
```

Release the memory allocated for the current thread's pool. Call this when a thread that has created Q objects is about to finish.

### `toggleSymbolLock`

```c
void toggleSymbolLock(QInt mode);
```

Set whether interning symbols uses a lock: `1` enables the lock, `0` disables it. Enable it when symbols may be created from more than one thread.

### `memoryStats`

```c
QObj *memoryStats(QInt mode);
```

Return memory statistics as a long list:

| `mode` | Result                                                                                      |
| -----: | ------------------------------------------------------------------------------------------- |
|    `0` | Memory usage for the current thread: bytes allocated, bytes available in the heap, and maximum heap size so far (3 longs) |
|    `1` | Symbol statistics: number of interned symbols and their memory usage (2 longs; null if not called on the main thread) |

The caller owns the returned list and must release it with `decRef`.
