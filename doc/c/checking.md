# `QObj` Type Checking

This document describes the type checking functions from [`is.h`](/src/c/cdk/is.h). These functions report what kind of value a Q object holds, and should be used before calling an [accessor](/doc/c/accessing.md) on an object whose type is not already known.

All type checking functions:

- take a `const QObj *` and return `bool`,
- return `false` for a `NULL` pointer, and
- are safe to call on an object of any type.

## Table of Contents

- [Atoms](#atoms)
- [Lists](#lists)
- [Complex Types](#complex-types)
- [Errors](#errors)
- [Example](#example)

## Atoms

| Function                                   | Returns `true` for                              |
| ------------------------------------------ | ----------------------------------------------- |
| `qIsAtom(obj)`                             | Any atom (negative type code)                   |
| `qIsAtomType(obj, type)`                   | An atom of `type` (e.g. `Q_TYPE_LONG`)          |
| `qIsBoolean`, `qIsGuid`, `qIsByte`, ...    | An atom of that type                            |

There is one function per primitive type: `qIsBoolean`, `qIsGuid`, `qIsByte`, `qIsShort`, `qIsInt`, `qIsLong`, `qIsReal`, `qIsFloat`, `qIsChar`, `qIsSymbol`, `qIsTimestamp`, `qIsMonth`, `qIsDate`, `qIsDatetime`, `qIsTimespan`, `qIsMinute`, `qIsSecond`, and `qIsTime`.

`qIsAtomType` takes the positive type code of the atom (the same code as the corresponding list):

```c
qIsAtomType(obj, Q_TYPE_LONG); // true for a long atom (type -7)
```

> Note: An error object (type `-128`) also has a negative type code, so `qIsAtom` returns `true` for it. Use `qIsError` to distinguish errors.

## Lists

| Function                                           | Returns `true` for                                                        |
| -------------------------------------------------- | ------------------------------------------------------------------------- |
| `qIsList(obj)`                                     | Any list: mixed (`0`), simple (`1`-`19`), enumerated (`20`-`76`), or mapped/nested (`77`-`97`) |
| `qIsSimpleList(obj)`                               | A simple list of one of the primitive types (`1`-`19`)                    |
| `qIsListType(obj, type)`                           | A list of `type` (any list type code `0`-`97`)                            |
| `qIsBooleanList`, `qIsGuidList`, `qIsByteList`, ... | A simple list of that type                                               |

There is one function per primitive type, named after the atom functions: `qIsBooleanList`, `qIsGuidList`, `qIsByteList`, `qIsShortList`, `qIsIntList`, `qIsLongList`, `qIsRealList`, `qIsFloatList`, `qIsCharList`, `qIsSymbolList`, `qIsTimestampList`, `qIsMonthList`, `qIsDateList`, `qIsDatetimeList`, `qIsTimespanList`, `qIsMinuteList`, `qIsSecondList`, and `qIsTimeList`.

Tables (`98`) and dictionaries (`99`) are not lists: `qIsList` returns `false` for them, and `qIsListType` returns `false` when `type` is `Q_TYPE_TABLE` or `Q_TYPE_DICTIONARY`.

## Complex Types

| Function            | Returns `true` for                                                   |
| ------------------- | -------------------------------------------------------------------- |
| `qIsMixedList(obj)` | A mixed list (type `0`)                                              |
| `qIsDict(obj)`      | A dictionary (type `99`) that is not a keyed table                   |
| `qIsTable(obj)`     | A table (type `98`)                                                  |
| `qIsKeyedTable(obj)`| A dictionary (type `99`) whose keys and values are both tables       |

A dictionary and a keyed table share type code `99`, so exactly one of `qIsDict` and `qIsKeyedTable` is `true` for a type `99` object.

## Errors

```c
bool qIsError(const QObj *obj);
```

Declared in [`err.h`](/src/c/cdk/err.h). Returns `true` for an error object (type `Q_TYPE_ERROR`), as returned by `qCheckError`. See [Errors](/doc/c/errors.md).

## Example

```c
#include <stdio.h>
#include "q.h"

static const char *describe(const QObj *obj) {
    if (!obj)
        return "null pointer";
    if (qIsError(obj))
        return "error";
    if (qIsAtom(obj))
        return "atom";
    if (qIsSimpleList(obj))
        return "simple list";
    if (qIsMixedList(obj))
        return "mixed list";
    if (qIsList(obj))
        return "other list";
    if (qIsTable(obj))
        return "table";
    if (qIsKeyedTable(obj))
        return "keyed table";
    if (qIsDict(obj))
        return "dictionary";
    return "other";
}

int main() {
    QLong longs[] = {1, 2, 3};
    QObj *atom = qNewLong(42);
    QObj *list = qNewLongList(longs, 3);
    QObj *mixed = qNewMixedList(2, qNewLong(1), qNewChar('a'));

    printf("%s\n", describe(atom));  // atom
    printf("%s\n", describe(list));  // simple list
    printf("%s\n", describe(mixed)); // mixed list

    decRef(atom);
    decRef(list);
    decRef(mixed);
    return 0;
}
```

Output:

```
atom
simple list
mixed list
```
