# List Operations

This document describes the list operation functions from [`list.h`](/src/c/cdk/list.h).

## Table of Contents

- [List Operations](#list-operations)
  - [Table of Contents](#table-of-contents)
  - [Appending](#appending)
  - [Functions](#functions)
    - [`qAppendValue`](#qappendvalue)
    - [`qAppendObject`](#qappendobject)
    - [`qAppendSymbol`](#qappendsymbol)
    - [`qAppendFixedSymbol`](#qappendfixedsymbol)
    - [`qAppendList`](#qappendlist)

## Appending

The append functions add items to the end of an existing list in place. They work the same way:

- **They take the address of the list pointer (`QObj **`).** Appending can reallocate the list, which moves it in memory. The functions write the new pointer back through this argument, so the caller's pointer always refers to the current list. They also return the new pointer, but there is no need to assign it: `qAppendValue(&list, &value);` is enough. Any other copies of the old pointer may be left dangling after a call.
- **The list must not be shared.** The list is modified in place, so it must have a single owner (`refs == 0`, see [Reference Counting](/doc/c/memory.md#reference-counting)). Appending to a list that something else also references changes it for them too, or frees memory they still use when it is reallocated.
- **The list must be the right type.** Each function only accepts the kinds of list given in its description.

The arguments are checked with `assert` in debug builds (see [Usage](/doc/c/README.md#usage)): the list pointer and the list must not be `NULL`, the list must be the right type, and the item being appended must not be `NULL` or the list itself (a `NULL` string is allowed by `qAppendFixedSymbol` when `length` is `0`).

## Functions

### `qAppendValue`

Append a single raw value to a simple list.

```c
QObj *qAppendValue(QObj **list, void *value);
```

**Parameters**

| Parameter | Description                                                                                                                        |
| --------- | ---------------------------------------------------------------------------------------------------------------------------------- |
| `list`    | The address of a pointer to a simple list to append to. Updated in place if the list is reallocated                                |
| `value`   | A pointer to the raw value to append. It must point to a value of the list's element type (for example, a `QLong` for a long list) |

**Returns**

A pointer to the updated list (the same as the new `*list`).

> Note: The value is copied into the list using the size of the list's element type, so `value` must point to a value of exactly that type. Passing a pointer to an `int` when appending to a long list, for example, reads past the end of the `int`. For symbol lists use [`qAppendSymbol`](#qappendsymbol) or [`qAppendFixedSymbol`](#qappendfixedsymbol), which intern the symbol for you. For mixed lists use [`qAppendObject`](#qappendobject).

**Example**

```c
#include <inttypes.h>
#include <stdio.h>

#include "q.h"

int main(void) {
    QLong longs[] = {1, 12, 123};
    QObj *list = qNewLongList(longs, 3);

    QLong newValue = 1234;
    qAppendValue(&list, &newValue);

    for (QSize i = 0; i < list->length; i++)
        printf("list[%" PRIu64 "] = %" PRId64 "\n", i, qGetLongAtIndex(list, i));

    qDecRef(list);
    return 0;
}
```

Output:

```
list[0] = 1
list[1] = 12
list[2] = 123
list[3] = 1234
```

### `qAppendObject`

Append a Q object to a mixed list.

```c
QObj *qAppendObject(QObj **list, QObj *obj);
```

**Parameters**

| Parameter | Description                                                                                        |
| --------- | -------------------------------------------------------------------------------------------------- |
| `list`    | The address of a pointer to a mixed list to append to. Updated in place if the list is reallocated |
| `obj`     | A pointer to the Q object to append (takes ownership). Must not be the list itself                 |

**Returns**

A pointer to the updated list (the same as the new `*list`).

> Note: The list takes ownership of `obj`, so `obj` must not be released by the caller afterwards. It is released when the list is. To keep using `obj` independently of the list, call `qIncRef(obj)` first (see [Ownership](/doc/c/memory.md#ownership)).

**Example**

```c
#include <inttypes.h>
#include <stdio.h>

#include "q.h"

int main(void) {
    // Create the initial mixed list
    QObj *symbol = qNewSymbol("abc");
    QObj *long_ = qNewLong(10);
    QObj *chars = qNewCharListFromString("xyz");
    QObj *mixedList = qNewMixedList(3, symbol, long_, chars);

    // Append a float atom (mixedList takes ownership of it)
    qAppendObject(&mixedList, qNewFloat(3.14));

    printf("mixedList[0] = %s\n", qGetSymbol(qGetMixedAtIndex(mixedList, 0)));
    printf("mixedList[1] = %" PRId64 "\n", qGetLong(qGetMixedAtIndex(mixedList, 1)));

    QObj *charList = qGetMixedAtIndex(mixedList, 2); // borrowed: do not release
    printf("mixedList[2] = %.*s\n", (int)charList->length, (char *)charList->list);

    printf("mixedList[3] = %.2f\n", qGetFloat(qGetMixedAtIndex(mixedList, 3)));

    qDecRef(mixedList); // also releases the items
    return 0;
}
```

Output:

```
mixedList[0] = abc
mixedList[1] = 10
mixedList[2] = xyz
mixedList[3] = 3.14
```

### `qAppendSymbol`

Intern a null-terminated string and append it to a symbol list.

```c
QObj *qAppendSymbol(QObj **list, const QChar *string);
```

**Parameters**

| Parameter | Description                                                                                         |
| --------- | --------------------------------------------------------------------------------------------------- |
| `list`    | The address of a pointer to a symbol list to append to. Updated in place if the list is reallocated |
| `string`  | Null-terminated string to intern and append                                                         |

**Returns**

A pointer to the updated list (the same as the new `*list`).

> Note: `string` is interned before it is appended, in the same way as [`qNewSymbol`](/doc/c/creating.md#qnewsymbol) and [`qNewSymbolList`](/doc/c/creating.md#qnewsymbollist), so it does not need to be interned first. kdb+'s `js`, which this wraps, does not intern: it stores the pointer it is given. Interning a symbol that is already interned returns the same symbol, so a `QSymbol` can also be passed. See [Symbols](/doc/c/symbols.md).

**Example**

```c
#include <inttypes.h>
#include <stdio.h>

#include "q.h"

int main(void) {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    QObj *list = qNewSymbolList(symbols, 3);

    qAppendSymbol(&list, "jkl");

    for (QSize i = 0; i < list->length; i++)
        printf("list[%" PRIu64 "] = %s\n", i, qGetSymbolAtIndex(list, i));

    printf("interned: %d\n", qGetSymbolAtIndex(list, 3) == qInternString("jkl"));

    qDecRef(list);
    return 0;
}
```

Output:

```
list[0] = abc
list[1] = def
list[2] = ghi
list[3] = jkl
interned: 1
```

### `qAppendFixedSymbol`

Intern the first `length` characters of a string and append them to a symbol list.

```c
QObj *qAppendFixedSymbol(QObj **list, const QChar *string, QSize length);
```

**Parameters**

| Parameter | Description                                                                                         |
| --------- | --------------------------------------------------------------------------------------------------- |
| `list`    | The address of a pointer to a symbol list to append to. Updated in place if the list is reallocated |
| `string`  | String to intern and append (need not be null-terminated). May be `NULL` if `length` is `0`         |
| `length`  | Maximum number of characters to intern (must not exceed `INT32_MAX`)                                |

**Returns**

A pointer to the updated list (the same as the new `*list`).

> Note: Reads at most `length` characters, stopping early at a null character, in the same way as [`qInternFixedString`](/doc/c/symbols.md#qinternfixedstring). A `length` of `0` appends the null symbol. Use this to append part of a larger buffer, such as a field of a delimited string, without copying it into a null-terminated string first. As with [`qAppendSymbol`](#qappendsymbol), the string is interned before it is appended.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>

#include "q.h"

int main(void) {
    // Split a comma-separated string into a symbol list
    const QChar *text = "abc,de,,fghi";
    QObj *list = qNewSymbolList(NULL, 0);

    const QChar *start = text;
    for (const QChar *p = text;; p++) {
        if (*p == ',' || *p == '\0') {
            qAppendFixedSymbol(&list, start, (QSize)(p - start));
            if (*p == '\0')
                break;
            start = p + 1;
        }
    }

    for (QSize i = 0; i < list->length; i++)
        printf("list[%" PRIu64 "] = `%s\n", i, qGetSymbolAtIndex(list, i));

    qDecRef(list);
    return 0;
}
```

Output:

```
list[0] = `abc
list[1] = `de
list[2] = `
list[3] = `fghi
```

### `qAppendList`

Append the items of one list to another list of the same type.

```c
QObj *qAppendList(QObj **left, QObj *right);
```

**Parameters**

| Parameter | Description                                                                                                            |
| --------- | ---------------------------------------------------------------------------------------------------------------------- |
| `left`    | The address of a pointer to a simple or mixed list to append to. Updated in place if the list is reallocated           |
| `right`   | A pointer to a list of the same type as `left` whose items are appended (borrowed). Must not be the `left` list itself |

**Returns**

A pointer to the updated list (the same as the new `*left`).

> Note: `right` is borrowed, not consumed: the caller still owns it and must release it with `qDecRef` when it is no longer needed. When both lists are mixed lists, the items of `right` are shared with `left` (their reference counts are incremented), so releasing `right` does not affect `left`.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>

#include "q.h"

int main(void) {
    QLong leftLongs[] = {1, 12, 123};
    QObj *list = qNewLongList(leftLongs, 3);

    QLong rightLongs[] = {4, 45, 456};
    QObj *right = qNewLongList(rightLongs, 3);

    qAppendList(&list, right);
    qDecRef(right); // still owned by the caller

    for (QSize i = 0; i < list->length; i++)
        printf("list[%" PRIu64 "] = %" PRId64 "\n", i, qGetLongAtIndex(list, i));

    qDecRef(list);
    return 0;
}
```

Output:

```
list[0] = 1
list[1] = 12
list[2] = 123
list[3] = 4
list[4] = 45
list[5] = 456
```
