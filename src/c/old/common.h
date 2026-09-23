
/**
 * @file common.h
 * @brief Common definitions.
 */

#ifndef QLIB_COMMON_H
#define QLIB_COMMON_H

typedef unsigned long SZ;

static inline SZ saturated_sub(SZ x, SZ y) { return y >= x ? 0 : x - y; }

// use "-DBUFFER_SIZE=2048" to set when compiling
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096 
#endif

#define PRINTF_ERROR -1
#define BUFFER_FULL_ERROR -2

#endif // QLIB_COMMON_H
