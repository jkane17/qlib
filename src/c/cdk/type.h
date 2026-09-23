
/**
 * @file type.h
 * @brief Q type definitions and type codes.
 *
 * Defines the C representations of Q's primitive types, Q type codes, list attributes, sizes,
 * and special null and infinity values.
 */

#ifndef QLIB_TYPE_H
#define QLIB_TYPE_H

#include <math.h>
#include <stdint.h>

// Q boolean value stored as a single unsigned byte.
typedef unsigned char QBoolean;

// Q GUID value consisting of 16 bytes (128 bits).
typedef struct {
    unsigned char bytes[16];
} QGuid;

// Q byte value stored as a single unsigned byte.
typedef unsigned char QByte;

// Q short value representing a signed 16-bit integer.
typedef int16_t QShort;

// Q int value representing a signed 32-bit integer.
typedef int32_t QInt;

// Q long value representing a signed 64-bit integer.
typedef int64_t QLong;

// Q real value representing a 32-bit floating-point value.
typedef float QReal;

// Q float value representing a 64-bit floating-point value.
typedef double QFloat;

// Q char value stored as a single character.
typedef char QChar;

// Q symbol value stored as a pointer to the symbol's character data.
typedef char *QSymbol;

// Q timestamp value stored as a signed 64-bit integer.
typedef int64_t QTimestamp;

// Q month value stored as a signed 32-bit integer.
typedef int32_t QMonth;

// Q date value stored as a signed 32-bit integer.
typedef int32_t QDate;

// Q datetime value stored as a 64-bit floating-point value.
typedef double QDatetime;

// Q timespan value stored as a signed 64-bit integer.
typedef int64_t QTimespan;

// Q minute value stored as a signed 32-bit integer.
typedef int32_t QMinute;

// Q second value stored as a signed 32-bit integer.
typedef int32_t QSecond;

// Q time value stored as a signed 32-bit integer.
typedef int32_t QTime;

/**
 * @brief Q type code.
 *
 * Q uses the sign of a type code to distinguish atoms from lists:
 *
 * - Negative values represent atoms.
 *
 * - Zero and positive values represent lists.
 */
typedef signed char QType;

// Q list attribute flags (sorted, unique, parted, grouped).
typedef char QAttr;

// Q object size or list length stored as an unsigned 64-bit integer.
typedef uint64_t QSize;

// Maximum valid QSize (kdb+ stores lengths as signed 64-bit integers)
#define Q_SIZE_MAX ((QSize)INT64_MAX)

///// Special Values /////

// Temporal types use the null and infinity values of their underlying type:
//   timestamp, timespan                     -> Q_LONG_*
//   month, date, minute, second, time       -> Q_INT_*
//   datetime                                -> Q_FLOAT_*

// QShort null value (0Nh)
#define Q_SHORT_NULL ((QShort)INT16_MIN)

// QShort infinity value (0Wh)
#define Q_SHORT_INF ((QShort)INT16_MAX)

// QShort negative infinity value (-0Wh)
#define Q_SHORT_NINF ((QShort)-INT16_MAX)

// QInt null value (0Ni)
#define Q_INT_NULL ((QInt)INT32_MIN)

// QInt infinity value (0Wi)
#define Q_INT_INF ((QInt)INT32_MAX)

// QInt negative infinity value (-0Wi)
#define Q_INT_NINF ((QInt)-INT32_MAX)

// QLong null value (0N)
#define Q_LONG_NULL ((QLong)INT64_MIN)

// QLong infinity value (0W)
#define Q_LONG_INF ((QLong)INT64_MAX)

// QLong negative infinity value (-0W)
#define Q_LONG_NINF ((QLong)-INT64_MAX)

// QReal null value (0Ne). Any NaN is treated as null, so test with isnan() rather than ==.
#define Q_REAL_NULL ((QReal)NAN)

// QReal infinity value (0We)
#define Q_REAL_INF ((QReal)INFINITY)

// QReal negative infinity value (-0We)
#define Q_REAL_NINF ((QReal)-INFINITY)

// QFloat null value (0n). Any NaN is treated as null, so test with isnan() rather than ==.
#define Q_FLOAT_NULL ((QFloat)NAN)

// QFloat infinity value (0w)
#define Q_FLOAT_INF ((QFloat)INFINITY)

// QFloat negative infinity value (-0w)
#define Q_FLOAT_NINF ((QFloat)-INFINITY)

// QChar null value (" ")
#define Q_CHAR_NULL ((QChar)' ')

// QSymbol null value (`). Symbols are interned, so test for null with sym[0] == '\0' rather than
// by comparing pointers.
#define Q_SYMBOL_NULL ""

// QGuid null value (0Ng), all bytes zero.
static constexpr QGuid Q_GUID_NULL = {{0}};

///// Attributes /////

// Q list attribute codes (requires C23).
typedef enum : QAttr {
    Q_ATTR_NONE = 0,    // no attribute
    Q_ATTR_SORTED = 1,  // `s#
    Q_ATTR_UNIQUE = 2,  // `u#
    Q_ATTR_PARTED = 3,  // `p#
    Q_ATTR_GROUPED = 4, // `g#
} QAttrCode;

///// Type Codes /////

// Q type codes (requires C23).
typedef enum : QType {
    Q_TYPE_ERROR = -128, // error (atom)

    Q_TYPE_MIXED = 0,
    Q_TYPE_BOOLEAN = 1,
    Q_TYPE_GUID = 2,
    Q_TYPE_BYTE = 4,
    Q_TYPE_SHORT = 5,
    Q_TYPE_INT = 6,
    Q_TYPE_LONG = 7,
    Q_TYPE_REAL = 8,
    Q_TYPE_FLOAT = 9,
    Q_TYPE_CHAR = 10,
    Q_TYPE_SYMBOL = 11,
    Q_TYPE_TIMESTAMP = 12,
    Q_TYPE_MONTH = 13,
    Q_TYPE_DATE = 14,
    Q_TYPE_DATETIME = 15,
    Q_TYPE_TIMESPAN = 16,
    Q_TYPE_MINUTE = 17,
    Q_TYPE_SECOND = 18,
    Q_TYPE_TIME = 19,

    Q_TYPE_ENUM_FIRST = 20, // enumerated list (first type code)
    Q_TYPE_ENUM_LAST = 76,  // enumerated list (last type code)

    Q_TYPE_ANYMAP = 77,       // mapped list of lists of any type
    Q_TYPE_NESTED_FIRST = 78, // mapped nested list, 77 + primitive type (first type code)
    Q_TYPE_NESTED_LAST = 96,  // mapped nested list, 77 + primitive type (last type code)

    Q_TYPE_TABLE = 98,
    Q_TYPE_DICTIONARY = 99,

    Q_TYPE_LAMBDA = 100,           // lambda
    Q_TYPE_UNARY_PRIMITIVE = 101,  // unary primitive
    Q_TYPE_OPERATOR = 102,         // operator (binary primitive)
    Q_TYPE_ITERATOR = 103,         // iterator
    Q_TYPE_PROJECTION = 104,       // projection
    Q_TYPE_COMPOSITION = 105,      // composition
    Q_TYPE_EACH = 106,             // f'
    Q_TYPE_OVER = 107,             // f/
    Q_TYPE_SCAN = 108,             // f\ (scan)
    Q_TYPE_EACH_PRIOR = 109,       // f':
    Q_TYPE_EACH_RIGHT = 110,       // f/:
    Q_TYPE_EACH_LEFT = 111,        // f\:
    Q_TYPE_DYNAMIC_LOAD = 112,     // dynamic load (foreign function)
} QTypeCode;

#endif // QLIB_TYPE_H
