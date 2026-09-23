# Formatting

This document describes the formatting functions from [`fmt.h`](/src/c/cdk/fmt.h). These functions write Q values into a caller-provided character buffer, either as a plain string or as a Q literal.

## Table of Contents

- [Conventions](#conventions)
    - [String and Literal Forms](#string-and-literal-forms)
    - [Return Values](#return-values)
    - [Buffer Size](#buffer-size)
- [Atoms](#atoms)
- [Lists](#lists)
- [Generic Functions](#generic-functions)
- [Limitations](#limitations)
- [Examples](#examples)

## Conventions

### String and Literal Forms

Every type has two functions:

- `q<Type>ToStr` writes the value as a plain string, as it would appear in a table cell (`42`, `abc`, `2000.01.01`).
- `q<Type>ToLiteral` writes the value in a form that Q parses back to the same type where the type needs a marker (`42i`, `` `abc ``, `0xab`, `"a"`).

For example, the int `42` is `42` as a string and `42i` as a literal, while the long `42` is `42` in both forms.

### Return Values

On success, a function returns the number of characters written, not including the terminating null character. The terminating null character is written in every case except an empty list, which writes nothing at all (see [Lists](#lists)).

On failure, it returns one of the negative `QFmtError` values:

| Constant             | Value | Cause                                                          |
| -------------------- | ----: | -------------------------------------------------------------- |
| `Q_FMT_ERROR_PRINTF` |  `-1` | The underlying `printf`-family function failed                 |
| `Q_FMT_ERROR_SIZE`   |  `-2` | `buffer` is `NULL`, or `size` is too small for the whole result |
| `Q_FMT_ERROR_TYPE`   |  `-3` | The type code is not supported (generic functions only)        |

### Buffer Size

`size` is the size of `buffer` in bytes, including space for the terminating null character.

Unlike `snprintf`, the functions do not truncate: if the result does not fit, they return `Q_FMT_ERROR_SIZE`. They also cannot be used to measure the required size (passing a `NULL` buffer is an error). After an error, the contents of the buffer are unspecified; a list function may already have written some of its items, without a terminating null character.

Many types have a fixed width, which makes the required size easy to work out:

| Type      | Width (characters)                     |
| --------- | -------------------------------------- |
| boolean   | 1 (string), 2 (literal)                |
| guid      | 36                                     |
| byte      | 2 (string), 4 (literal)                |
| month     | 7 (string), 8 (literal)                |
| date      | 10                                     |
| minute    | 5                                      |
| second    | 8                                      |
| time      | 12 (13 if negative)                    |
| datetime  | 23                                     |
| timestamp | 29                                     |

A long needs at most 20 characters, and a literal adds at most one suffix character. For lists, allow for each item plus a separating space. When the size is hard to predict (for example, floats or symbols), retry with a larger buffer on `Q_FMT_ERROR_SIZE`.

## Atoms

Each atom function has the form:

```c
int qLongToStr(char *buffer, QSize size, QLong long_);
int qLongToLiteral(char *buffer, QSize size, QLong long_);
```

The real and float functions take an additional `precision`, the number of decimal places written (only the first 6 are accurate for a real):

```c
int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision);
```

The guid functions take a `const QGuid *`.

| Type      | Value                                  | String                          | Literal                         |
| --------- | -------------------------------------- | ------------------------------- | ------------------------------- |
| boolean   | `1`                                    | `1`                             | `1b`                            |
| guid      | bytes `12 34 56 78 9a bc ...`          | `12345678-9abc-def0-1234-56789abcdef0` | same as string           |
| byte      | `0xab`                                 | `ab`                            | `0xab`                          |
| short     | `42`                                   | `42`                            | `42h`                           |
| int       | `42`                                   | `42`                            | `42i`                           |
| long      | `42`                                   | `42`                            | `42`                            |
| real      | `1.5` (precision 2)                    | `1.50`                          | `1.50e`                         |
| float     | `1.5` (precision 3)                    | `1.500`                         | `1.500`                         |
| char      | `'a'`                                  | `a`                             | `"a"`                           |
| symbol    | `"abc"`                                | `abc`                           | `` `abc ``                      |
| timestamp | 1.5 days in nanoseconds                | `2000.01.02D12:00:00.000000000` | same as string                  |
| month     | `13`                                   | `2001.02`                       | `2001.02m`                      |
| date      | `31`                                   | `2000.02.01`                    | same as string                  |
| datetime  | `1.5`                                  | `2000.01.02T12:00:00.000`       | same as string                  |
| timespan  | 1 day, 1 second and 5 nanoseconds      | `1D00:00:01.000000005`          | same as string                  |
| minute    | `90`                                   | `01:30`                         | same as string                  |
| second    | `3661`                                 | `01:01:01`                      | same as string                  |
| time      | `3661001`                              | `01:01:01.001`                  | same as string                  |

## Lists

Each list function takes a pointer to the first item and the number of items:

```c
int qLongListToStr(char *buffer, QSize size, const QLong *longs, QSize length);
int qLongListToLiteral(char *buffer, QSize size, const QLong *longs, QSize length);
```

As with atoms, the real and float list functions take an additional `precision`. The items of a Q list can be passed directly, e.g. `qLongListToStr(buffer, size, (const QLong *)obj->list, obj->length)`.

> Note: The guid list functions are the exception. They take an array of *pointers* to guids (`QGuid *const *`), not the guids themselves, so the items of a Q guid list cannot be passed to them directly. See [Limitations](#limitations).

Items are separated by a single space in the string form. The literal form follows Q's syntax for each type:

| Type    | Items                | String                    | Literal                   |
| ------- | -------------------- | ------------------------- | ------------------------- |
| long    | `1 2 3`              | `1 2 3`                   | `1 2 3`                   |
| boolean | `1 0 1`              | `1 0 1`                   | `101b`                    |
| byte    | `0x01 0xff`          | `01 ff`                   | `0x01ff`                  |
| int     | `1 -2 3`             | `1 -2 3`                  | `1 -2 3i`                 |
| real    | `1.5 2.25` (precision 2) | `1.50 2.25`           | `1.50 2.25e`              |
| char    | `'a' 'b'`            | `a b`                     | `"ab"`                    |
| symbol  | `"a" "b"`            | `a b`                     | `` `a`b ``                |
| date    | `0 1`                | `2000.01.01 2000.01.02`   | same as string            |

An empty list returns `0` and leaves the buffer unchanged: not even a terminating null character is written. Initialise the buffer (e.g. `buffer[0] = '\0'`) if the list may be empty.

## Generic Functions

These functions take a type code and dispatch to the function for that type:

```c
int qAtomToStr(char *buffer, QSize size, const void *atom, QTypeCode type, int precision);
int qAtomToLiteral(char *buffer, QSize size, const void *atom, QTypeCode type, int precision);
int qListToStr(char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision);
int qListToLiteral(char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision);
```

- `type` is the positive type code of the value (e.g. `Q_TYPE_LONG` for both a long atom and a long list).
- `atom` points to the atom value (e.g. a `QLong *`), and `list` to the first item of the list.
- `precision` is used for real and float values and ignored otherwise.

Only the primitive types (`Q_TYPE_BOOLEAN` to `Q_TYPE_TIME`) are supported; any other type code (including `Q_TYPE_MIXED`, tables, and dictionaries) returns `Q_FMT_ERROR_TYPE`.

## Limitations

The formatting functions write the raw value of each type. They do not currently handle the following, which should be taken into account when the output must match Q or be parsed back by Q:

- **Nulls and infinities are not recognised.** They are written as ordinary values: a long null as `-9223372036854775808` rather than `0N`, a float null as `nan` rather than `0n`, and a float infinity as `inf` rather than `0w`. A temporal null is written as an incorrect value; for example, a date null is written as `2000.01.01`. Check for [special values](/doc/c/type.md#special-values) before formatting.
- **Char literals are not escaped.** A `"` is written as `"""` rather than `"\""`, and backslashes and control characters (such as a newline) are written as they are.
- **Symbols are not quoted.** A symbol containing a space, such as `a b`, is written as `` `a b ``, which Q parses as two tokens.
- **A float literal can lose its type.** With a precision of `0`, the float `1.0` is written as `1`, which Q parses as a long.
- **One-item and empty list literals are not distinguished from atoms.** A one-item long list is written as `1` (Q would write `,1`), a one-item char list as `"a"` (Q: `,"a"`), and an empty list as nothing at all (Q: `` `long$() ``, or `""` for an empty char list).
- **Guid lists use a different layout.** `qGuidListToStr`, `qGuidListToLiteral`, and `qListToStr`/`qListToLiteral` with `Q_TYPE_GUID` expect an array of pointers to guids. Passing the items of a Q guid list (`obj->list`) to them is undefined behaviour and typically crashes. Format the guids of a Q list individually with `qGuidToStr` instead, or build an array of pointers to them.

## Examples

Formatting atoms and lists read from Q objects:

```c
#include <stdio.h>
#include "q.h"

int main() {
    char buffer[64];

    QObj *intAtom = qNewInt(42);
    qIntToStr(buffer, sizeof(buffer), qGetInt(intAtom));
    printf("int string: %s\n", buffer);
    qIntToLiteral(buffer, sizeof(buffer), qGetInt(intAtom));
    printf("int literal: %s\n", buffer);

    QSymbol symbols[] = {"abc", "xyz"};
    QObj *symbolList = qNewSymbolList(symbols, 2);
    qListToStr(buffer, sizeof(buffer), symbolList->list, symbolList->length, Q_TYPE_SYMBOL, 0);
    printf("symbol list string: %s\n", buffer);
    qListToLiteral(buffer, sizeof(buffer), symbolList->list, symbolList->length, Q_TYPE_SYMBOL, 0);
    printf("symbol list literal: %s\n", buffer);

    QObj *date = qNewDate(31);
    qDateToStr(buffer, sizeof(buffer), qGetDate(date));
    printf("date: %s\n", buffer);

    decRef(intAtom);
    decRef(symbolList);
    decRef(date);
    return 0;
}
```

Output:

```
int string: 42
int literal: 42i
symbol list string: abc xyz
symbol list literal: `abc`xyz
date: 2000.02.01
```

Handling errors, and checking for a null before formatting:

```c
#include <stdio.h>
#include "q.h"

int main() {
    char small[4];
    int result = qDateToStr(small, sizeof(small), 0);
    if (result == Q_FMT_ERROR_SIZE)
        printf("buffer too small\n");

    char buffer[32];
    QLong value = Q_LONG_NULL;
    if (value == Q_LONG_NULL)
        printf("value: 0N\n");
    else if (qLongToStr(buffer, sizeof(buffer), value) >= 0)
        printf("value: %s\n", buffer);

    QLong atom = 7;
    if (qAtomToStr(buffer, sizeof(buffer), &atom, Q_TYPE_TABLE, 0) == Q_FMT_ERROR_TYPE)
        printf("tables are not supported\n");

    return 0;
}
```

Output:

```
buffer too small
value: 0N
tables are not supported
```
