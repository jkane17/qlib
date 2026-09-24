# Symbols

This document describes the symbol interning functions from [`sym.h`](/src/c/cdk/sym.h).

## Table of Contents

- [Interning](#interning)
- [Functions](#functions)
    - [`qInternString`](#qinternstring)
    - [`qInternFixedString`](#qinternfixedstring)

## Interning

A Q symbol (`QSymbol`) is a pointer to an interned string. kdb+ keeps a single copy of each distinct symbol in a shared symbol table, and every symbol with the same characters points to that copy. This means:

- **Symbols can be compared with `==`.** Two interned symbols are equal exactly when their pointers are equal, so `strcmp` is not needed.
- **Interned symbols are owned by kdb+.** They live for the rest of the process and must not be modified or freed. Symbols are not reference counted, so there is nothing to release.
- **Interning copies the characters.** The string passed in remains the caller's, and changing it afterwards does not change the symbol.

`qNewSymbol` and `qNewSymbolList` intern their arguments automatically. The functions in this document are needed when symbols are written into a list directly, for example into a symbol list allocated with [`qNewList`](/doc/c/creating.md#qnewlist). Every element of a symbol list must be an interned symbol: storing a pointer to an ordinary C string breaks comparisons with `==` and leaves the list pointing at memory kdb+ does not own.

When symbols may be interned from more than one thread, enable the symbol table lock with [`qToggleSymbolLock`](/doc/c/memory.md#qtogglesymbollock).

## Functions

### `qInternString`

Intern a null-terminated string as a symbol.

```c
QSymbol qInternString(const QChar *string);
```

**Parameters**

| Parameter | Description                        |
| --------- | ---------------------------------- |
| `string`  | Null-terminated string to intern   |

**Returns**

The interned symbol.

> Note: `string` must not be `NULL` (checked with `assert`). Interning `""` returns the null symbol.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QChar buffer[] = "hello";

    QSymbol sym0 = qInternString("hello");
    QSymbol sym1 = qInternString(buffer);

    printf("sym0 = %s\n", sym0);
    printf("sym0 == sym1: %d\n", sym0 == sym1);
    printf("sym0 == buffer: %d\n", sym0 == buffer);

    return 0;
}
```

Output:

```
sym0 = hello
sym0 == sym1: 1
sym0 == buffer: 0
```

`sym0` and `sym1` are the same pointer because both intern the characters `hello`. Neither is `buffer`, because interning copies the characters into the symbol table.

### `qInternFixedString`

Intern the first `length` characters of a string as a symbol.

```c
QSymbol qInternFixedString(const QChar *string, QSize length);
```

**Parameters**

| Parameter | Description                                                        |
| --------- | ------------------------------------------------------------------ |
| `string`  | String to intern (need not be null-terminated)                     |
| `length`  | Maximum number of characters to intern                             |

**Returns**

The interned symbol.

> Note: At most `length` characters are read, stopping early at a null character. (kdb+'s underlying `sn` would intern the null and the characters after it, giving a symbol that prints like the prefix but is not equal to it.) `string` may be `NULL` only if `length` is `0`, and `length` must not exceed `INT32_MAX` (both checked with `assert`). A `length` of `0` returns the null symbol.

**Example**

Intern part of a string, and a buffer that is not null-terminated:

```c
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol sym0 = qInternFixedString("hello world", 5);

    QChar buffer[] = {'a', 'b', 'c'};
    QSymbol sym1 = qInternFixedString(buffer, 3);

    printf("sym0 = %s\n", sym0);
    printf("sym1 = %s\n", sym1);
    printf("sym0 == qInternString(\"hello\"): %d\n", sym0 == qInternString("hello"));

    return 0;
}
```

Output:

```
sym0 = hello
sym1 = abc
sym0 == qInternString("hello"): 1
```

## Example: Filling a Symbol List

A symbol list allocated with `qNewList` must be filled with interned symbols:

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    const QChar *csv = "abc,de,fghi";
    QSize lengths[] = {3, 2, 4};

    QObj *list = qNewList(Q_TYPE_SYMBOL, 3);
    QSymbol *syms = (QSymbol *)list->list;

    const QChar *start = csv;
    for (QSize i = 0; i < list->length; i++) {
        syms[i] = qInternFixedString(start, lengths[i]);
        start += lengths[i] + 1; // skip the comma
    }

    for (QSize i = 0; i < list->length; i++)
        printf("sym[%" PRIu64 "] = %s\n", i, qGetSymbolAtIndex(list, i));

    qDecRef(list);
    return 0;
}
```

Output:

```
sym[0] = abc
sym[1] = de
sym[2] = fghi
```
