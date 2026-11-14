# Formatting

This document describes the formatting functions from [`fmt.h`](/src/c/cdk/fmt.h). These functions write Q values into a caller-provided character buffer as plain text or Q-style literal text for display. They are formatting functions, not a serialization API.

## Table of Contents

- [Formatting](#formatting)
  - [Table of Contents](#table-of-contents)
  - [Conventions](#conventions)
    - [String and Literal Forms](#string-and-literal-forms)
    - [Expanded temporal fields](#expanded-temporal-fields)
    - [Nulls and infinities](#nulls-and-infinities)
    - [Return Values](#return-values)
    - [Buffer Size](#buffer-size)
    - [Pointer Requirements](#pointer-requirements)
  - [Atoms](#atoms)
  - [Lists](#lists)
    - [Empty and singleton lists](#empty-and-singleton-lists)
  - [Generic Functions](#generic-functions)
  - [Limitations](#limitations)
    - [Real and float display](#real-and-float-display)
    - [Symbol text](#symbol-text)
    - [Console-width truncation](#console-width-truncation)
    - [Temporal rounding](#temporal-rounding)
  - [Examples](#examples)

## Conventions

### String and Literal Forms

The library produces a single printable string, matching Q's own display output in
most cases. For supported primitive atoms and lists, the intended formatting behaviour is:

- `q<Type>ToStr` follows Q's `string`.
- `q<Type>ToLiteral` follows Q's `.Q.s1`.

These rules also apply to generic atom/list dispatch, with the deliberate choices
described below for joined list strings and expanded temporal fields. Other current
differences are recorded in [Limitations](#limitations).

For example, the int `42` is `42` as a string and `42i` as a literal, while the long
`42` is `42` in both forms.

Q's `string` applied to a list returns a list of strings. Our list `ToStr` functions
deliberately join those element strings with a single space to produce one printable
string, corresponding to `" " sv string values` subject to the element-formatting
rules below. This is the intended API contract, not a compatibility defect or a
planned change to return separate strings. Empty element strings retain their
separators, so null elements can produce leading, trailing, or repeated spaces.
`ToLiteral` targets `.Q.s1` of the whole list, including its list-level syntax, rather
than joining the literal forms of individual atoms.

Dictionaries and tables are not currently supported. They will have special handling
later; the atom/list rules above do not define their future formatting behavior.

These are display functions, not a serialization API. Even `.Q.s1` output does not
always provide a parseable expression or preserve the original value: symbols can
contain unquoted spaces or punctuation, and floating-point display precision can
lose information. The `Literal` name therefore does not promise a round trip through
Q's parser. Applications needing serialization require a separate encoder with
explicit round-trip rules.

### Expanded temporal fields

Temporal fields deliberately expand to show their full numeric values rather than
hiding information behind Q's fixed-width placeholders. This applies to both
`ToStr` and `ToLiteral`, for atoms and list elements. It is an intentional difference
from Q, not a compatibility issue to fix:


| Value                             | Current string output | Q `string` output |
| --------------------------------- | --------------------- | ----------------- |
| minute count 6000                 | `100:00`              | `**:00`           |
| second count 360000               | `100:00:00`           | `**:00:00`        |
| time count 360000000 milliseconds | `100:00:00.000`       | `**:00:00.000`    |
| month representing January 10000  | `10000.01`            | `**00.01`         |
| date representing January 1 10000 | `10000.01.01`         | `0000.00.00`      |

Out-of-range datetime dates similarly use `0000.00.00` in Q. Extreme month values
also produced unusual punctuation in this build, while our formatter produces an
expanded year and month. The library will retain its informative output rather than
reproduce those placeholders or build-specific artifacts. Existing supported calendar
ranges and conversion errors still apply: expansion does not make every underlying
value convertible. Nulls and infinities continue to use their Q special-value tokens.
This decision concerns display width and ranges, not datetime sub-millisecond rounding.


### Nulls and infinities

Numeric and temporal nulls and infinities follow Q's `string` and `.Q.s1`.
Special values are detected before calendar conversion or duration arithmetic.
Integer-backed types reserve their minimum value for null, minimum plus one for
negative infinity, and maximum for positive infinity. Any floating-point NaN is
formatted as null, regardless of its sign or payload.

The following atom outputs are implemented. `""` in the string column means zero
output characters, not two quote characters. It returns zero and writes a final
null terminator, requiring a non-null buffer with capacity of at least one byte.
Real/float precision is still validated for special values, but valid precision
does not change their text.

| Type      | `string` null | `string` positive / negative infinity | `.Q.s1` null | `.Q.s1` positive / negative infinity |
| --------- | ------------- | ------------------------------------- | ------------ | ------------------------------------ |
| short     | `""`          | `0W` / `-0W`                          | `0Nh`        | `0Wh` / `-0Wh`                       |
| int       | `""`          | `0W` / `-0W`                          | `0Ni`        | `0Wi` / `-0Wi`                       |
| long      | `""`          | `0W` / `-0W`                          | `0N`         | `0W` / `-0W`                         |
| real      | `""`          | `0w` / `-0w`                          | `0Ne`        | `0we` / `-0we`                       |
| float     | `""`          | `0w` / `-0w`                          | `0n`         | `0w` / `-0w`                         |
| timestamp | `""`          | `0W` / `-0W`                          | `0Np`        | `0Wp` / `-0Wp`                       |
| month     | `""`          | `0W` / `-0W`                          | `0Nm`        | `0Wm` / `-0Wm`                       |
| date      | `""`          | `0W` / `-0W`                          | `0Nd`        | `0Wd` / `-0Wd`                       |
| datetime  | `""`          | `0w` / `-0w`                          | `0Nz`        | `0wz` / `-0wz`                       |
| timespan  | `""`          | `0W` / `-0W`                          | `0Nn`        | `0Wn` / `-0Wn`                       |
| minute    | `""`          | `0W` / `-0W`                          | `0Nu`        | `0Wu` / `-0Wu`                       |
| second    | `""`          | `0W` / `-0W`                          | `0Nv`        | `0Wv` / `-0Wv`                       |
| time      | `""`          | `0W` / `-0W`                          | `0Nt`        | `0Wt` / `-0Wt`                       |

Not every null becomes empty text under `string`. A null GUID prints the full
all-zero GUID in both modes; a null char prints a space, quoted under `.Q.s1`.
A null symbol prints empty text under `string` and a single backtick under `.Q.s1`.
Boolean and byte have no distinct null/infinity values.
A null input pointer remains an argument error, not a Q null value.

Literal lists use Q's list-specific special-value rules: untyped tokens within the
list and a type marker where necessary. The following outputs are implemented:

| List contents                | Q `.Q.s1` output |
| ---------------------------- | ---------------- |
| int null, 42                 | `0N 42i`         |
| date null, January 1 2000    | `0N 2000.01.01`  |
| January 1 2000, date null    | `2000.01.01 0N`  |
| date null, positive infinity | `0N 0Wd`         |

For list string output, joining Q's element strings retains blank fields for nulls.
For example, an int null followed by 42 produces `" 42"`, including the leading
separator. An all-null list of two items produces one space. Literal list shape is
preserved as described under [Empty and singleton lists](#empty-and-singleton-lists).

### Return Values

On success, a function returns the number of bytes written, excluding the final null
terminator. Atom functions, nonempty list functions, and all literal list functions
write that terminator on success. An empty list formatted with `ToStr` returns `0`
and leaves the buffer unchanged; an empty literal list writes its Q representation
(see [Lists](#lists)). Formatting an empty symbol or a numeric/temporal null as a
string also returns `0`, but writes
`buffer[0] = '\0'`. A null GUID pointer is an argument error, not a successful no-op.

Character data can contain embedded null bytes. Those bytes count toward the return
value, so `strlen(buffer)` need not equal it. For example, formatting the character
`'\0'` as a string returns `1` and writes the character followed by the terminator.

Successful output counts are limited to `INT_MAX` characters (excluding the terminator),
even though `QSize` can represent larger buffers. List functions check accumulated
lengths, including prefixes, separators, and suffixes, and return `Q_FMT_ERROR_RANGE`
if the count would exceed that limit. Partial output may already have been written.

On failure, it returns one of the negative `QFmtError` values:

| Constant               | Value | Cause                                                                                             |
| ---------------------- | ----: | ------------------------------------------------------------------------------------------------- |
| `Q_FMT_ERROR_PRINTF`   |  `-1` | The underlying `printf`-family function failed                                                    |
| `Q_FMT_ERROR_SIZE`     |  `-2` | `buffer` is `NULL`, or `size` is too small for the whole result                                   |
| `Q_FMT_ERROR_TYPE`     |  `-3` | The type code is not supported (generic functions only)                                           |
| `Q_FMT_ERROR_RANGE`    |  `-4` | Value or accumulated list output length exceeds the supported range, or calendar conversion fails |
| `Q_FMT_ERROR_ARGUMENT` |  `-5` | A required input pointer is null                                                                  |

Date and timestamp formatting uses UTC. The supported range depends on the platform's
`time_t` and `gmtime_r`; an atom conversion failure returns `Q_FMT_ERROR_RANGE` without
writing to the buffer. List functions propagate this error but may already have written earlier items.

Datetime formatting uses the proleptic Gregorian calendar, independently of `time_t`,
with years from `INT32_MIN` through `INT32_MAX` (inclusive). Finite values
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
atoms and literal lists, and for nonempty string lists. The empty string-list no-op
is the exception, not a sizing query. When the size is hard to predict, retry with a larger buffer after
`Q_FMT_ERROR_SIZE`.

The following widths exclude the final terminator. **Add one byte** when allocating
an atom buffer. These upper bounds cover finite values and special-value text
where conversion succeeds; date/timestamp platform limits and datetime's
supported year range still apply.

| Type         | Width rule                                                                       | Maximum string / literal width |
| ------------ | -------------------------------------------------------------------------------- | ------------------------------ |
| boolean      | Fixed for valid values `0` and `1`                                               | 1 / 2                          |
| guid         | Fixed                                                                            | 36 / 36                        |
| byte         | Fixed                                                                            | 2 / 4                          |
| short        | Decimal digits, optional sign; literal adds `h`                                  | 6 / 7                          |
| int          | Decimal digits, optional sign; literal adds `i`                                  | 11 / 12                        |
| long         | Decimal digits and optional sign; no literal suffix                              | 20 / 20                        |
| char         | One raw byte; literal adds quotes and escapes                                            | 1 / 6                          |
| month        | 7 / 8 with a four-digit nonnegative year; larger or negative years may need more | 13 / 14                        |
| date         | 10 with a four-digit nonnegative year; larger or negative years may need more    | 14 / 14                        |
| datetime     | 23 with a four-digit nonnegative year; larger or negative years may need more    | 30 / 30                        |
| timestamp    | Fixed when conversion succeeds                                                   | 29 / 29                        |
| timespan     | Variable day count and optional sign                                             | 26 / 26                        |
| minute       | At least 5; sign and hours above 99 increase the width                           | 12 / 12                        |
| second       | At least 8; sign and hours above 99 increase the width                           | 13 / 13                        |
| time         | At least 12; sign and hours above 99 increase the width                          | 14 / 14                        |
| symbol       | String byte length; literal adds a backtick                                      | Input-dependent                |
| real / float | Depends on value, precision, and numeric locale; real literal adds `e`           | Input-dependent                |

Month, date, and datetime formatting measures the actual output size rather than
assuming a four-digit year. Minute, second, time, and timespan widths also vary with
the value. For example, `100:00` needs seven bytes including the terminator, and
`-100:00` needs eight.

For a nonempty list's string form, add the actual string widths of the items,
`length - 1` separating spaces, and one final terminator. Literal forms have their
own rules: a byte list of at least two items needs `2 + 2 * length + 1` bytes (`0x`, hexadecimal digits,
terminator). A character-list literal needs the sum of the escaped byte widths
(1, 2, or 4 per input byte), plus two quotes and a terminator: at most
`4 * length + 3` bytes, plus one byte for the leading comma when `length == 1`.
All singleton literals need this extra comma (a one-byte list needs 6 bytes including
its terminator). Do not generally sum atom-literal widths:
list suffixes may appear only once. Check sizing arithmetic for overflow and respect
the `INT_MAX` output-count limit. Empty string lists need no output storage; empty
literal lists need enough space for their Q representation and its terminator.

### Pointer Requirements

Required input pointers must not be null. A null GUID, symbol string, generic atom,
or nonempty list pointer returns `Q_FMT_ERROR_ARGUMENT`. A null string inside a
symbol list also returns this error when reached; earlier items may already have
been written. An empty symbol (`""`) is valid and distinct from a null pointer.

An empty list accepts a null input pointer. Its `ToStr` form returns `0` and writes
nothing, even with a null output pointer. Its `ToLiteral` form requires a non-null
output buffer with space for the literal and terminator. For otherwise valid inputs,
a null output buffer returns `Q_FMT_ERROR_SIZE` except for empty string lists.
Generic functions check the type first: unsupported type
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

The real and float functions take an explicit Q-style display `precision`:

```c
int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision);
```

| Precision                                | Meaning                                                        |
| ---------------------------------------- | -------------------------------------------------------------- |
| `Q_FMT_DEFAULT_PRECISION` (`7`)          | Q's default display precision: 7 significant digits            |
| `1` through `Q_FMT_MAX_PRECISION` (`17`) | Requested significant digits                                   |
| `0`                                      | Q's maximum display precision: 8 digits for real, 17 for float |

Precision zero is type-specific: it is not equivalent to an explicit precision of
17 for reals. For example, the real `1.23456789e` displays as `1.2345679` with precision
zero, but as `1.2345678806304932` with precision 17. More displayed digits expose the
stored binary approximation; they do not add accuracy to the value.

The formatter suppresses trailing zeros and the unnecessary decimal point. It uses
scientific notation when the rounded decimal exponent is below -4 or at least the
effective precision. For example, with precision 7:

| Value        | String output  |
| ------------ | -------------- |
| `1.5`        | `1.5`          |
| `1.23456789` | `1.234568`     |
| `12345678.9` | `1.234568e+07` |
| `1e-20`      | `1e-20`        |
| `1.25`       | `1.25`         |

Values outside `0..17` return `Q_FMT_ERROR_RANGE` before formatting. This also applies
to nulls and infinities, whose text is otherwise independent of valid precision.
Empty lists ignore precision: string lists remain no-ops, while literal lists write
their typed empty representation. Generic functions ignore precision for other types.
The default constant is a convenience for callers, not an implicit
fallback for invalid arguments.

**Compatibility change:** precision previously meant fixed fractional digits, with
values up to 1074. It now means Q display precision. In particular, precision zero
no longer rounds to an integer, and precision 2 formats `1.25` as `1.2`, not `1.25`.
There is currently no separate fixed-decimal formatting mode.

Real (32-bit) subnormal values display as signed zero, matching the installed Q
runtime. Float (64-bit) subnormals retain their numeric text. This affects only
formatting; neither the input value nor the floating-point environment is changed.

These functions do not query a Q runtime or require a Q connection. A Q-facing caller
can obtain `system "P"` once per formatting operation and pass that setting to the
atom/list formatter. Standalone C callers can use `Q_FMT_DEFAULT_PRECISION` or their
own setting. Querying a new Q process would only read that process's setting, not
another session's current precision.

Formatting uses `printf` general-format rounding and the current numeric locale.
Examples here assume the C numeric locale and the default round-to-nearest mode.
Real literals append `e`. Float literals append `f` only when the displayed text
would otherwise look like an integer. This decision follows rounding: `1.25` with
precision 1 becomes `1f`, whereas precision 2 produces `1.2`. Signed zero becomes
`0f` or `-0f`; scientific notation and special values need no extra marker.

Float literal lists append one `f` at the end only when every displayed item looks
like an integer. A decimal, exponent, null, or infinity token anywhere in the list
already identifies its type, matching `.Q.s1`:

| Values | Precision | Literal output |
| ------ | --------- | -------------- |
| `1 2f` | 7 | `1 2f` |
| `1.25 2.25` | 1 | `1 2f` |
| `1 1.5` | 7 | `1 1.5` |
| `1 1e7` | 7 | `1 1e+07` |
| `1 0n` | 7 | `1 0n` |
| `0w 1` | 7 | `0w 1` |

Character literals follow Q's `.Q.s1` escaping, for both atoms and lists. Printable
ASCII bytes (32 through 126) are emitted directly except for quotes and backslashes.
Tab, newline, and carriage return use `\t`, `\n`, and `\r`; a quote uses `\"` and a
backslash uses `\\`. All other bytes use exactly three octal digits: for example,
a zero byte is `\000`, backspace is `\010`, and byte 255 is `\377`. Escaping is
byte-based and independent of locale, including for UTF-8 input.

The escapes are enclosed in double quotes. For example, a newline atom produces
`"\n"`, and the three bytes `a`, zero, `b` produce `"a\000b"`. Character `ToStr`
functions continue to emit raw bytes, including embedded zero bytes; list strings
retain their separating spaces. A singleton literal includes a leading comma before
the opening quote; an empty character-list literal is `""`.

The guid functions take a `const QGuid *`.

| Type      | Value                             | String                                 | Literal        |
| --------- | --------------------------------- | -------------------------------------- | -------------- |
| boolean   | `1`                               | `1`                                    | `1b`           |
| guid      | bytes `12 34 56 78 9a bc ...`     | `12345678-9abc-def0-1234-56789abcdef0` | same as string |
| byte      | `0xab`                            | `ab`                                   | `0xab`         |
| short     | `42`                              | `42`                                   | `42h`          |
| int       | `42`                              | `42`                                   | `42i`          |
| long      | `42`                              | `42`                                   | `42`           |
| real      | `1.5` (precision 2)               | `1.5`                                  | `1.5e`         |
| float     | `1.5` (precision 3)               | `1.5`                                  | `1.5`          |
| char      | `'a'`                             | `a`                                    | `"a"`          |
| symbol    | `"abc"`                           | `abc`                                  | `` `abc ``     |
| timestamp | 1.5 days in nanoseconds           | `2000.01.02D12:00:00.000000000`        | same as string |
| month     | `13`                              | `2001.02`                              | `2001.02m`     |
| date      | `31`                              | `2000.02.01`                           | same as string |
| datetime  | `1.5`                             | `2000.01.02T12:00:00.000`              | same as string |
| timespan  | 1 day, 1 second and 5 nanoseconds | `1D00:00:01.000000005`                 | same as string |
| minute    | `90`                              | `01:30`                                | same as string |
| second    | `3661`                            | `01:01:01`                             | same as string |
| time      | `3661001`                         | `01:01:01.001`                         | same as string |

## Lists

Each list function takes a pointer to the first item and the number of items:

```c
int qLongListToStr(char *buffer, QSize size, const QLong *longs, QSize length);
int qLongListToLiteral(char *buffer, QSize size, const QLong *longs, QSize length);
```

As with atoms, the real and float list functions take an additional `precision`. The items of a Q list can be passed directly, e.g. `qLongListToStr(buffer, size, (const QLong *)obj->list, obj->length)`, or `qGuidListToStr(buffer, size, (const QGuid *)obj->list, obj->length)` for a guid list.

Items are separated by a single space in the string form. The literal form uses type-specific Q-style display conventions, without guaranteeing a parseable list expression:

| Type    | Items                    | String                  | Literal        |
| ------- | ------------------------ | ----------------------- | -------------- |
| long    | `1 2 3`                  | `1 2 3`                 | `1 2 3`        |
| boolean | `1 0 1`                  | `1 0 1`                 | `101b`         |
| byte    | `0x01 0xff`              | `01 ff`                 | `0x01ff`       |
| int     | `1 -2 3`                 | `1 -2 3`                | `1 -2 3i`      |
| real    | `1.5 2.25` (precision 3) | `1.5 2.25`              | `1.5 2.25e`    |
| char    | `'a' 'b'`                | `a b`                   | `"ab"`         |
| symbol  | `"a" "b"`                | `a b`                   | `` `a`b ``     |
| date    | `0 1`                    | `2000.01.01 2000.01.02` | same as string |

### Empty and singleton lists

`ToLiteral` preserves primitive list shape using Q's `.Q.s1` conventions:

| Value | Literal output |
| ----- | -------------- |
| empty long list | `` `long$() `` |
| empty char list | `""` |
| one long | `,42` |
| one float | `,1f` |
| one char | `,"a"` |
| one byte | `,0xab` |
| one null date | `,0Nd` |

Every singleton literal starts with a comma, before any opening quote, backtick, or
`0x` prefix. Empty character lists use `""`; other empty primitive lists use
`` `type$() ``, where `type` is `boolean`, `guid`, `byte`, `short`, `int`, `long`,
`real`, `float`, `symbol`, `timestamp`, `month`, `date`, `datetime`, `timespan`,
`minute`, `second`, or `time`.

**Compatibility change:** empty literal lists now write output and a terminator.
They accept a null input pointer, but require a non-null output buffer with sufficient
capacity. Otherwise they return `Q_FMT_ERROR_SIZE`. Singleton literals require one
additional byte for the comma. Empty lists ignore real/float precision because there
are no elements to format; unsupported generic type codes still return `Q_FMT_ERROR_TYPE`.

List `ToStr` retains the existing joined-element convention. An empty string list
returns `0` and leaves the buffer unchanged, including when input/output pointers
are null. Initialize the buffer (e.g. `buffer[0] = '\0'`) if an empty string list
should be represented as a C string. A singleton string list has no comma or added
literal syntax; a null element can produce an empty string with a terminator.

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

The following are known differences from Q's `string` and `.Q.s1`, not intended
compatibility guarantees. They were checked against the locally installed KDB-X 5.0
(build 2026.07.23), using its default `\P 7` unless otherwise stated and a wide console
for complete output. Extreme-value results may be specific to that build. No implementation changes are implied by
this comparison.

### Real and float display

Significant-digit precision, trailing-zero suppression, and scientific notation now
follow Q's display rules. Numeric output still follows the current C numeric locale
and floating-point rounding mode; matching the examples assumes the C locale and
default round-to-nearest mode.

### Symbol text

Symbols containing spaces or punctuation are not quoted or escaped by the current
formatters, which matches `.Q.s1` in the tested cases. For example, the symbol
containing `a b` displays as `` `a b ``. This is a limitation of parsing display text
back into Q, not a formatting compatibility defect.

### Console-width truncation

Q's `.Q.s1` can truncate long output according to its console width (`\c`). These
C functions have no console-width setting: they produce the complete text or return
`Q_FMT_ERROR_SIZE` when the buffer is too small. Q comparisons use a sufficiently
wide console to check the complete formatting rather than the truncated display.

### Temporal rounding

Datetime formatting currently rounds to the nearest millisecond. Q discards the
sub-millisecond time within the day in the tested cases:

| Datetime value (days since epoch) | Current output            | Q output in both modes    |
| --------------------------------- | ------------------------- | ------------------------- |
| `0.000000006`                     | `2000.01.01T00:00:00.001` | `2000.01.01T00:00:00.000` |
| `0.999999999`                     | `2000.01.02T00:00:00.000` | `2000.01.01T23:59:59.999` |
| `-0.000000001`                    | `2000.01.01T00:00:00.000` | `1999.12.31T23:59:59.999` |


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

Handling errors and formatting a null as a literal:

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
    if (qLongToLiteral(buffer, sizeof(buffer), value) >= 0)
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
