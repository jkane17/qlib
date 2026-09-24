
/**
 * @file mem.h
 * @brief Memory and reference management.
 */

#ifndef QLIB_MEM_H
#define QLIB_MEM_H

#include "obj.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Decrement the reference count of a Q object (freed if count < 0).
 *
 * @param obj A pointer to a Q object to decrement.
 */
static inline void qDecRef(QObj *obj) {
    extern void r0(QObj *);
    r0(obj);
}

/**
 * @brief Increment the reference count of a Q object.
 *
 * @param obj A pointer to a Q object to increment.
 */
static inline void qIncRef(QObj *obj) {
    extern void r1(QObj *);
    r1(obj);
}

/**
 * @brief Release the memory allocated for a thread’s pool. Call when the thread is about to
 * complete.
 */
static inline void qFreePool() {
    extern void m9();
    m9();
}

/**
 * @brief Set whether interning symbols uses a lock (thread safety).
 *
 * @param mode 1 to enable the lock, 0 to disable.
 */
static inline void qToggleSymbolLock(QInt mode) {
    extern QInt setm(QInt);
    setm(mode);
}

/**
 * @brief Retrieve memory statistics.
 *
 * @param mode
 *      0   Memory usage for current thread (3 longs):
 *          0   The number of bytes allocated.
 *          1   Bytes available in the heap.
 *          2   Maximum heap size so far.
 *      1   Symbol statistics (2 longs):
 *          0   The number of internalised symbols (null if this is not the main thread).
 *          1   The corresponding symbol memory usage (null if this is not the main thread).
 * @return A pointer to a Q object contain the memory statistic as a 3 or 2 item list of longs.
 */
static inline QObj *qMemoryStats(QInt mode) {
    extern QObj *m4(QInt);
    return m4(mode);
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_MEM_H
