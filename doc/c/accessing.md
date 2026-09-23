# `QObj` Access

This document describes the accessor functions from [`get.h`](/src/c/cdk/get.h). These functions read the values stored in Q objects: atom values, list items, and the parts of dictionaries, tables, and keyed tables.

## Table of Contents

- [Conventions](#conventions)
- [Atom Accessors](#atom-accessors)
- [List Accessors](#list-accessors)
- [Complex Accessors](#complex-accessors)
    - [Mixed Lists](#mixed-lists)
    - [Dictionaries](#dictionaries)
    - [Tables](#tables)
    - [Keyed Tables](#keyed-tables)
- [Generic Accessors](#generic-accessors)
    - [`qGetCount`](#qgetcount)
- [Example](#example)

## Conventions

**Check the type first.** Accessors read directly from the object's storage and do not check its type in release builds. Use the functions in [Type Checking](/doc/c/checking.md) to confirm the type of an object before accessing it, unless the type is already known.

**Debug checks.** In builds without `NDEBUG` defined, every accessor `assert`s that:

- the object is not `NULL`,
- the object has exactly the type the accessor expects (e.g. `qGetLong` requires a long atom, type `-7`), and
- for the `...AtIndex` accessors, `index` is less than the list's length.

A failed check aborts the program with a message naming the accessor. The checks are strict: types that share a C representation are not interchangeable, so reading a timestamp atom with `qGetLong` fails the check. Use the accessor that matches the type (`qGetTimestamp`). Release builds (`build.sh --release` defines `NDEBUG`) contain no checks.

**Borrowed results.** Accessors that return a `QObj *` return a pointer to an object owned by the object being accessed. The reference count is not incremented, so the result must not be released with `decRef`, and it is only valid while the containing object is alive. Call `incRef` on it to keep it beyond that (see [Memory](/doc/c/memory.md)).

**Const.** Accessors take a `const QObj *` and do not modify the object.

## Atom Accessors

Each atom accessor returns the value of an atom of one type.

| Function        | Q type    | Return type     | Notes                                           |
| --------------- | --------- | --------------- | ----------------------------------------------- |
| `qGetBoolean`   | boolean   | `QBoolean`      | `0` or `1`                                      |
| `qGetGuid`      | guid      | `const QGuid *` | Pointer to the 16 bytes stored in the object    |
| `qGetByte`      | byte      | `QByte`         |                                                 |
| `qGetShort`     | short     | `QShort`        |                                                 |
| `qGetInt`       | int       | `QInt`          |                                                 |
| `qGetLong`      | long      | `QLong`         |                                                 |
| `qGetReal`      | real      | `QReal`         |                                                 |
| `qGetFloat`     | float     | `QFloat`        |                                                 |
| `qGetChar`      | char      | `QChar`         |                                                 |
| `qGetSymbol`    | symbol    | `QSymbol`       | Interned string; do not modify or free          |
| `qGetTimestamp` | timestamp | `QTimestamp`    | Nanoseconds since 2000.01.01D00:00:00           |
| `qGetMonth`     | month     | `QMonth`        | Months since 2000.01                            |
| `qGetDate`      | date      | `QDate`         | Days since 2000.01.01                           |
| `qGetDatetime`  | datetime  | `QDatetime`     | Fractional days since 2000.01.01                |
| `qGetTimespan`  | timespan  | `QTimespan`     | Nanoseconds                                     |
| `qGetMinute`    | minute    | `QMinute`       | Minutes since midnight                          |
| `qGetSecond`    | second    | `QSecond`       | Seconds since midnight                          |
| `qGetTime`      | time      | `QTime`         | Milliseconds since midnight                     |

All atom accessors have the form:

```c
QLong qGetLong(const QObj *obj);
```

See [Special Values](/doc/c/type.md#special-values) for the null and infinity values an atom may hold.

## List Accessors

Each list accessor returns the item at `index` of a simple list of one type.

| Function               | Q type    | Return type     |
| ---------------------- | --------- | --------------- |
| `qGetBooleanAtIndex`   | boolean   | `QBoolean`      |
| `qGetGuidAtIndex`      | guid      | `const QGuid *` |
| `qGetByteAtIndex`      | byte      | `QByte`         |
| `qGetShortAtIndex`     | short     | `QShort`        |
| `qGetIntAtIndex`       | int       | `QInt`          |
| `qGetLongAtIndex`      | long      | `QLong`         |
| `qGetRealAtIndex`      | real      | `QReal`         |
| `qGetFloatAtIndex`     | float     | `QFloat`        |
| `qGetCharAtIndex`      | char      | `QChar`         |
| `qGetSymbolAtIndex`    | symbol    | `QSymbol`       |
| `qGetTimestampAtIndex` | timestamp | `QTimestamp`    |
| `qGetMonthAtIndex`     | month     | `QMonth`        |
| `qGetDateAtIndex`      | date      | `QDate`         |
| `qGetDatetimeAtIndex`  | datetime  | `QDatetime`     |
| `qGetTimespanAtIndex`  | timespan  | `QTimespan`     |
| `qGetMinuteAtIndex`    | minute    | `QMinute`       |
| `qGetSecondAtIndex`    | second    | `QSecond`       |
| `qGetTimeAtIndex`      | time      | `QTime`         |

All list accessors have the form:

```c
QLong qGetLongAtIndex(const QObj *obj, QSize index);
```

`index` must be less than `obj->length`. The number of items in a list is available as `obj->length` (or `qGetCount(obj)`).

## Complex Accessors

### Mixed Lists

```c
QObj *qGetMixedAtIndex(const QObj *obj, QSize index);
```

Returns the item at `index` of a mixed list (type `0`) as a borrowed `QObj *`.

### Dictionaries

| Function         | Returns                                                     |
| ---------------- | ----------------------------------------------------------- |
| `qGetDictKeys`   | The keys (borrowed `QObj *`)                                |
| `qGetDictValues` | The values (borrowed `QObj *`)                              |
| `qGetDictCount`  | The number of key-value pairs (`QSize`)                     |

These accept any dictionary (type `99`), including a keyed table.

### Tables

| Function               | Returns                                                                |
| ---------------------- | ---------------------------------------------------------------------- |
| `qGetTableHeader`      | The column names, as a symbol list (borrowed `QObj *`)                 |
| `qGetTableColumns`     | The columns, as a mixed list with one item per column (borrowed `QObj *`) |
| `qGetTableColumnCount` | The number of columns (`QSize`)                                        |
| `qGetTableRowCount`    | The number of rows (`QSize`); `0` for a table with no columns          |

These accept a table (type `98`). Use `qGetMixedAtIndex` on the result of `qGetTableColumns` to access a single column.

### Keyed Tables

| Function               | Returns                                                   |
| ---------------------- | --------------------------------------------------------- |
| `qGetKeyedTableKeys`   | The table of key columns (borrowed `QObj *`)              |
| `qGetKeyedTableValues` | The table of value columns (borrowed `QObj *`)            |

A keyed table is a dictionary (type `99`) whose keys and values are both tables. The results can be accessed with the [table accessors](#tables).

## Generic Accessors

### `qGetCount`

```c
QSize qGetCount(const QObj *obj);
```

Returns the number of items in any Q object, equivalent to Q's `count`:

| Object                                                   | Result                        |
| -------------------------------------------------------- | ----------------------------- |
| `NULL`                                                   | `0`                           |
| Atom or function                                         | `1`                           |
| List (mixed, simple, enumerated, or mapped/nested)       | Number of items               |
| Table                                                    | Number of rows                |
| Dictionary                                               | Number of key-value pairs     |
| Keyed table                                              | Number of rows                |

Unlike the other accessors, `qGetCount` accepts an object of any type.

## Example

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

static void printTable(const QObj *table) {
    QObj *header = qGetTableHeader(table);
    QObj *columns = qGetTableColumns(table);

    for (QSize col = 0; col < qGetTableColumnCount(table); col++) {
        QObj *column = qGetMixedAtIndex(columns, col);
        printf("%s:", qGetSymbolAtIndex(header, col));

        for (QSize row = 0; row < qGetTableRowCount(table); row++) {
            if (qIsLongList(column))
                printf(" %" PRId64, qGetLongAtIndex(column, row));
            else if (qIsCharList(column))
                printf(" %c", qGetCharAtIndex(column, row));
        }
        printf("\n");
    }
}

int main() {
    QSymbol names[] = {"id", "code"};
    QLong ids[] = {1, 2, 3};
    QChar codes[] = {'a', 'b', 'c'};

    QObj *table = qNewTable(qNewSymbolList(names, 2), qNewLongList(ids, 3), qNewCharList(codes, 3));
    printf("count = %" PRIu64 "\n", qGetCount(table));
    printTable(table);

    decRef(table);
    return 0;
}
```

Output:

```
count = 3
id: 1 2 3
code: a b c
```
