# Formatting

This document describes the formatting functions from [`fmt.h`](/src/c/cdk/fmt.h). These functions write Q values into a caller-provided character buffer, either as a plain string or as a Q literal.

## Table of Contents

- [Conventions](#conventions)
    - [String and Literal Forms](#string-and-literal-forms)
    - [Return Values](#return-values)
    - [Buffer Size](#buffer-size)
    - [Pointer Requirements](#pointer-requirements)
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

On success, a function returns the number of bytes written, excluding the final null
terminator. Atom functions and nonempty list functions write that terminator on
success. An empty list instead returns `0` and leaves the buffer unchanged (see
[Lists](#lists)). Formatting an empty symbol as a string also returns `0`, but writes
`buffer[0] = '\0'`. A null GUID pointer is an argument error, not a successful no-op.

Character data can contain embedded null bytes. Those bytes count toward the return
value, so `strlen(buffer)` need not equal it. For example, formatting the character
`'\0'` as a string returns `1` and writes the character followed by the terminator.

Successful output counts are limited to `INT_MAX` characters (excluding the terminator),
even though `QSize` can represent larger buffers. List functions check accumulated
lengths, including prefixes, separators, and suffixes, and return `Q_FMT_ERROR_RANGE`
if the count would exceed that limit. Partial output may already have been written.

On failure, it returns one of the negative `QFmtError` values:

| Constant             | Value | Cause                                                          |
| -------------------- | ----: | -------------------------------------------------------------- |
| `Q_FMT_ERROR_PRINTF` |  `-1` | The underlying `printf`-family function failed                 |
| `Q_FMT_ERROR_SIZE`   |  `-2` | `buffer` is `NULL`, or `size` is too small for the whole result |
| `Q_FMT_ERROR_TYPE`   |  `-3` | The type code is not supported (generic functions only)        |
| `Q_FMT_ERROR_RANGE`  |  `-4` | Value or accumulated list output length exceeds the supported range, or calendar conversion fails |
| `Q_FMT_ERROR_ARGUMENT` | `-5` | A required input pointer is null |

Date and timestamp formatting uses UTC. The supported range depends on the platform's
`time_t` and `gmtime_r`; an atom conversion failure returns `Q_FMT_ERROR_RANGE` without
writing to the buffer. List functions propagate this error but may already have written earlier items.

Datetime formatting uses the proleptic Gregorian calendar, independently of `time_t`,
with years from `INT32_MIN` through `INT32_MAX` (inclusive). NaN, infinities, and values
outside that range return `Q_FMT_ERROR_RANGE` without writing output. The fractional
day is rounded to the nearest millisecond, with halfway values rounding toward the
later time; rounding across midnight advances the date. A rounded result outside the
supported year range also returns `Q_FMT_ERROR_RANGE`. Floating-point precision limits
the available time-of-day resolution for dates far from the epoch.

### Buffer Size

`size` is the size of `buffer` in bytes, including space for the terminating null character.

These functions never report truncated output as success: if the whole result does
not fit, they return `Q_FMT_ERROR_SIZE`. A failed call can still have modified the
buffer. In particular, a list may have written earlier items or a separator without
a final null terminator. Do not use the buffer as a string after an error. Individual
functions may document stronger guarantees for particular errors.

There is no sizing-query mode: a null output buffer is an error for otherwise valid
atom or nonempty-list inputs. The empty-list no-op is the exception, not a sizing
query. When the size is hard to predict, retry with a larger buffer after
`Q_FMT_ERROR_SIZE`.

The following widths exclude the final terminator. **Add one byte** when allocating
an atom buffer. Maxima cover the underlying numeric ranges, including raw special
values, where conversion succeeds; date/timestamp platform limits and datetime's
supported year range still apply.

| Type | Width rule | Maximum string / literal width |
| ---- | ---------- | ------------------------------ |
| boolean | Fixed for valid values `0` and `1` | 1 / 2 |
| guid | Fixed | 36 / 36 |
| byte | Fixed | 2 / 4 |
| short | Decimal digits, optional sign; literal adds `h` | 6 / 7 |
| int | Decimal digits, optional sign; literal adds `i` | 11 / 12 |
| long | Decimal digits and optional sign; no literal suffix | 20 / 20 |
| char | One raw byte; literal adds two quotes | 1 / 3 |
| month | 7 / 8 with a four-digit nonnegative year; larger or negative years may need more | 13 / 14 |
| date | 10 with a four-digit nonnegative year; larger or negative years may need more | 14 / 14 |
| datetime | 23 with a four-digit nonnegative year; larger or negative years may need more | 30 / 30 |
| timestamp | Fixed when conversion succeeds | 29 / 29 |
| timespan | Variable day count and optional sign | 26 / 26 |
| minute | At least 5; sign and hours above 99 increase the width | 12 / 12 |
| second | At least 8; sign and hours above 99 increase the width | 13 / 13 |
| time | At least 12; sign and hours above 99 increase the width | 14 / 14 |
| symbol | String byte length; literal adds a backtick | Input-dependent |
| real / float | Depends on value, precision, and numeric locale; real literal adds `e` | Input-dependent |

Month, date, and datetime formatting measures the actual output size rather than
assuming a four-digit year. Minute, second, time, and timespan widths also vary with
the value. For example, `100:00` needs seven bytes including the terminator, and
`-100:00` needs eight.

For a nonempty list's string form, add the actual string widths of the items,
`length - 1` separating spaces, and one final terminator. Literal forms have their
own rules: a byte list needs `2 + 2 * length + 1` bytes (`0x`, hexadecimal digits,
terminator), while an unescaped character-list literal needs `length + 2 + 1`
bytes (characters, quotes, terminator). Do not generally sum atom-literal widths:
list suffixes may appear only once. Check sizing arithmetic for overflow and respect
the `INT_MAX` output-count limit. Empty lists need no output storage because they
write nothing.

### Pointer Requirements

Required input pointers must not be null. A null GUID, symbol string, generic atom,
or nonempty list pointer returns `Q_FMT_ERROR_ARGUMENT`. A null string inside a
symbol list also returns this error when reached; earlier items may already have
been written. An empty symbol (`""`) is valid and distinct from a null pointer.

An empty list returns `0` and writes nothing, even when its input and output pointers
are null. For nonempty output with valid inputs, a null output buffer continues to
return `Q_FMT_ERROR_SIZE`. Generic functions check the type first: unsupported type
codes return `Q_FMT_ERROR_TYPE`, including for null inputs or empty lists.

Callers must provide live, correctly aligned input storage matching the declared
type and list length, null-terminated symbol strings, and writable output storage
of at least `size` bytes. Input and output storage must not overlap. Dangling
pointers, insufficient storage, misalignment, and overlap are caller errors; these
functions cannot reliably detect them.

## Atoms

Each atom function has the form:

```c
int qLongToStr(char *buffer, QSize size, QLong long_);
int qLongToLiteral(char *buffer, QSize size, QLong long_);
```

The real and float functions take an additional `precision`, the number of fractional digits to write for finite values:

```c
int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision);
```

`precision` must be between `0` and `Q_FMT_MAX_PRECISION` (`1074`), inclusive.
Negative values do not request the `printf` default precision: they return
`Q_FMT_ERROR_RANGE`, as do values above the limit, before any output is written.
The same rule applies to nonempty real/float lists and generic dispatch, including
when a value is NaN or infinity. Empty lists remain no-ops regardless of precision;
generic functions ignore precision for other types.

The limit allows the complete fractional decimal expansion of an IEEE binary64
value while bounding formatting work per value. It is an output-formatting limit,
not a claim about numeric accuracy. A buffer must still accommodate the integer
part, decimal point, requested fractional digits, sign, any literal suffix, and
final terminator. NaN and infinity retain the underlying `printf` spelling rather
than receiving fractional digits.

Fractional digits and significant digits are different. `QReal` uses C `float`,
whose decimal precision is `FLT_DIG` (typically 6 significant digits); `QFloat`
uses C `double`, with `DBL_DIG` (typically 15). These describe decimal precision
of the stored value, not a guaranteed number of accurate places after the decimal
point. More output digits may reveal the binary approximation or add trailing zeros;
they do not restore information lost when the value was stored.

Formatting uses `printf` rounding and the current numeric locale. For example,
`1.25` with precision `4` becomes `1.2500`, while precision `0` omits the decimal
point and fractional digits. The real literal still adds `e`; float literals at
precision `0` can lose their Q type (see [Limitations](#limitations)).

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

As with atoms, the real and float list functions take an additional `precision`. The items of a Q list can be passed directly, e.g. `qLongListToStr(buffer, size, (const QLong *)obj->list, obj->length)`, or `qGuidListToStr(buffer, size, (const QGuid *)obj->list, obj->length)` for a guid list.

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
- `atom` points to the atom value (e.g. a `QLong *`), **except for symbols**, where it is the string pointer itself. Pass `symbol`, not `&symbol`.
- `list` points to the first item. A symbol list is an array of string pointers (`QSymbol[]`).
- `precision` is used for real and float values and ignored otherwise.

For example, the generic atom calls use different pointer levels for a long and a symbol:

```c
QLong value = 42;
QSymbol symbol = "abc";
qAtomToStr(buffer, sizeof(buffer), &value, Q_TYPE_LONG, 0);
qAtomToStr(buffer, sizeof(buffer), symbol, Q_TYPE_SYMBOL, 0);
```

Only the primitive types (`Q_TYPE_BOOLEAN` to `Q_TYPE_TIME`) are supported; any other type code (including `Q_TYPE_MIXED`, tables, and dictionaries) returns `Q_FMT_ERROR_TYPE`.

## Limitations

The formatting functions write the raw value of each type. They do not currently handle the following, which should be taken into account when the output must match Q or be parsed back by Q:

- **Nulls and infinities are not recognised.** They are written as ordinary values: a long null as `-9223372036854775808` rather than `0N`, a float null as `nan` rather than `0n`, and a float infinity as `inf` rather than `0w`. Temporal special values are not formatted as Q nulls or infinities: non-finite datetimes return `Q_FMT_ERROR_RANGE`, and other temporal types format their underlying value where supported. Check for [special values](/doc/c/type.md#special-values) before formatting.
- **Timespans use the full signed 64-bit raw value range.** For example, `INT64_MIN` (the timespan null sentinel) is formatted as `-106751D23:47:16.854775808`, not as a Q null.
- **Char literals are not escaped.** A `"` is written as `"""` rather than `"\""`, and backslashes and control characters (such as a newline) are written as they are.
- **Symbols are not quoted.** A symbol containing a space, such as `a b`, is written as `` `a b ``, which Q parses as two tokens.
- **A float literal can lose its type.** With a precision of `0`, the float `1.0` is written as `1`, which Q parses as a long.
- **One-item and empty list literals are not distinguished from atoms.** A one-item long list is written as `1` (Q would write `,1`), a one-item char list as `"a"` (Q: `,"a"`), and an empty list as nothing at all (Q: `` `long$() ``, or `""` for an empty char list).

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

    qDecRef(intAtom);
    qDecRef(symbolList);
    qDecRef(date);
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
