
/**
 * @file sym.h
 * @brief Symbol interning.
 */

#ifndef QLIB_SYM_H
#define QLIB_SYM_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Intern a null-terminated string as a symbol.
 *
 * Strings with the same characters intern to the same pointer, so interned symbols can be compared
 * with ==. The returned symbol is owned by kdb+ and lives for the rest of the process: it must not
 * be modified or freed.
 *
 * @param string Null-terminated string to intern.
 * @return The interned symbol.
 */
static inline QSymbol qInternString(const QChar *string) {
    extern QSymbol ss(const QChar *);
    assert(string);
    return ss(string);
}

/**
 * @brief Intern the first length characters of a string as a symbol.
 *
 * Reads at most length characters, stopping early at a null character (kdb+'s sn would otherwise
 * intern the null and the characters after it, giving a symbol that prints like the prefix but is
 * not equal to it). See qInternString for how interned symbols behave.
 *
 * @param string String to intern (need not be null-terminated). May be NULL if length is 0.
 * @param length Maximum number of characters to intern (must not exceed INT32_MAX).
 * @return The interned symbol.
 */
static inline QSymbol qInternFixedString(const QChar *string, QSize length) {
    extern QSymbol sn(const QChar *, QInt);
    assert((string || length == 0) && length <= INT32_MAX);
    if (length > 0) {
        const QChar *nul = (const QChar *)memchr(string, '\0', length);
        if (nul)
            length = (QSize)(nul - string);
    }
    return sn(string, (QInt)length);
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_SYM_H
