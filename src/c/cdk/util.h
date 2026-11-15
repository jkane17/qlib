
/**
 * @file util.h
 * @brief Uncategorised utilities.
 */

#ifndef QLIB_UTIL_H
#define QLIB_UTIL_H

#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the storage size in bytes of one primitive value.
 *
 * @param type Positive primitive type code (for both atoms and list elements).
 * @return The size of the corresponding C type, or 0 for unsupported codes,
 * including negative atom codes and mixed lists. For symbols this is the size
 * of a symbol pointer, not the length of its text. Object headers are excluded.
 */
static inline QSize qTypeSize(QTypeCode type) {
    switch (type) {
        case Q_TYPE_BOOLEAN:
            return sizeof(QBoolean);
        case Q_TYPE_GUID:
            return sizeof(QGuid);
        case Q_TYPE_BYTE:
            return sizeof(QByte);
        case Q_TYPE_SHORT:
            return sizeof(QShort);
        case Q_TYPE_INT:
            return sizeof(QInt);
        case Q_TYPE_LONG:
            return sizeof(QLong);
        case Q_TYPE_REAL:
            return sizeof(QReal);
        case Q_TYPE_FLOAT:
            return sizeof(QFloat);
        case Q_TYPE_CHAR:
            return sizeof(QChar);
        case Q_TYPE_SYMBOL:
            return sizeof(QSymbol);
        case Q_TYPE_TIMESTAMP:
            return sizeof(QTimestamp);
        case Q_TYPE_MONTH:
            return sizeof(QMonth);
        case Q_TYPE_DATE:
            return sizeof(QDate);
        case Q_TYPE_DATETIME:
            return sizeof(QDatetime);
        case Q_TYPE_TIMESPAN:
            return sizeof(QTimespan);
        case Q_TYPE_MINUTE:
            return sizeof(QMinute);
        case Q_TYPE_SECOND:
            return sizeof(QSecond);
        case Q_TYPE_TIME:
            return sizeof(QTime);
        default:
            return 0;
    }
}

/**
 * @brief Computes a - b, saturating at numeric bounds.
 * 
 * @param a The minuend.
 * @param b The subtrahend.
 * @return QSize The result of a - b, or 0 if b >= a.
 */
static inline QSize saturatedSub(QSize a, QSize b) { return b >= a ? 0 : a - b; }

#ifdef __cplusplus
}
#endif

#endif // QLIB_UTIL_H
