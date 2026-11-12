
/**
 * @file list.h
 * @brief List operations such as appending.
 */

#ifndef QLIB_LIST_H
#define QLIB_LIST_H

#include <assert.h>
#include <stdint.h>

#include "is.h"
#include "obj.h"
#include "sym.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Append a single raw value to a Q object containing a simple list.
 *
 * The list is modified in place, so it must not be shared (its `refs` must be 0).
 *
 * @param list The address of a pointer to a Q object containing a simple list to append to. Updated
 * in place if the list is reallocated.
 * @param value A pointer to the raw value to append. It must point to a value of the list's element
 * type (for example, a `QLong` for a long list). For a symbol list it must point to an interned
 * symbol: prefer `qAppendSymbol`, which interns for you.
 * @return A pointer to the (possibly reallocated) updated Q object, the same as the new `*list`.
 */
static inline QObj *qAppendValue(QObj **list, void *value) {
    extern QObj *ja(QObj **, void *);
    assert(list && qIsSimpleList(*list) && value);
    return ja(list, value);
}

/**
 * @brief Append a Q object to a Q object containing a mixed list.
 *
 * The list is modified in place, so it must not be shared (its `refs` must be 0).
 *
 * @param list The address of a pointer to a Q object containing a mixed list to append to. Updated
 * in place if the list is reallocated.
 * @param obj A pointer to the Q object to append (takes ownership). Must not be the list itself.
 * @return A pointer to the (possibly reallocated) updated Q object, the same as the new `*list`.
 */
static inline QObj *qAppendObject(QObj **list, QObj *obj) {
    extern QObj *jk(QObj **, QObj *);
    assert(list && qIsMixedList(*list) && obj && obj != *list);
    return jk(list, obj);
}

/**
 * @brief Intern a null-terminated string and append it to a Q object containing a symbol list.
 *
 * The string is interned before it is appended (kdb+'s js does not intern), so any string can be
 * passed. Interning an already interned symbol returns the same symbol, so passing a `QSymbol` is
 * also fine.
 *
 * The list is modified in place, so it must not be shared (its `refs` must be 0).
 *
 * @param list The address of a pointer to a Q object containing a symbol list to append to. Updated
 * in place if the list is reallocated.
 * @param string Null-terminated string to intern and append.
 * @return A pointer to the (possibly reallocated) updated Q object, the same as the new `*list`.
 */
static inline QObj *qAppendSymbol(QObj **list, const QChar *string) {
    extern QObj *js(QObj **, QSymbol);
    assert(list && qIsSymbolList(*list) && string);
    return js(list, qInternString(string));
}

/**
 * @brief Intern the first length characters of a string and append them to a Q object containing a
 * symbol list.
 *
 * Reads at most length characters, stopping early at a null character (see qInternFixedString). As
 * with qAppendSymbol, the string is interned before it is appended.
 *
 * The list is modified in place, so it must not be shared (its `refs` must be 0).
 *
 * @param list The address of a pointer to a Q object containing a symbol list to append to. Updated
 * in place if the list is reallocated.
 * @param string String to intern and append (need not be null-terminated). May be NULL if length is
 * 0.
 * @param length Maximum number of characters to intern (must not exceed INT32_MAX).
 * @return A pointer to the (possibly reallocated) updated Q object, the same as the new `*list`.
 */
static inline QObj *qAppendFixedSymbol(QObj **list, const QChar *string, QSize length) {
    extern QObj *js(QObj **, QSymbol);
    assert(list && qIsSymbolList(*list) && (string || length == 0) && length <= INT32_MAX);
    return js(list, qInternFixedString(string, length));
}

/**
 * @brief Append the items of one list to another list of the same type.
 *
 * Both lists must be simple lists of the same type, or both mixed lists. The left list is modified
 * in place, so it must not be shared (its `refs` must be 0).
 *
 * @param left The address of a pointer to a Q object containing a list to append to. Updated in
 * place if the list is reallocated.
 * @param right A pointer to a Q object containing the list whose items are appended (borrowed: the
 * caller still owns it and must release it). Must not be the left list itself.
 * @return A pointer to the (possibly reallocated) updated Q object, the same as the new `*left`.
 */
static inline QObj *qAppendList(QObj **left, QObj *right) {
    extern QObj *jv(QObj **, QObj *);
    assert(
        left && (qIsSimpleList(*left) || qIsMixedList(*left)) && right && right != *left &&
        right->type == (*left)->type
    );
    return jv(left, right);
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_LIST_H
