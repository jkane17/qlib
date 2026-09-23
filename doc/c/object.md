# Q Object (`QObj`)

`QObj` is the C representation of a Q value. It contains the value's type and metadata, together with storage for the value itself.

A `QObj` can represent Q atoms, lists, symbols, and nested objects such as tables and keyed tables.

## Definition

```c
typedef struct _q0 {
    signed char _reserved_m; // internal value
    signed char _reserved_a; // internal value

    QType type;   // data type (negative for atom, zero or postive for list)
    QAttr attr;   // attribute flag (sorted, unique, grouped, parted)
    int32_t refs; // reference count

    union {
        QByte qbyte;     // boolean, byte, char
        QShort qshort;   // short
        QInt qint;       // int, month, date, minute, second, time
        QLong qlong;     // long, timestamp, timespan
        QReal qreal;     // real
        QFloat qfloat;   // float, datetime
        QSymbol qsymbol; // symbol

        struct _q0 *nested; // table (points to its column dictionary)

        struct {
            QSize length; // number of elements in list
            QByte list[]; // start of contiguous list data
        };
    };
} QObj;
```

## Layout Compatibility

`QObj` must have exactly the same memory layout as the kdb+ `K` object (`struct k0`), since objects are created and read by the kdb+ runtime. `obj.h` enforces this at compile time with `static_assert` checks on the size of `QObj` and the offsets of its fields.

The anonymous list structure containing a flexible array member inside a union is a GCC/Clang extension rather than standard C or C++, so a compiler supporting this extension is required.

## Fields

### `_reserved_m`

Reserved for internal use.

Applications should not access or modify this field directly.

### `_reserved_a`

Reserved for internal use.

Applications should not access or modify this field directly.

### `type`

The Q type of the value.

`QType` corresponds to Q's type values. Negative values represent atoms, while zero and positive values represent lists.

For example:

| Q type | Meaning    |
| -----: | ---------- |
|   `-7` | long atom  |
|    `7` | long list  |
|   `-9` | float atom |
|    `9` | float list |

See [Q Type](/doc/c/type.md) for the complete list of Q type values.

The `type` field also determines how the storage in the `union` should be interpreted.

### `attr`

Attribute associated with the value.

The value is one of the `QAttrCode` constants: `Q_ATTR_NONE`, `Q_ATTR_SORTED`, `Q_ATTR_UNIQUE`, `Q_ATTR_PARTED`, or `Q_ATTR_GROUPED` (see [Q Types](/doc/c/type.md#qattr)).

Attributes are applicable to lists where supported.

### `refs`

Reference count used for object memory management.

A value of `0` indicates that there are no additional references to the object. Positive values indicate that the object has additional references.

Objects with additional references may be shared, so code that mutates a `QObj` should take its reference count into account.

When the reference count is decremented below zero, the object's allocated memory is released.

## Value Storage

The value is stored in the anonymous `union`. The appropriate union member is determined by `type`.

A C `union` provides a single region of storage shared by all of its members. Its size is sufficient to contain its largest member.

### `qbyte`

```c
QByte qbyte;
```

Storage for Q one-byte types:

- boolean
- byte
- char

### `qshort`

```c
QShort qshort;
```

Storage for the Q short type.

### `qint`

```c
QInt qint;
```

Storage for Q four-byte integer-based types:

- int
- month
- date
- minute
- second
- time

### `qlong`

```c
QLong qlong;
```

Storage for Q eight-byte integer-based types:

- long
- timestamp
- timespan

### `qreal`

```c
QReal qreal;
```

Storage for the Q real type.

### `qfloat`

```c
QFloat qfloat;
```

Storage for Q floating-point types:

- float
- datetime

### `qsymbol`

```c
QSymbol qsymbol;
```

Storage for a Q symbol.

`QSymbol` is pointer-sized and refers to the symbol's storage.

### `nested`

```c
struct _q0 *nested;
```

Pointer to another `QObj`.

Used only by tables (type `98`). A table's `nested` member points to its column dictionary (type `99`), whose keys are the column names and whose values are the columns.

Dictionaries and keyed tables (both type `99`) do not use `nested`. They are stored as a two-item list: the `list` storage holds a pointer to the keys, followed by a pointer to the values. For a keyed table, both the keys and the values are tables.

The `_q0` structure tag allows `QObj` to refer to itself.

## List Storage

Lists use the following structure:

```c
struct {
    QSize length;
    QByte list[];
};
```

### `length`

```c
QSize length;
```

Number of elements in the list.

### `list`

```c
QByte list[];
```

Flexible array member containing the list's contiguous data.

The data is stored as bytes and must be interpreted as the appropriate element type based on the Q type.

For example, a long list uses the same underlying `QByte` storage but its elements are accessed as `QLong` values.

Because `list` is a flexible array member, it does not contribute to `sizeof(QObj)`. Its storage is allocated after the fixed portion of the object.

For example, conceptually:

```text
┌─────────────────────────┐
│ QObj                    │
│ ├─ _reserved_m          │
│ ├─ _reserved_a          │
│ ├─ type                 │
│ ├─ attr                 │
│ ├─ refs                 │
│ └─ union                │
│    └─ length            │
├─────────────────────────┤
│ list[0]                 │
│ list[1]                 │
│ ...                     │
│ list[n-1]               │
└─────────────────────────┘
```

The amount of storage required for a list is therefore:

```c
sizeof(QObj) + length * element_size
```

where `element_size` is the size of the list's element type.

## Size

The size of `QObj` is determined by the fixed fields and the largest member of its union. The flexible array member `list[]` does not contribute to the result of `sizeof`.

The fields have fixed sizes (`QType` and `QAttr` are one byte each, and `QSize` is a 64-bit integer), and the flexible array member contributes no bytes to `sizeof`:

```text
_reserved_m   1 byte
_reserved_a   1 byte
type          1 byte
attr          1 byte
refs          4 bytes
union         8 bytes
              --------
              16 bytes
```

Thus:

```c
sizeof(QObj) == 16
```

This is guaranteed rather than platform-dependent: it must match the kdb+ `K` object, and `obj.h` checks it (along with the field offsets) using `static_assert` (see [Layout Compatibility](#layout-compatibility)). A list's elements therefore always start 16 bytes after the start of the object.

## Self-Referential Structure

`QObj` is declared using the structure tag `_q0`:

```c
typedef struct _q0 {
    ...
} QObj;
```

The tag allows the structure to refer to itself before the `typedef` name `QObj` has been introduced.

This is required for the `nested` member:

```c
struct _q0 *nested;
```

After the declaration, both forms refer to the same structure type:

```c
QObj *obj;
struct _q0 *obj;
```

## Type and Storage

The `type` field determines which member of the union contains the value.

For example, a long atom uses:

```c
obj->type = -7;
obj->qlong = ...;
```

while a float atom uses:

```c
obj->type = -9;
obj->qfloat = ...;
```

The union itself does not record which member is being used. The `type` field provides the information required to interpret the union correctly.

Accessing a union member that does not correspond to the object's `type` should therefore be avoided.
