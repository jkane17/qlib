
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
