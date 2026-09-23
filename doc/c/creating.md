# `QObj` Creation

This document provides comprehensive documentation for Q object creation functions from [`new.h`](/src/c/cdk/new.h). These functions create Q objects containing various atomic, list, and complex types.

## Table of Contents

- [Conventions](#conventions)
- [Atom Creation](#atom-creation)
    - [`qNewBoolean`](#qnewboolean)
    - [`qNewGuid`](#qnewguid)
    - [`qNewByte`](#qnewbyte)
    - [`qNewShort`](#qnewshort)
    - [`qNewInt`](#qnewint)
    - [`qNewLong`](#qnewlong)
    - [`qNewReal`](#qnewreal)
    - [`qNewFloat`](#qnewfloat)
    - [`qNewChar`](#qnewchar)
    - [`qNewSymbol`](#qnewsymbol)
    - [`qNewTimestamp`](#qnewtimestamp)
    - [`qNewMonth`](#qnewmonth)
    - [`qNewDate`](#qnewdate)
    - [`qNewDatetime`](#qnewdatetime)
    - [`qNewTimespan`](#qnewtimespan)
    - [`qNewMinute`](#qnewminute)
    - [`qNewSecond`](#qnewsecond)
    - [`qNewTime`](#qnewtime)
- [List Creation](#list-creation)
    - [`qNewList`](#qnewlist)
    - [`qNewBooleanList`](#qnewbooleanlist)
    - [`qNewGuidList`](#qnewguidlist)
    - [`qNewByteList`](#qnewbytelist)
    - [`qNewShortList`](#qnewshortlist)
    - [`qNewIntList`](#qnewintlist)
    - [`qNewLongList`](#qnewlonglist)
    - [`qNewRealList`](#qnewreallist)
    - [`qNewFloatList`](#qnewfloatlist)
    - [`qNewCharList`](#qnewcharlist)
    - [`qNewCharListFromString`](#qnewcharlistfromstring)
    - [`qNewCharListFromFixedString`](#qnewcharlistfromfixedstring)
    - [`qNewSymbolList`](#qnewsymbollist)
    - [`qNewTimestampList`](#qnewtimestamplist)
    - [`qNewMonthList`](#qnewmonthlist)
    - [`qNewDateList`](#qnewdatelist)
    - [`qNewDatetimeList`](#qnewdatetimelist)
    - [`qNewTimespanList`](#qnewtimespanlist)
    - [`qNewMinuteList`](#qnewminutelist)
    - [`qNewSecondList`](#qnewsecondlist)
    - [`qNewTimeList`](#qnewtimelist)
- [Complex Creation](#complex-creation)
    - [`qNewMixedList`](#qnewmixedlist)
    - [`qNewMixedListVar`](#qnewmixedlistvar)
    - [`qNewDict`](#qnewdict)
    - [`qNewTable`](#qnewtable)
    - [`qNewTableVar`](#qnewtablevar)
    - [`qNewKeyedTable`](#qnewkeyedtable)
    - [`qKeyTable`](#qkeytable)
    - [`qUnkeyTable`](#qunkeytable)

## Conventions

**Errors.** On failure, a creation function returns a null pointer and sets the global error indicator. Use `qCheckError` to retrieve the error as an error object (type `Q_TYPE_ERROR`) and `qGetError` to read its message. Call `qCheckError` immediately after the failing call, since it clears the indicator:

```c
QObj *list = qNewLongList(NULL, 3);
if (!list) {
    QObj *error = qCheckError(list);
    printf("error: %s\n", qGetError(error)); // error: domain
}
```

The errors returned by this library are:

| Error    | Cause                                                                                     |
| -------- | ----------------------------------------------------------------------------------------- |
| `domain` | A required pointer argument is `NULL`, or a length or count is outside the supported range |
| `type`   | An argument has the wrong Q type (e.g. a table header that is not a symbol list)           |
| `length` | Arguments that must have the same count do not (e.g. dictionary keys and values)           |

**Ownership.** Functions documented as taking ownership of an argument release it (see [Memory](/doc/c/memory.md)) whether they succeed or fail, so the caller must not use or release that argument afterwards. To keep using an object after passing it to such a function, call `incRef` on it first.

**Examples.** For brevity, the examples in this document do not release the objects they create. Real code should call `decRef` on each object it owns once it is no longer needed.

## Atom Creation

### `qNewBoolean`

Create a Q object containing a `boolean` atom.

```c
QObj *qNewBoolean(QBoolean value);
```

**Parameters**

| Parameter | Description                             |
| --------- | --------------------------------------- |
| `value`   | Value to create the `boolean` atom from |

**Returns**

A pointer to a Q object containing a `boolean` atom.

**Example**

Zero creates false (`0`), and any non-zero value creates true (`1`):

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *boolObj0 = qNewBoolean(0);
    QObj *boolObj1 = qNewBoolean(1);
    QObj *boolObj2 = qNewBoolean(2);
    QObj *boolObjNeg = qNewBoolean(-1);

    printf("bool0 = %hhu\n", qGetBoolean(boolObj0));
    printf("bool1 = %hhu\n", qGetBoolean(boolObj1));
    printf("bool2 = %hhu\n", qGetBoolean(boolObj2));
    printf("boolNeg = %hhu\n", qGetBoolean(boolObjNeg));

    return 0;
}
```

Output:

```
bool0 = 0
bool1 = 1
bool2 = 1
boolNeg = 1
```

### `qNewGuid`

Create a Q object containing a `guid` atom.

```c
QObj *qNewGuid(QGuid value);
```

**Parameters**

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| `value`   | Raw value to create the `guid` atom from |

**Returns**

A pointer to a Q object containing a `guid` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *guidObj = qNewGuid((QGuid){{
        0x12, 0x34, 0x56, 0x78,
        0x9A, 0xBC, 0xDE, 0xF0,
        0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88
    }});

    const QGuid *guid = qGetGuid(guidObj);

    static const char hex[] = "0123456789abcdef";
    char buffer[37];
    QSize pos = 0;

    for (QSize i = 0; i < 16; i++) {
        buffer[pos++] = hex[guid->bytes[i] >> 4];
        buffer[pos++] = hex[guid->bytes[i] & 0x0F];
        // Insert hyphens after 4-2-2-2 bytes
        if (i == 3 || i == 5 || i == 7 || i == 9) buffer[pos++] = '-';
    }

    buffer[pos] = '\0';

    printf("guid = %s\n", buffer);

    return 0;
}
```

Output:

```
guid = 12345678-9abc-def0-1122-334455667788
```

### `qNewByte`

Create a Q object containing a `byte` atom.

```c
QObj *qNewByte(QBytes value);
```

**Parameters**

| Parameter | Description                          |
| --------- | ------------------------------------ |
| `value`   | Value to create the `byte` atom from |

**Returns**

A pointer to a Q object containing a `byte` atom.

**Example**

`byte` values wrap their values when the given value overflows its bounds:

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *byteObj0 = qNewByte(0);
    QObj *byteObj1 = qNewByte(10);
    QObj *byteObj2 = qNewByte((QByte)256); // Wraps to start (0x00)
    QObj *byteObj3 = qNewByte((QByte)-1);  // Wraps to end (0xff)

    printf("byte0 = 0x%02x\n", qGetByte(byteObj0));
    printf("byte1 = 0x%02x\n", qGetByte(byteObj1));
    printf("byte2 = 0x%02x\n", qGetByte(byteObj2));
    printf("byte3 = 0x%02x\n", qGetByte(byteObj3));

    return 0;
}
```

Output:

```
byte0 = 0x00
byte1 = 0x0a
byte2 = 0x00
byte3 = 0xff
```

### `qNewShort`

Create a Q object containing a `short` atom.

```c
QObj *qNewShort(QShort value);
```

**Parameters**

| Parameter | Description                           |
| --------- | ------------------------------------- |
| `value`   | Value to create the `short` atom from |

**Returns**

A pointer to a Q object containing a `short` atom.

**Example**

`short` values wrap their values when the given value overflows its bounds:

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *shortObj0 = qNewShort(0);
    QObj *shortObj1 = qNewShort(10);
    QObj *shortObj2 = qNewShort((QShort)32768);  // Wraps to -32768
    QObj *shortObj3 = qNewShort((QShort)-32769); // Wraps to 32767

    printf("short0 = %d\n", qGetShort(shortObj0));
    printf("short1 = %d\n", qGetShort(shortObj1));
    printf("short2 = %d\n", qGetShort(shortObj2));
    printf("short3 = %d\n", qGetShort(shortObj3));

    return 0;
}
```

Output:

```
short0 = 0
short1 = 10
short2 = -32768
short3 = 32767
```

### `qNewInt`

Create a Q object containing an `int` atom.

```c
QObj *qNewInt(QInt value);
```

**Parameters**

| Parameter | Description                         |
| --------- | ----------------------------------- |
| `value`   | Value to create the `int` atom from |

**Returns**

A pointer to a Q object containing an `int` atom.

**Example**

`int` values wrap their values when the given value overflows its bounds:

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *intObj0 = qNewInt(0);
    QObj *intObj1 = qNewInt(10);
    QObj *intObj2 = qNewInt((QInt)2147483648);  // Wraps to -2147483648
    QObj *intObj3 = qNewInt((QInt)-2147483649); // Wraps to 2147483647

    printf("int0 = %d\n", qGetInt(intObj0));
    printf("int1 = %d\n", qGetInt(intObj1));
    printf("int2 = %d\n", qGetInt(intObj2));
    printf("int3 = %d\n", qGetInt(intObj3));

    return 0;
}
```

Output:

```
int0 = 0
int1 = 10
int2 = -2147483648
int3 = 2147483647
```

### `qNewLong`

Create a Q object containing a `long` atom.

```c
QObj *qNewLong(QLong value);
```

**Parameters**

| Parameter | Description                          |
| --------- | ------------------------------------ |
| `value`   | Value to create the `long` atom from |

**Returns**

A pointer to a Q object containing a `long` atom.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QObj *longObj0 = qNewLong(0);
    QObj *longObj1 = qNewLong(1234567890123LL);
    QObj *longObj2 = qNewLong(-1234567890123LL);

    printf("long0 = %" PRId64 "\n", qGetLong(longObj0));
    printf("long1 = %" PRId64 "\n", qGetLong(longObj1));
    printf("long2 = %" PRId64 "\n", qGetLong(longObj2));

    return 0;
}
```

Output:

```
long0 = 0
long1 = 1234567890123
long2 = -1234567890123
```

### `qNewReal`

Create a Q object containing a `real` atom.

```c
QObj *qNewReal(QReal value);
```

**Parameters**

| Parameter | Description                          |
| --------- | ------------------------------------ |
| `value`   | Value to create the `real` atom from |

**Returns**

A pointer to a Q object containing a `real` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *realObj0 = qNewReal(0.0f);
    QObj *realObj1 = qNewReal(3.14f);
    QObj *realObj2 = qNewReal(-3.14f);

    printf("real0 = %f\n", qGetReal(realObj0));
    printf("real1 = %f\n", qGetReal(realObj1));
    printf("real2 = %f\n", qGetReal(realObj2));

    return 0;
}
```

Output:

```
real0 = 0.000000
real1 = 3.140000
real2 = -3.140000
```

### `qNewFloat`

Create a Q object containing a `float` atom.

```c
QObj *qNewFloat(QFloat value);
```

**Parameters**

| Parameter | Description                           |
| --------- | ------------------------------------- |
| `value`   | Value to create the `float` atom from |

**Returns**

A pointer to a Q object containing a `float` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *floatObj0 = qNewFloat(0.0);
    QObj *floatObj1 = qNewFloat(3.141592653589793);
    QObj *floatObj2 = qNewFloat(-3.141592653589793);

    printf("float0 = %.15f\n", qGetFloat(floatObj0));
    printf("float1 = %.15f\n", qGetFloat(floatObj1));
    printf("float2 = %.15f\n", qGetFloat(floatObj2));

    return 0;
}
```

Output:

```
float0 = 0.000000000000000
float1 = 3.141592653589793
float2 = -3.141592653589793
```

### `qNewChar`

Create a Q object containing a `char` atom.

```c
QObj *qNewChar(QChar value);
```

**Parameters**

| Parameter | Description                          |
| --------- | ------------------------------------ |
| `value`   | Value to create the `char` atom from |

**Returns**

A pointer to a Q object containing a `char` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *charObj0 = qNewChar('a');
    QObj *charObj1 = qNewChar('Z');
    QObj *charObj2 = qNewChar('\n');

    printf("char0 = %c\n", qGetChar(charObj0));
    printf("char1 = %c\n", qGetChar(charObj1));
    printf("char2 = %c\n", qGetChar(charObj2));
    printf("End\n");

    return 0;
}
```

Output:

```
char0 = a
char1 = Z
char2 =

End
```

### `qNewSymbol`

Create a Q object containing a `symbol` atom.

```c
QObj *qNewSymbol(const QChar *value);
```

**Parameters**

| Parameter | Description                            |
| --------- | -------------------------------------- |
| `value`   | Value to create the `symbol` atom from |

**Returns**

A pointer to a Q object containing a `symbol` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *symbolObj = qNewSymbol("hello");

    printf("symbol = %s\n", qGetSymbol(symbolObj));

    return 0;
}
```

Output:

```
symbol = hello
```

### `qNewTimestamp`

Create a Q object containing a `timestamp` atom.

```c
QObj *qNewTimestamp(QTimestamp value);
```

**Parameters**

| Parameter | Description                                     |
| --------- | ----------------------------------------------- |
| `value`   | Nanoseconds since 2000.01.01D00:00:00.000000000 |

**Returns**

A pointer to a Q object containing a `timestamp` atom.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QObj *timestampObj = qNewTimestamp(821977817123456789LL);

    printf("timestamp = %" PRId64 "\n", qGetTimestamp(timestampObj));

    return 0;
}
```

Output:

```
timestamp = 821977817123456789
```

### `qNewMonth`

Create a Q object containing a `month` atom.

```c
QObj *qNewMonth(QMonth value);
```

**Parameters**

| Parameter | Description                           |
| --------- | ------------------------------------- |
| `value`   | Value to create the `month` atom from |

**Returns**

A pointer to a Q object containing a `month` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *monthObj = qNewMonth(312);

    printf("month = %d\n", qGetMonth(monthObj));

    return 0;
}
```

Output:

```
month = 312
```

### `qNewDate`

Create a Q object containing a `date` atom.

```c
QObj *qNewDate(QDate value);
```

**Parameters**

| Parameter | Description                     |
| --------- | ------------------------------- |
| `value`   | Number of days since 2000.01.01 |

**Returns**

A pointer to a Q object containing a `date` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *dateObj = qNewDate(9513);

    printf("date = %d\n", qGetDate(dateObj));

    return 0;
}
```

Output:

```
date = 9513
```

### `qNewDatetime`

Create a Q object containing a `datetime` atom.

```c
QObj *qNewDatetime(QDatetime value);
```

**Parameters**

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| `value`   | Value to create the `datetime` atom from |

**Returns**

A pointer to a Q object containing a `datetime` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *datetimeObj = qNewDatetime(9513.6321426273153);

    printf("datetime = %.15f\n", qGetDatetime(datetimeObj));

    return 0;
}
```

Output:

```
datetime = 9513.632142627315261
```

### `qNewTimespan`

Create a Q object containing a `timespan` atom.

```c
QObj *qNewTimespan(QTimespan value);
```

**Parameters**

| Parameter | Description                   |
| --------- | ----------------------------- |
| `value`   | Timespan value in nanoseconds |

**Returns**

A pointer to a Q object containing a `timespan` atom.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QObj *timespanObj = qNewTimespan(1473576123456789LL);

    printf("timespan = %" PRId64 "\n", qGetTimespan(timespanObj));

    return 0;
}
```

Output:

```
timespan = 1473576123456789
```

### `qNewMinute`

Create a Q object containing a `minute` atom.

```c
QObj *qNewMinute(QMinute value);
```

**Parameters**

| Parameter | Description                   |
| --------- | ----------------------------- |
| `value`   | Number of minutes since 00:00 |

**Returns**

A pointer to a Q object containing a `minute` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *minuteObj = qNewMinute(980);

    printf("minute = %d\n", qGetMinute(minuteObj));

    return 0;
}
```

Output:

```
minute = 980
```

### `qNewSecond`

Create a Q object containing a `second` atom.

```c
QObj *qNewSecond(QSecond value);
```

**Parameters**

| Parameter | Description                      |
| --------- | -------------------------------- |
| `value`   | Number of seconds since 00:00:00 |

**Returns**

A pointer to a Q object containing a `second` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *secondObj = qNewSecond(58817);

    printf("second = %d\n", qGetSecond(secondObj));

    return 0;
}
```

Output:

```
second = 58817
```

### `qNewTime`

Create a Q object containing a `time` atom.

```c
QObj *qNewTime(QTime value);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `value`   | Number of milliseconds since 00:00:00.000 |

**Returns**

A pointer to a Q object containing a `time` atom.

**Example**

```c
#include <stdio.h>
#include "q.h"

int main() {
    QObj *timeObj = qNewTime(58817123);

    printf("time = %d\n", qGetTime(timeObj));

    return 0;
}
```

Output:

```
time = 58817123
```

## List Creation

> Note: For the functions that copy from a `values` array (and `qNewCharListFromFixedString`), `values` may be `NULL` when `length` is `0`, which creates an empty list. A `NULL` pointer with a non-zero length, or a `NULL` string passed to `qNewCharListFromString`, returns a `domain` error.

### `qNewList`

Allocate a Q list of the given type and length without initialising its elements.

```c
QObj *qNewList(QTypeCode type, QSize length);
```

**Parameters**

| Parameter | Description                     |
| --------- | ------------------------------- |
| `type`    | Type code for the list elements |
| `length`  | Number of elements to allocate  |

**Returns**

A pointer to a Q object containing an allocated list.

> Note: The elements are not initialised. Returns a `domain` error if `length` exceeds `Q_SIZE_MAX`.

**Example**

`qNewList` allocates the list storage but does not populate it with values:

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QObj *intList = qNewList(Q_TYPE_INT, 3);

    printf("Type = %d\n", intList->type);
    printf("Length = %" PRIu64 "\n", intList->length);

    return 0;
}
```

Output:

```
Type = 6
Length = 3
```

### `qNewBooleanList`

Create a Q object containing a `boolean` list.

```c
QObj *qNewBooleanList(const QBoolean *values, QSize length);
```

**Parameters**

| Parameter | Description                                  |
| --------- | -------------------------------------------- |
| `values`  | Array of boolean values to populate the list |
| `length`  | Number of elements in the list               |

**Returns**

A pointer to a Q object containing a `boolean` list.

**Example**

Boolean values are normalized to `0` or `1` when the list is created:

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QBoolean values[] = {0, 1, 2, -1};
    QObj *booleanList = qNewBooleanList(values, 4);

    for (QSize i = 0; i < booleanList->length; i++) {
        printf("boolean[%" PRIu64 "] = %hhu\n", i, qGetBooleanAtIndex(booleanList, i));
    }

    return 0;
}
```

Output:

```
boolean[0] = 0
boolean[1] = 1
boolean[2] = 1
boolean[3] = 1
```

### `qNewGuidList`

Create a Q object containing a `guid` list.

```c
QObj *qNewGuidList(const QGuid *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of guid values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `guid` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QGuid values[] = {
        {{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0,
          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
        {{0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
          0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12}}
    };

    QObj *guidList = qNewGuidList(values, 2);

    for (QSize i = 0; i < guidList->length; i++) {
        printf(
            "guid[%" PRIu64 "] = %02x%02x...%02x%02x\n",
            i,
            qGetGuidAtIndex(guidList, i)->bytes[0],
            qGetGuidAtIndex(guidList, i)->bytes[1],
            qGetGuidAtIndex(guidList, i)->bytes[14],
            qGetGuidAtIndex(guidList, i)->bytes[15]
        );
    }

    return 0;
}
```

Output:

```
guid[0] = 1234...7788
guid[1] = 8877...3412
```

### `qNewByteList`

Create a Q object containing a `byte` list.

```c
QObj *qNewByteList(const QByte *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of byte values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `byte` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QByte values[] = {0x00, 0x0A, 0x7F, 0xFF};
    QObj *byteList = qNewByteList(values, 4);

    for (QSize i = 0; i < byteList->length; i++) {
        printf("byte[%" PRIu64 "] = 0x%02x\n", i, qGetByteAtIndex(byteList, i));
    }

    return 0;
}
```

Output:

```
byte[0] = 0x00
byte[1] = 0x0a
byte[2] = 0x7f
byte[3] = 0xff
```

### `qNewShortList`

Create a Q object containing a `short` list.

```c
QObj *qNewShortList(const QShort *values, QSize length);
```

**Parameters**

| Parameter | Description                                |
| --------- | ------------------------------------------ |
| `values`  | Array of short values to populate the list |
| `length`  | Number of elements in the list             |

**Returns**

A pointer to a Q object containing a `short` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QShort values[] = {-32768, -1, 0, 1, 32767};
    QObj *shortList = qNewShortList(values, 5);

    for (QSize i = 0; i < shortList->length; i++) {
        printf("short[%" PRIu64 "] = %d\n", i, qGetShortAtIndex(shortList, i));
    }

    return 0;
}
```

Output:

```
short[0] = -32768
short[1] = -1
short[2] = 0
short[3] = 1
short[4] = 32767
```

### `qNewIntList`

Create a Q object containing an `int` list.

```c
QObj *qNewIntList(const QInt *values, QSize length);
```

**Parameters**

| Parameter | Description                              |
| --------- | ---------------------------------------- |
| `values`  | Array of int values to populate the list |
| `length`  | Number of elements in the list           |

**Returns**

A pointer to a Q object containing an `int` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QInt values[] = {-2147483648, -1, 0, 1, 2147483647};
    QObj *intList = qNewIntList(values, 5);

    for (QSize i = 0; i < intList->length; i++) {
        printf("int[%" PRIu64 "] = %d\n", i, qGetIntAtIndex(intList, i));
    }

    return 0;
}
```

Output:

```
int[0] = -2147483648
int[1] = -1
int[2] = 0
int[3] = 1
int[4] = 2147483647
```

### `qNewLongList`

Create a Q object containing a `long` list.

```c
QObj *qNewLongList(const QLong *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of long values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `long` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QLong values[] = {-1234567890123LL, 0, 1234567890123LL};
    QObj *longList = qNewLongList(values, 3);

    for (QSize i = 0; i < longList->length; i++) {
        printf("long[%" PRIu64 "] = %" PRId64 "\n", i, qGetLongAtIndex(longList, i));
    }

    return 0;
}
```

Output:

```
long[0] = -1234567890123
long[1] = 0
long[2] = 1234567890123
```

### `qNewRealList`

Create a Q object containing a `real` list.

```c
QObj *qNewRealList(const QReal *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of real values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `real` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QReal values[] = {0.0f, 3.14f, -3.14f};
    QObj *realList = qNewRealList(values, 3);

    for (QSize i = 0; i < realList->length; i++) {
        printf("real[%" PRIu64 "] = %f\n", i, qGetRealAtIndex(realList, i));
    }

    return 0;
}
```

Output:

```
real[0] = 0.000000
real[1] = 3.140000
real[2] = -3.140000
```

### `qNewFloatList`

Create a Q object containing a `float` list.

```c
QObj *qNewFloatList(const QFloat *values, QSize length);
```

**Parameters**

| Parameter | Description                                |
| --------- | ------------------------------------------ |
| `values`  | Array of float values to populate the list |
| `length`  | Number of elements in the list             |

**Returns**

A pointer to a Q object containing a `float` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QFloat values[] = {0.0, 3.141592653589793, -3.141592653589793};
    QObj *floatList = qNewFloatList(values, 3);

    for (QSize i = 0; i < floatList->length; i++) {
        printf("float[%" PRIu64 "] = %.15f\n", i, qGetFloatAtIndex(floatList, i));
    }

    return 0;
}
```

Output:

```
float[0] = 0.000000000000000
float[1] = 3.141592653589793
float[2] = -3.141592653589793
```

### `qNewCharList`

Create a Q object containing a `char` list.

```c
QObj *qNewCharList(const QChar *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of char values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `char` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QChar values[] = {'a', 'b', 'c', '\n'};
    QObj *charList = qNewCharList(values, 4);

    for (QSize i = 0; i < charList->length; i++) {
        printf("char[%" PRIu64 "] = %c\n", i, qGetCharAtIndex(charList, i));
    }

    return 0;
}
```

Output:

```
char[0] = a
char[1] = b
char[2] = c
char[3] =

```

### `qNewCharListFromString`

Create a Q object containing a `char` list from a null-terminated string.

```c
QObj *qNewCharListFromString(const QChar *string);
```

**Parameters**

| Parameter | Description                                     |
| --------- | ----------------------------------------------- |
| `string`  | Null-terminated string to convert to char array |

**Returns**

A pointer to a Q object containing a `char` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QChar string[] = "hello";
    QObj *charList = qNewCharListFromString(string);

    for (QSize i = 0; i < charList->length; i++) {
        printf("char[%" PRIu64 "] = %c\n", i, qGetCharAtIndex(charList, i));
    }

    return 0;
}
```

Output:

```
char[0] = h
char[1] = e
char[2] = l
char[3] = l
char[4] = o
```

### `qNewCharListFromFixedString`

Create a Q object containing a `char` list from a fixed length string.

```c
QObj *qNewCharListFromFixedString(const QChar *string, QSize length);
```

**Parameters**

| Parameter | Description                                              |
| --------- | -------------------------------------------------------- |
| `string`  | Fixed length string to convert to char array             |
| `length`  | Number of characters to copy (excluding null terminator) |

**Returns**

A pointer to a Q object containing a `char` list.

> Note: Returns a `domain` error if `length` exceeds `Q_SIZE_MAX`, or if `string` is `NULL` and `length` is non-zero.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QChar string[] = "hello";
    QObj *charList = qNewCharListFromFixedString(string, 5);

    for (QSize i = 0; i < charList->length; i++) {
        printf("char[%" PRIu64 "] = %c\n", i, qGetCharAtIndex(charList, i));
    }

    return 0;
}
```

Output:

```
char[0] = h
char[1] = e
char[2] = l
char[3] = l
char[4] = o
```

### `qNewSymbolList`

Create a Q object containing a `symbol` list.

```c
QObj *qNewSymbolList(const QSymbol *values, QSize length);
```

**Parameters**

| Parameter | Description                                 |
| --------- | ------------------------------------------- |
| `values`  | Array of symbol values to populate the list |
| `length`  | Number of elements in the list              |

**Returns**

A pointer to a Q object containing a `symbol` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol values[] = {"hello", "world", "q"};
    QObj *symbolList = qNewSymbolList(values, 3);

    for (QSize i = 0; i < symbolList->length; i++) {
        printf("symbol[%" PRIu64 "] = %s\n", i, qGetSymbolAtIndex(symbolList, i));
    }

    return 0;
}
```

Output:

```
symbol[0] = hello
symbol[1] = world
symbol[2] = q
```

### `qNewTimestampList`

Create a Q object containing a `timestamp` list.

```c
QObj *qNewTimestampList(const QTimestamp *values, QSize length);
```

**Parameters**

| Parameter | Description                                    |
| --------- | ---------------------------------------------- |
| `values`  | Array of timestamp values to populate the list |
| `length`  | Number of elements in the list                 |

**Returns**

A pointer to a Q object containing a `timestamp` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QTimestamp values[] = {0LL, 821977817123456789LL};
    QObj *timestampList = qNewTimestampList(values, 2);

    for (QSize i = 0; i < timestampList->length; i++) {
        printf("timestamp[%" PRIu64 "] = %" PRId64 "\n", i, qGetTimestampAtIndex(timestampList, i));
    }

    return 0;
}
```

Output:

```
timestamp[0] = 0
timestamp[1] = 821977817123456789
```

### `qNewMonthList`

Create a Q object containing a `month` list.

```c
QObj *qNewMonthList(const QMonth *values, QSize length);
```

**Parameters**

| Parameter | Description                                |
| --------- | ------------------------------------------ |
| `values`  | Array of month values to populate the list |
| `length`  | Number of elements in the list             |

**Returns**

A pointer to a Q object containing a `month` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QMonth values[] = {0, 1, 312};
    QObj *monthList = qNewMonthList(values, 3);

    for (QSize i = 0; i < monthList->length; i++) {
        printf("month[%" PRIu64 "] = %d\n", i, qGetMonthAtIndex(monthList, i));
    }

    return 0;
}
```

Output:

```
month[0] = 0
month[1] = 1
month[2] = 312
```

### `qNewDateList`

Create a Q object containing a `date` list.

```c
QObj *qNewDateList(const QDate *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of date values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `date` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QDate values[] = {0, 9513, 10000};
    QObj *dateList = qNewDateList(values, 3);

    for (QSize i = 0; i < dateList->length; i++) {
        printf("date[%" PRIu64 "] = %d\n", i, qGetDateAtIndex(dateList, i));
    }

    return 0;
}
```

Output:

```
date[0] = 0
date[1] = 9513
date[2] = 10000
```

### `qNewDatetimeList`

Create a Q object containing a `datetime` list.

```c
QObj *qNewDatetimeList(const QDatetime *values, QSize length);
```

**Parameters**

| Parameter | Description                                   |
| --------- | --------------------------------------------- |
| `values`  | Array of datetime values to populate the list |
| `length`  | Number of elements in the list                |

**Returns**

A pointer to a Q object containing a `datetime` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QDatetime values[] = {0.0, 9513.6321426273153};
    QObj *datetimeList = qNewDatetimeList(values, 2);

    for (QSize i = 0; i < datetimeList->length; i++) {
        printf("datetime[%" PRIu64 "] = %.15f\n", i, qGetDatetimeAtIndex(datetimeList, i));
    }

    return 0;
}
```

Output:

```
datetime[0] = 0.000000000000000
datetime[1] = 9513.632142627315261
```

### `qNewTimespanList`

Create a Q object containing a `timespan` list.

```c
QObj *qNewTimespanList(const QTimespan *values, QSize length);
```

**Parameters**

| Parameter | Description                                   |
| --------- | --------------------------------------------- |
| `values`  | Array of timespan values to populate the list |
| `length`  | Number of elements in the list                |

**Returns**

A pointer to a Q object containing a `timespan` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QTimespan values[] = {0LL, 1473576123456789LL};
    QObj *timespanList = qNewTimespanList(values, 2);

    for (QSize i = 0; i < timespanList->length; i++) {
        printf("timespan[%" PRIu64 "] = %" PRId64 "\n", i, qGetTimespanAtIndex(timespanList, i));
    }

    return 0;
}
```

Output:

```
timespan[0] = 0
timespan[1] = 1473576123456789
```

### `qNewMinuteList`

Create a Q object containing a `minute` list.

```c
QObj *qNewMinuteList(const QMinute *values, QSize length);
```

**Parameters**

| Parameter | Description                                 |
| --------- | ------------------------------------------- |
| `values`  | Array of minute values to populate the list |
| `length`  | Number of elements in the list              |

**Returns**

A pointer to a Q object containing a `minute` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QMinute values[] = {0, 60, 980};
    QObj *minuteList = qNewMinuteList(values, 3);

    for (QSize i = 0; i < minuteList->length; i++) {
        printf("minute[%" PRIu64 "] = %d\n", i, qGetMinuteAtIndex(minuteList, i));
    }

    return 0;
}
```

Output:

```
minute[0] = 0
minute[1] = 60
minute[2] = 980
```

### `qNewSecondList`

Create a Q object containing a `second` list.

```c
QObj *qNewSecondList(const QSecond *values, QSize length);
```

**Parameters**

| Parameter | Description                                 |
| --------- | ------------------------------------------- |
| `values`  | Array of second values to populate the list |
| `length`  | Number of elements in the list              |

**Returns**

A pointer to a Q object containing a `second` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSecond values[] = {0, 1, 60, 58817};
    QObj *secondList = qNewSecondList(values, 4);

    for (QSize i = 0; i < secondList->length; i++) {
        printf("second[%" PRIu64 "] = %d\n", i, qGetSecondAtIndex(secondList, i));
    }

    return 0;
}
```

Output:

```
second[0] = 0
second[1] = 1
second[2] = 60
second[3] = 58817
```

### `qNewTimeList`

Create a Q object containing a `time` list.

```c
QObj *qNewTimeList(const QTime *values, QSize length);
```

**Parameters**

| Parameter | Description                               |
| --------- | ----------------------------------------- |
| `values`  | Array of time values to populate the list |
| `length`  | Number of elements in the list            |

**Returns**

A pointer to a Q object containing a `time` list.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QTime values[] = {0, 1000, 58817123};
    QObj *timeList = qNewTimeList(values, 3);

    for (QSize i = 0; i < timeList->length; i++) {
        printf("time[%" PRIu64 "] = %d\n", i, qGetTimeAtIndex(timeList, i));
    }

    return 0;
}
```

Output:

```
time[0] = 0
time[1] = 1000
time[2] = 58817123
```

## Complex Creation

### `qNewMixedList`

Create a Q object containing a mixed list.

```c
QObj *qNewMixedList(QSize length, ...);
```

**Parameters**

| Parameter | Description                            |
| --------- | -------------------------------------- |
| `length`  | Number of elements in the list         |
| `...`     | Q object pointers to populate the list |

**Returns**

A pointer to a Q object containing a mixed list.

> Note: `length` must not exceed `INT32_MAX`, otherwise a `domain` error is returned.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QObj *symbol = qNewSymbol("abc");
    QObj *long_ = qNewLong(10);
    QChar values[] = {'a', 'b', 'c'};
    QObj *chars = qNewCharList(values, 3);

    QObj *mixedList = qNewMixedList(3, symbol, long_, chars);

    printf("Type = %d\n", mixedList->type);                // 0
    printf("Length = %" PRIu64 "\n\n", mixedList->length); // 3

    printf("mixedList[0] = %s\n", qGetSymbol(qGetMixedAtIndex(mixedList, 0)));
    printf("mixedList[1] = %" PRId64 "\n", qGetLong(qGetMixedAtIndex(mixedList, 1)));

    QObj *charList = qGetMixedAtIndex(mixedList, 2);
    for (QSize i = 0; i < charList->length; i++)
        printf("mixedList[2][%" PRIu64 "] = %c\n", i, qGetCharAtIndex(charList, i));

    return 0;
}
```

Output:

```
Type = 0
Length = 3

mixedList[0] = abc
mixedList[1] = 10
mixedList[2][0] = a
mixedList[2][1] = b
mixedList[2][2] = c
```

### `qNewMixedListVar`

Create a Q object containing a mixed list from an existing `va_list`.

```c
QObj *qNewMixedListVar(QSize length, va_list args);
```

**Parameters**

| Parameter | Description                            |
| --------- | -------------------------------------- |
| `length`  | Number of elements in the list         |
| `args`    | Q object pointers to populate the list |

**Returns**

A pointer to a Q object containing a mixed list.

> Note: `length` must not exceed `INT32_MAX`, otherwise a `domain` error is returned.

**Example**

```c
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include "q.h"

static QObj *createMixedList(QSize length, ...) {
    va_list args;
    va_start(args);
    QObj *mixedList = qNewMixedListVar(length, args);
    va_end(args);
    return mixedList;
}

int main() {
    QObj *symbol = qNewSymbol("abc");
    QObj *long_ = qNewLong(10);

    QChar values[] = {'a', 'b', 'c'};
    QObj *chars = qNewCharList(values, 3);

    QObj *mixedList = createMixedList(3, symbol, long_, chars);

    printf("Type = %d\n", mixedList->type);                // 0
    printf("Length = %" PRIu64 "\n\n", mixedList->length); // 3

    printf("mixedList[0] = %s\n", qGetSymbol(qGetMixedAtIndex(mixedList, 0)));
    printf("mixedList[1] = %" PRId64 "\n", qGetLong(qGetMixedAtIndex(mixedList, 1)));

    QObj *charList = qGetMixedAtIndex(mixedList, 2);

    for (QSize i = 0; i < charList->length; i++)
        printf("mixedList[2][%" PRIu64 "] = %c\n", i, qGetCharAtIndex(charList, i));

    return 0;
}
```

Output:

```
Type = 0
Length = 3

mixedList[0] = abc
mixedList[1] = 10
mixedList[2][0] = a
mixedList[2][1] = b
mixedList[2][2] = c
```

### `qNewDict`

Create a Q object containing a dictionary.

```c
QObj *qNewDict(QObj *keys, QObj *values);
```

**Parameters**

| Parameter | Description                                                                      |
| --------- | -------------------------------------------------------------------------------- |
| `keys`    | A pointer to a Q object containing a list of keys or a table (takes ownership)   |
| `values`  | A pointer to a Q object containing a list of values or a table (takes ownership) |

**Returns**

A pointer to a Q object containing a dictionary.

> Note: `keys` and `values` must not be null, must be lists, dictionaries, or tables (not atoms or functions), and must have the same count (see `qGetCount`). Otherwise a `domain`, `type`, or `length` error is returned and both are released.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    QObj *keys = qNewSymbolList(symbols, 3);

    QLong longs[] = {10, 20, 30};
    QObj *values = qNewLongList(longs, 3);

    QObj *dict = qNewDict(keys, values);

    printf("Type = %d\n", dict->type);                      // 99
    printf("Length = %" PRIu64 "\n", dict->length);         // 2 -> List of keys and list of values
    printf("Count = %" PRIu64 "\n\n", qGetDictCount(dict)); // Number of key-value pairs

    QObj *dictKeys = qGetDictKeys(dict);
    QObj *dictValues = qGetDictValues(dict);
    for (QSize i = 0; i < dictKeys->length; i++) {
        printf(
            "%s | %" PRId64 "\n", qGetSymbolAtIndex(dictKeys, i), qGetLongAtIndex(dictValues, i));
    }

    return 0;
}
```

Output:

```
Type = 99
Length = 2
Count = 3

abc | 10
def | 20
ghi | 30
```

### `qNewTable`

Create a Q object containing a table.

```c
QObj *qNewTable(QObj *header, ...);
```

**Parameters**

| Parameter | Description                                                                                                                                                                                     |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `header`  | A pointer to a Q object containing a list of column names (takes ownership)                                                                                                                     |
| `...`     | Q object pointers, each containing a single column (takes ownership). All underlying lists should have equal lengths. There should be as many objects as there are column names (see `header`). |

**Returns**

A pointer to a Q object containing a table.

> Note: `header` length determines the number of columns, while the column length determines the number of rows.

> Note: Returns a `type` error if `header` is not a symbol list (`header` is released), or a `domain` error if `header` or any column is `NULL` (a non-`NULL` `header` and all non-`NULL` columns are released). Columns are not released when `header` is `NULL` or not a symbol list, as the number of columns is unknown.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

static QObj *makeTable() {
    QSymbol symbols[] = {"col0", "col1"};
    QObj *header = qNewSymbolList(symbols, 2);

    QLong longs[] = {1, 2, 3};
    QObj *col0 = qNewLongList(longs, 3);

    QChar chars[] = {'a', 'b', 'c'};
    QObj *col1 = qNewCharList(chars, 3);

    return qNewTable(header, col0, col1);
}

static void printTable(QObj *table) {
    QObj *header = qGetTableHeader(table);
    for (QSize i = 0; i < header->length; i++)
        printf("%s ", qGetSymbolAtIndex(header, i));

    printf("\n---------\n");

    QObj *columns = qGetTableColumns(table);
    QObj *col0 = qGetMixedAtIndex(columns, 0);
    QObj *col1 = qGetMixedAtIndex(columns, 1);

    for (QSize i = 0; i < col0->length; i++) {
        printf("%" PRId64 "    ", qGetLongAtIndex(col0, i));
        printf("%c\n", qGetCharAtIndex(col1, i));
    }
}

int main() {
    QObj *table = makeTable();

    printf("Type = %d\n", table->type);                                  // 98
    printf("Column count = %" PRIu64 "\n", qGetTableColumnCount(table)); // 2
    printf("Row count = %" PRIu64 "\n\n", qGetTableRowCount(table));     // 3

    printTable(table);

    return 0;
}
```

Output:

```
Type = 98
Column count = 2
Row count = 3

col0 col1
---------
1    a
2    b
3    c
```

### `qNewTableVar`

Create a Q object containing a table from an existing `va_list`.

```c
QObj *qNewTableVar(QObj *header, va_list args);
```

**Parameters**

| Parameter | Description                                                                                                                                                                                     |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `header`  | A pointer to a Q object containing a list of column names (takes ownership)                                                                                                                     |
| `args`    | Q object pointers, each containing a single column (takes ownership). All underlying lists should have equal lengths. There should be as many objects as there are column names (see `header`). |

**Returns**

A pointer to a Q object containing a table.

> Note: `header` length determines the number of columns, while the column length determines the number of rows.

> Note: Returns a `type` error if `header` is not a symbol list (`header` is released), or a `domain` error if `header` or any column is `NULL` (a non-`NULL` `header` and all non-`NULL` columns are released). Columns are not released when `header` is `NULL` or not a symbol list, as the number of columns is unknown.

**Example**

```c
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include "q.h"

static QObj *makeTable0(QObj *header, ...) {
    va_list args;
    va_start(args);
    QObj *table = qNewTableVar(header, args);
    va_end(args);
    return table;
}

static QObj *makeTable() {
    QSymbol symbols[] = {"col0", "col1"};
    QObj *header = qNewSymbolList(symbols, 2);

    QLong longs[] = {1, 2, 3};
    QObj *col0 = qNewLongList(longs, 3);

    QChar chars[] = {'a', 'b', 'c'};
    QObj *col1 = qNewCharList(chars, 3);

    return makeTable0(header, col0, col1);
}

static void printTable(QObj *table) {
    QObj *header = qGetTableHeader(table);
    for (QSize i = 0; i < header->length; i++)
        printf("%s ", qGetSymbolAtIndex(header, i));

    printf("\n---------\n");

    QObj *columns = qGetTableColumns(table);
    QObj *col0 = qGetMixedAtIndex(columns, 0);
    QObj *col1 = qGetMixedAtIndex(columns, 1);

    for (QSize i = 0; i < col0->length; i++) {
        printf("%" PRId64 "    ", qGetLongAtIndex(col0, i));
        printf("%c\n", qGetCharAtIndex(col1, i));
    }
}

int main() {
    QObj *table = makeTable();

    printf("Type = %d\n", table->type);                                  // 98
    printf("Column count = %" PRIu64 "\n", qGetTableColumnCount(table)); // 2
    printf("Row count = %" PRIu64 "\n\n", qGetTableRowCount(table));     // 3

    printTable(table);

    return 0;
}
```

Output:

```
Type = 98
Column count = 2
Row count = 3

col0 col1
---------
1    a
2    b
3    c
```

### `qNewKeyedTable`

Create a Q object containing a keyed table.

```c
QObj *qNewKeyedTable(QObj *keys, QObj *values);
```

**Parameters**

| Parameter | Description                                                                                  |
| --------- | -------------------------------------------------------------------------------------------- |
| `keys`    | A pointer to a Q object containing a table that contains the key columns (takes ownership)   |
| `values`  | A pointer to a Q object containing a table that contains the value columns (takes ownership) |

**Returns**

A pointer to a Q object containing a keyed table.

> Note: `keys` and `values` must not be null, must be tables, and must have the same row count. Otherwise a `domain`, `type`, or `length` error is returned and both are released.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

static QObj *makeKeyedTable() {
    QSymbol keySymbols[] = {"key0", "key1"};
    QObj *keyHeader = qNewSymbolList(keySymbols, 2);
    QLong keyLongs[] = {1, 2, 3};
    QObj *key0 = qNewLongList(keyLongs, 3);
    QChar keyChars[] = {'a', 'b', 'c'};
    QObj *key1 = qNewCharList(keyChars, 3);

    QObj *keys = qNewTable(keyHeader, key0, key1);

    QSymbol valueSymbols[] = {"col0", "col1", "col2"};
    QObj *valueHeader = qNewSymbolList(valueSymbols, 3);
    QLong valueLongs[] = {10, 20, 30};
    QObj *col0 = qNewLongList(valueLongs, 3);
    QChar valueChars[] = {'x', 'y', 'z'};
    QObj *col1 = qNewCharList(valueChars, 3);
    QFloat valueFloats[] = {1.2, 3.4, 5.6};
    QObj *col2 = qNewFloatList(valueFloats, 3);

    QObj *values = qNewTable(valueHeader, col0, col1, col2);

    return qNewKeyedTable(keys, values);
}

static void printKeyedTable(QObj *keyedTable) {
    QObj *keys = qGetKeyedTableKeys(keyedTable);
    QObj *values = qGetKeyedTableValues(keyedTable);

    QObj *keysHeader = qGetTableHeader(keys);
    for (QSize i = 0; i < keysHeader->length; i++)
        printf("%s ", qGetSymbolAtIndex(keysHeader, i));

    printf("| ");

    QObj *valuesHeader = qGetTableHeader(values);
    for (QSize i = 0; i < valuesHeader->length; i++)
        printf("%s ", qGetSymbolAtIndex(valuesHeader, i));

    printf("\n--------- | --------------\n");

    QObj *keysColumns = qGetTableColumns(keys);
    QObj *key0 = qGetMixedAtIndex(keysColumns, 0);
    QObj *key1 = qGetMixedAtIndex(keysColumns, 1);

    QObj *valuesColumns = qGetTableColumns(values);
    QObj *col0 = qGetMixedAtIndex(valuesColumns, 0);
    QObj *col1 = qGetMixedAtIndex(valuesColumns, 1);
    QObj *col2 = qGetMixedAtIndex(valuesColumns, 2);

    for (QSize i = 0; i < key0->length; i++) {
        printf("%" PRId64 "    ", qGetLongAtIndex(key0, i));
        printf("%c    | ", qGetCharAtIndex(key1, i));
        printf("%" PRId64 "   ", qGetLongAtIndex(col0, i));
        printf("%c    ", qGetCharAtIndex(col1, i));
        printf("%.1f\n", qGetFloatAtIndex(col2, i));
    }
}

int main() {
    QObj *keyedTable = makeKeyedTable();

    printf("Type = %d\n", keyedTable->type);                                // 99
    printf("Keys type = %d\n", qGetKeyedTableKeys(keyedTable)->type);       // 98
    printf("Values type = %d\n\n", qGetKeyedTableValues(keyedTable)->type); // 98

    printKeyedTable(keyedTable);

    return 0;
}
```

Output:

```
Type = 99
Keys type = 98
Values type = 98

key0 key1 | col0 col1 col2
--------- | --------------
1    a    | 10   x    1.2
2    b    | 20   y    3.4
3    c    | 30   z    5.6
```

### `qKeyTable`

Create a Q object containing a keyed table from a simple table.

```c
QObj *qKeyTable(QSize nkeys, QObj *table);
```

**Parameters**

| Parameter | Description                                               |
| --------- | --------------------------------------------------------- |
| `nkeys`   | Number of leading columns to use as the key columns       |
| `table`   | Pointer to the simple table to split into keys and values (does not take ownership) |

**Returns**

A pointer to a Q object containing a keyed table or a null pointer if an error occurred.

> Note: Does not take ownership of `table`. The first `nkeys` columns become the key table, and the remaining columns become the value table.

> Note: Returns a `type` error if `table` is not a table, or a `domain` error if `nkeys` exceeds `Q_SIZE_MAX`.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol symbols[] = {"key0", "key1", "col0", "col1", "col2"};
    QObj *header = qNewSymbolList(symbols, 5);

    QLong key0[] = {1, 2, 3};
    QObj *keyColumn0 = qNewLongList(key0, 3);

    QChar key1[] = {'a', 'b', 'c'};
    QObj *keyColumn1 = qNewCharList(key1, 3);

    QLong col0[] = {10, 20, 30};
    QObj *valueColumn0 = qNewLongList(col0, 3);

    QChar col1[] = {'a', 'b', 'c'};
    QObj *valueColumn1 = qNewCharList(col1, 3);

    QFloat col2[] = {1.2, 3.4, 5.6};
    QObj *valueColumn2 = qNewFloatList(col2, 3);

    QObj *table =
        qNewTable(header, keyColumn0, keyColumn1, valueColumn0, valueColumn1, valueColumn2);
    QObj *keyedTable = qKeyTable(2, table);

    printf("Key columns = %" PRIu64 "\n", qGetTableColumnCount(qGetKeyedTableKeys(keyedTable)));
    printf("Value columns = %" PRIu64 "\n", qGetTableColumnCount(qGetKeyedTableValues(keyedTable)));

    return 0;
}
```

Output:

```
Key columns = 2
Value columns = 3
```

### `qUnkeyTable`

Create a Q object containing a simple table from a keyed table.

```c
QObj *qUnkeyTable(QObj *keyedTable);
```

**Parameters**

| Parameter    | Description                                      |
| ------------ | ------------------------------------------------ |
| `keyedTable` | Pointer to the keyed table to convert to a table |

**Returns**

A pointer to a Q object containing a simple table.

> Note: `qUnkeyTable` takes ownership of `keyedTable`. The key columns are followed by the value
> columns in the resulting table.

**Example**

```c
#include <inttypes.h>
#include <stdio.h>
#include "q.h"

int main() {
    QSymbol keySymbols[] = {"key"};
    QObj *keyHeader = qNewSymbolList(keySymbols, 1);
    QLong keyValues[] = {1, 2, 3};
    QObj *keyColumn = qNewLongList(keyValues, 3);
    QObj *keys = qNewTable(keyHeader, keyColumn);

    QSymbol valueSymbols[] = {"value"};
    QObj *valueHeader = qNewSymbolList(valueSymbols, 1);
    QChar valueValues[] = {'a', 'b', 'c'};
    QObj *valueColumn = qNewCharList(valueValues, 3);
    QObj *values = qNewTable(valueHeader, valueColumn);

    QObj *keyedTable = qNewKeyedTable(keys, values);
    QObj *table = qUnkeyTable(keyedTable);

    printf("Columns = %" PRIu64 "\n", qGetTableColumnCount(table));
    printf("Rows = %" PRIu64 "\n", qGetTableRowCount(table));

    return 0;
}
```

Output:

```
Columns = 2
Rows = 3
```
