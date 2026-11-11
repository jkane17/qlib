#ifndef QLIB_INTERNAL_FMT_COUNT_H
#define QLIB_INTERNAL_FMT_COUNT_H

#include <limits.h>

#include "../type.h"

// Keep the public int return value representable. Failure leaves the count unchanged.
static inline bool tryAddFormattedCount(QSize *count, QSize additional) {
    if (*count > (QSize)INT_MAX || additional > (QSize)INT_MAX - *count)
        return false;
    *count += additional;
    return true;
}

#endif // QLIB_INTERNAL_FMT_COUNT_H
