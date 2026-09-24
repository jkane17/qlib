# Errors

This document describes the error functions from [`err.h`](/src/c/cdk/err.h).

## How Errors Work

kdb+ signals an error from C by returning a null pointer after setting an error message in an error indicator. There are two indicators:

- the **global** error indicator, for Q errors such as `type` or `length`, and
- the **system** error indicator, for operating system errors (the message is reported together with the OS error).

When a C function called from Q returns a null pointer, Q raises the error held in the indicator. Within C, the error can be retrieved as an error object (type `Q_TYPE_ERROR`, `-128`) using `qCheckError`.

## Functions

### `qNewError`

```c
QObj *qNewError(const QChar *error);
```

Set the global error indicator to `error` and return a null pointer. Typically used as a return value:

```c
if (!qIsLong(arg))
    return qNewError("type");
```

### `qNewOSError`

```c
QObj *qNewOSError(const QChar *error);
```

Set the system error indicator to `error` and return a null pointer.

### `qCheckError`

```c
QObj *qCheckError(QObj *obj);
```

If `obj` is a null pointer, return an error object containing the current error message and clear the error indicator. Otherwise, return `obj` unchanged.

The caller owns the returned error object and should release it with `decRef` when finished with it.

Because the indicator is cleared, call `qCheckError` immediately after the call that may have failed. If several failing calls are made first, only the last error is available, and a second `qCheckError` finds no error.

### `qIsError`

```c
bool qIsError(const QObj *obj);
```

Return `true` if `obj` is an error object (type `Q_TYPE_ERROR`). Returns `false` for a `NULL` pointer, so check the result of `qCheckError` rather than the original return value.

### `qGetError`

```c
const QChar *qGetError(const QObj *obj);
```

Return the message of an error object, or an empty string if `obj` is not an error object.

## Example

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *list = qNewLongList(NULL, 3); // NULL values with a non-zero length

    QObj *result = qCheckError(list);
    if (qIsError(result))
        printf("error: %s\n", qGetError(result));

    return 0;
}
```

Output:

```
error: domain
```

## Errors Returned by This Library

| Error    | Cause                                                                                      |
| -------- | ------------------------------------------------------------------------------------------ |
| `domain` | A required pointer argument is `NULL`, or a length or count is outside the supported range |
| `type`   | An argument has the wrong Q type (e.g. a table header that is not a symbol list)           |
| `length` | Arguments that must have the same count do not (e.g. dictionary keys and values)           |

Errors raised by kdb+ itself (for example, by the functions used to build tables) are passed through unchanged. See [`QObj` Creation](/doc/c/creating.md) for the errors each creation function can return.
