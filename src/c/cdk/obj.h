
/**
 * @file obj.h
 * @brief Q object definition.
 */

#ifndef QLIB_OBJ_H
#define QLIB_OBJ_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "type.h"

// Q object representation
//
// The layout must match the kdb+ K object (struct k0 in k.h) exactly. The anonymous list struct
// containing a flexible array member inside a union is a GCC/Clang extension (not standard C or
// C++), so only compilers supporting it are supported.
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

// Guard against the layout drifting from the kdb+ K object
static_assert(sizeof(QObj) == 16, "QObj size must match kdb+ K object");
static_assert(offsetof(QObj, type) == 2, "QObj type offset must match kdb+ K object");
static_assert(offsetof(QObj, attr) == 3, "QObj attr offset must match kdb+ K object");
static_assert(offsetof(QObj, refs) == 4, "QObj refs offset must match kdb+ K object");
static_assert(offsetof(QObj, length) == 8, "QObj length offset must match kdb+ K object");
static_assert(offsetof(QObj, list) == 16, "QObj list offset must match kdb+ K object");

#endif // QLIB_OBJ_H
