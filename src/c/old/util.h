
/**
 * @file util.h
 * @brief Uncategorised utilities.
 */

#ifndef QLIB_UTIL_H
#define QLIB_UTIL_H

#include "common.h"
#include "k.h"

/**
 * @brief Access the element at a given index within a K object.
 * 
 * @note Simply returns the object if it is not an indexable type.
 * 
 * @param x K object to index.
 * @param i Index to access.
 * @return The element at the given index.
 */
V *k_index(K x, SZ i);

/**
 * @brief Get a pointer to the underlying value of a K object.
 * 
 * @param x K object to get value of.
 * @return A pointer to the underlying value.
 */
static inline V *k_value(K x) { return xt >= 0 || xt == -UU ? xG : &xg; }

#endif // QLIB_UTIL_H
