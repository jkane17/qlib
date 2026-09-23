# Q Types

The types defined in [`type.h`](/src/c/cdk/type.h) provide the C representations of Q's primitive data types, type codes, attributes, and special numeric values.

## Primitive Types

| C type       | Q type    | Description                                                      |
| ------------ | --------- | ---------------------------------------------------------------- |
| `QBoolean`   | boolean   | Boolean value                                                    |
| `QGuid`      | guid      | 128-bit globally unique identifier                               |
| `QByte`      | byte      | Unsigned 8-bit integer                                           |
| `QShort`     | short     | Signed 16-bit integer                                            |
| `QInt`       | int       | Signed 32-bit integer                                            |
| `QLong`      | long      | Signed 64-bit integer                                            |
| `QReal`      | real      | 32-bit floating-point value                                      |
| `QFloat`     | float     | 64-bit floating-point value                                      |
| `QChar`      | char      | Character                                                        |
| `QSymbol`    | symbol    | Symbol (interned string)                                         |
| `QTimestamp` | timestamp | Nanoseconds since 2000.01.01D00:00:00                            |
| `QMonth`     | month     | Months since Jan 2000                                            |
| `QDate`      | date      | Days since 1st Jan 2000                                          |
| `QDatetime`  | datetime  | Fractional days since 2000.01.01 (deprecated)                    |
| `QTimespan`  | timespan  | Nanosecond duration                                              |
| `QMinute`    | minute    | Minutes since midnight                                           |
| `QSecond`    | second    | Seconds since midnight                                           |
| `QTime`      | time      | Milliseconds since midnight                                      |

## Type Definitions

### `QBoolean`

```c
typedef unsigned char QBoolean;
```

Represents a Q boolean value.

A `QBoolean` occupies one byte.

### `QGuid`

```c
typedef struct {
    unsigned char bytes[16];
} QGuid;
```

Represents a Q GUID.

A `QGuid` contains 16 bytes and therefore occupies 128 bits.

### `QByte`

```c
typedef unsigned char QByte;
```

Represents a Q byte value.

A `QByte` occupies one byte.

### `QShort`

```c
typedef int16_t QShort;
```

Represents a Q short value.

A `QShort` is a signed 16-bit integer.

### `QInt`

```c
typedef int32_t QInt;
```

Represents a Q int value.

A `QInt` is a signed 32-bit integer.

### `QLong`

```c
typedef int64_t QLong;
```

Represents a Q long value.

A `QLong` is a signed 64-bit integer.

### `QReal`

```c
typedef float QReal;
```

Represents a Q real value.

A `QReal` is a 32-bit IEEE 754 floating-point value.

### `QFloat`

```c
typedef double QFloat;
```

Represents a Q float value.

A `QFloat` is a 64-bit IEEE 754 floating-point value.

### `QChar`

```c
typedef char QChar;
```

Represents a Q char value.

A `QChar` occupies one byte.

### `QSymbol`

```c
typedef char *QSymbol;
```

Represents a Q symbol.

`QSymbol` is a pointer to the symbol's character data.

### `QTimestamp`

```c
typedef int64_t QTimestamp;
```

Represents a Q timestamp.

A `QTimestamp` is a signed 64-bit integer.

### `QMonth`

```c
typedef int32_t QMonth;
```

Represents a Q month.

A `QMonth` is a signed 32-bit integer.

### `QDate`

```c
typedef int32_t QDate;
```

Represents a Q date.

A `QDate` is a signed 32-bit integer.

### `QDatetime`

```c
typedef double QDatetime;
```

Represents a Q datetime.

A `QDatetime` is a 64-bit floating-point value.

### `QTimespan`

```c
typedef int64_t QTimespan;
```

Represents a Q timespan.

A `QTimespan` is a signed 64-bit integer.

### `QMinute`

```c
typedef int32_t QMinute;
```

Represents a Q minute.

A `QMinute` is a signed 32-bit integer.

### `QSecond`

```c
typedef int32_t QSecond;
```

Represents a Q second.

A `QSecond` is a signed 32-bit integer.

### `QTime`

```c
typedef int32_t QTime;
```

Represents a Q time.

A `QTime` is a signed 32-bit integer.

## Type Metadata

### `QType`

```c
typedef signed char QType;
```

Represents a Q type code.

Q uses the sign of the type code to distinguish atoms from lists:

- Negative values represent atoms.
- Zero and positive values represent lists.

For example:

```text
-7  long atom
 7  long list

-9  float atom
 9  float list
```

### `QAttr`

```c
typedef char QAttr;
```

Represents attributes associated with a Q list.

The attribute is one of the `QAttrCode` values:

```c
typedef enum : QAttr {
    Q_ATTR_NONE = 0,
    Q_ATTR_SORTED = 1,
    Q_ATTR_UNIQUE = 2,
    Q_ATTR_PARTED = 3,
    Q_ATTR_GROUPED = 4,
} QAttrCode;
```

| Constant         | Value | Q attribute     |
| ---------------- | ----: | --------------- |
| `Q_ATTR_NONE`    |   `0` | none            |
| `Q_ATTR_SORTED`  |   `1` | sorted (`` `s# ``)  |
| `Q_ATTR_UNIQUE`  |   `2` | unique (`` `u# ``)  |
| `Q_ATTR_PARTED`  |   `3` | parted (`` `p# ``)  |
| `Q_ATTR_GROUPED` |   `4` | grouped (`` `g# ``) |

### `QSize`

```c
typedef uint64_t QSize;
```

Represents the size or length of a Q object.

`QSize` is an unsigned 64-bit integer.

kdb+ itself stores lengths as signed 64-bit integers. `QSize` is unsigned so that a negative length cannot be expressed, but its valid range is limited to that of the kdb+ representation:

```c
#define Q_SIZE_MAX ((QSize)INT64_MAX)
```

Functions that accept a `QSize` (such as `qNewList`) return a `domain` error if the value exceeds `Q_SIZE_MAX`. Lengths read from a `QObj` are never negative, so they always fit in a `QSize`.

Use `PRIu64` (from `<inttypes.h>`) to print a `QSize`:

```c
printf("Length = %" PRIu64 "\n", obj->length);
```

As with `size_t`, take care with arithmetic that could go below zero. For example, a reverse loop written as `for (QSize i = n - 1; i >= 0; i--)` never terminates (and underflows when `n` is `0`). Instead use:

```c
for (QSize i = n; i-- > 0;) {
    // ...
}
```

## Special Values

Q defines special null and infinity values for many of its types. These are exposed as macros (and, for `QGuid`, a constant).

### Integer Types

| Macro          | Definition            | Q value |
| -------------- | --------------------- | ------- |
| `Q_SHORT_NULL` | `(QShort)INT16_MIN`   | `0Nh`   |
| `Q_SHORT_INF`  | `(QShort)INT16_MAX`   | `0Wh`   |
| `Q_SHORT_NINF` | `(QShort)-INT16_MAX`  | `-0Wh`  |
| `Q_INT_NULL`   | `(QInt)INT32_MIN`     | `0Ni`   |
| `Q_INT_INF`    | `(QInt)INT32_MAX`     | `0Wi`   |
| `Q_INT_NINF`   | `(QInt)-INT32_MAX`    | `-0Wi`  |
| `Q_LONG_NULL`  | `(QLong)INT64_MIN`    | `0N`    |
| `Q_LONG_INF`   | `(QLong)INT64_MAX`    | `0W`    |
| `Q_LONG_NINF`  | `(QLong)-INT64_MAX`   | `-0W`   |

The null value is the minimum value of the type, so negative infinity is one greater than it.

### Floating-Point Types

| Macro          | Definition           | Q value |
| -------------- | -------------------- | ------- |
| `Q_REAL_NULL`  | `(QReal)NAN`         | `0Ne`   |
| `Q_REAL_INF`   | `(QReal)INFINITY`    | `0We`   |
| `Q_REAL_NINF`  | `(QReal)-INFINITY`   | `-0We`  |
| `Q_FLOAT_NULL` | `(QFloat)NAN`        | `0n`    |
| `Q_FLOAT_INF`  | `(QFloat)INFINITY`   | `0w`    |
| `Q_FLOAT_NINF` | `(QFloat)-INFINITY`  | `-0w`   |

Q treats any NaN as null, and NaN never compares equal to anything (including itself), so test for a null with `isnan()` rather than `==`:

```c
if (isnan(qGetFloat(obj))) {
    // null
}
```

### Char, Symbol, and GUID

| Name            | Definition                     | Q value |
| --------------- | ------------------------------ | ------- |
| `Q_CHAR_NULL`   | `(QChar)' '`                   | `" "`   |
| `Q_SYMBOL_NULL` | `""`                           | `` ` `` |
| `Q_GUID_NULL`   | `QGuid` constant of all zero bytes | `0Ng`   |

Symbols are interned, so a null symbol should be detected by checking for an empty string (`sym[0] == '\0'`) rather than by comparing pointers.

`Q_GUID_NULL` is a constant object rather than a macro (`static const` in C, `static constexpr` in C++), so it can be used anywhere a `QGuid` value is needed:

```c
QObj *nullGuid = qNewGuid(Q_GUID_NULL);
```

### Temporal Types

Temporal types have no macros of their own. They use the null and infinity values of their underlying representation:

| Q types                             | Use          |
| ----------------------------------- | ------------ |
| timestamp, timespan                 | `Q_LONG_*`   |
| month, date, minute, second, time   | `Q_INT_*`    |
| datetime                            | `Q_FLOAT_*`  |

Boolean and byte have no null or infinity values.

## Type Codes

`QTypeCode` defines the type codes used by Q.

The enumeration uses a C23 fixed underlying type:

```c
typedef enum : QType {
    ...
} QTypeCode;
```

### Atom and List Types

The primitive Q types use type codes `1` through `19`, with the exception of `3`.

For a primitive type code `n`:

- `-n` identifies an atom.
- `n` identifies a list.

| Code | Type      |  Atom | List |
| ---: | --------- | ----: | ---: |
|  `1` | boolean   |  `-1` |  `1` |
|  `2` | guid      |  `-2` |  `2` |
|  `4` | byte      |  `-4` |  `4` |
|  `5` | short     |  `-5` |  `5` |
|  `6` | int       |  `-6` |  `6` |
|  `7` | long      |  `-7` |  `7` |
|  `8` | real      |  `-8` |  `8` |
|  `9` | float     |  `-9` |  `9` |
| `10` | char      | `-10` | `10` |
| `11` | symbol    | `-11` | `11` |
| `12` | timestamp | `-12` | `12` |
| `13` | month     | `-13` | `13` |
| `14` | date      | `-14` | `14` |
| `15` | datetime  | `-15` | `15` |
| `16` | timespan  | `-16` | `16` |
| `17` | minute    | `-17` | `17` |
| `18` | second    | `-18` | `18` |
| `19` | time      | `-19` | `19` |

`Q_TYPE_MIXED` is `0` and represents a mixed list.

### `QTypeCode` Values

| Constant                 | Value      | Description                                      |
| ------------------------ | ---------: | ------------------------------------------------ |
| `Q_TYPE_ERROR`           |   `-128`   | Error                                            |
| `Q_TYPE_MIXED`           |      `0`   | Mixed list                                       |
| `Q_TYPE_BOOLEAN`         |      `1`   | Boolean                                          |
| `Q_TYPE_GUID`            |      `2`   | GUID                                             |
| `Q_TYPE_BYTE`            |      `4`   | Byte                                             |
| `Q_TYPE_SHORT`           |      `5`   | Short                                            |
| `Q_TYPE_INT`             |      `6`   | Int                                              |
| `Q_TYPE_LONG`            |      `7`   | Long                                             |
| `Q_TYPE_REAL`            |      `8`   | Real                                             |
| `Q_TYPE_FLOAT`           |      `9`   | Float                                            |
| `Q_TYPE_CHAR`            |     `10`   | Char                                             |
| `Q_TYPE_SYMBOL`          |     `11`   | Symbol                                           |
| `Q_TYPE_TIMESTAMP`       |     `12`   | Timestamp                                        |
| `Q_TYPE_MONTH`           |     `13`   | Month                                            |
| `Q_TYPE_DATE`            |     `14`   | Date                                             |
| `Q_TYPE_DATETIME`        |     `15`   | Datetime                                         |
| `Q_TYPE_TIMESPAN`        |     `16`   | Timespan                                         |
| `Q_TYPE_MINUTE`          |     `17`   | Minute                                           |
| `Q_TYPE_SECOND`          |     `18`   | Second                                           |
| `Q_TYPE_TIME`            |     `19`   | Time                                             |
| `Q_TYPE_ENUM_FIRST`      |     `20`   | Enumerated list (first type code)                |
| `Q_TYPE_ENUM_LAST`       |     `76`   | Enumerated list (last type code)                 |
| `Q_TYPE_ANYMAP`          |     `77`   | Mapped list of lists of any type                 |
| `Q_TYPE_NESTED_FIRST`    |     `78`   | Mapped nested list (first type code)             |
| `Q_TYPE_NESTED_LAST`     |     `96`   | Mapped nested list (last type code)              |
| `Q_TYPE_TABLE`           |     `98`   | Table                                            |
| `Q_TYPE_DICTIONARY`      |     `99`   | Dictionary                                       |
| `Q_TYPE_LAMBDA`          |    `100`   | Lambda                                           |
| `Q_TYPE_UNARY_PRIMITIVE` |    `101`   | Unary primitive                                  |
| `Q_TYPE_OPERATOR`        |    `102`   | Operator (binary primitive)                      |
| `Q_TYPE_ITERATOR`        |    `103`   | Iterator                                         |
| `Q_TYPE_PROJECTION`      |    `104`   | Projection                                       |
| `Q_TYPE_COMPOSITION`     |    `105`   | Composition                                      |
| `Q_TYPE_EACH`            |    `106`   | `f'`                                             |
| `Q_TYPE_OVER`            |    `107`   | `f/`                                             |
| `Q_TYPE_SCAN`            |    `108`   | `f\`                                             |
| `Q_TYPE_EACH_PRIOR`      |    `109`   | `f':`                                            |
| `Q_TYPE_EACH_RIGHT`      |    `110`   | `f/:`                                            |
| `Q_TYPE_EACH_LEFT`       |    `111`   | `f\:`                                            |
| `Q_TYPE_DYNAMIC_LOAD`    |    `112`   | Dynamic load (foreign function)                  |

### Tables and Dictionaries

`Q_TYPE_TABLE` and `Q_TYPE_DICTIONARY` use the values `98` and `99`, respectively.

Unlike the primitive types, these values do not follow the atom/list type-code convention. They identify compound Q objects.

### Errors

`Q_TYPE_ERROR` (`-128`) is the type of the error object returned by `qCheckError` when an error has occurred. Use `qIsError` to test for it.

### Enumerations and Mapped Lists

Enumerated lists use type codes `20` to `76` (`Q_TYPE_ENUM_FIRST` to `Q_TYPE_ENUM_LAST`); the corresponding atoms use the negated codes.

`Q_TYPE_ANYMAP` (`77`) is a mapped list of lists of any type. Mapped nested lists of a primitive type `t` have type code `77 + t`, from `Q_TYPE_NESTED_FIRST` (`78`) to `Q_TYPE_NESTED_LAST` (`96`).

### Functions

Type codes `100` to `112` identify the different kinds of Q function, from `Q_TYPE_LAMBDA` to `Q_TYPE_DYNAMIC_LOAD`.

## Type Representation

The type code and the C representation are related but are not necessarily one-to-one.

Several Q types share the same underlying C representation:

| Q types                                | C representation |
| -------------------------------------- | ---------------- |
| boolean, byte, char                    | `QByte`          |
| short                                  | `QShort`         |
| int, month, date, minute, second, time | `QInt`           |
| long, timestamp, timespan              | `QLong`          |
| real                                   | `QReal`          |
| float, datetime                        | `QFloat`         |

The Q type code must therefore be used to distinguish values that share the same C representation.

For example, both a Q `long` and a Q `timestamp` are represented by an `int64_t`, but their Q type codes are different:

```c
Q_TYPE_LONG       // 7
Q_TYPE_TIMESTAMP  // 12
```

## Headers

`type.h` includes:

```c
#include <math.h>
#include <stdint.h>
```

`<stdint.h>` provides the fixed-width integer types used by the Q representations, while `<math.h>` provides `NAN` and `INFINITY` used by the floating-point special-value macros.
