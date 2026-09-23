
/**
 * @file is.h
 * @brief Q object underlying type checking.
 */

#ifndef QLIB_IS_H
#define QLIB_IS_H

#include "get.h"
#include "obj.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

///// Atom Type Checking /////

/**
 * @brief Check if the underlying value of a Q object is an atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is an atom, false otherwise.
 */
static inline bool qIsAtom(const QObj *obj) {
    return obj && obj->type < 0;
}

/**
 * @brief Check if the underlying value of a Q object is an atom of the given type.
 *
 * @param obj A pointer to a Q object to check.
 * @param type Type code to check for.
 * @return true if the underlying value is an atom of the given type, false otherwise.
 */
static inline bool qIsAtomType(const QObj *obj, QTypeCode type) {
    return obj && obj->type == -type;
}

/**
 * @brief Check if the underlying value of a Q object is a boolean atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a boolean atom, false otherwise.
 */
static inline bool qIsBoolean(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_BOOLEAN);
}

/**
 * @brief Check if the underlying value of a Q object is a guid atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a guid atom, false otherwise.
 */
static inline bool qIsGuid(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_GUID);
}

/**
 * @brief Check if the underlying value of a Q object is a byte atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a byte atom, false otherwise.
 */
static inline bool qIsByte(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_BYTE);
}

/**
 * @brief Check if the underlying value of a Q object is a short atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a short atom, false otherwise.
 */
static inline bool qIsShort(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_SHORT);
}

/**
 * @brief Check if the underlying value of a Q object is an int atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is an int atom, false otherwise.
 */
static inline bool qIsInt(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_INT);
}

/**
 * @brief Check if the underlying value of a Q object is a long atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a long atom, false otherwise.
 */
static inline bool qIsLong(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_LONG);
}

/**
 * @brief Check if the underlying value of a Q object is a real atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a real atom, false otherwise.
 */
static inline bool qIsReal(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_REAL);
}

/**
 * @brief Check if the underlying value of a Q object is a float atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a float atom, false otherwise.
 */
static inline bool qIsFloat(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_FLOAT);
}

/**
 * @brief Check if the underlying value of a Q object is a char atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a char atom, false otherwise.
 */
static inline bool qIsChar(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_CHAR);
}

/**
 * @brief Check if the underlying value of a Q object is a symbol atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a symbol atom, false otherwise.
 */
static inline bool qIsSymbol(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_SYMBOL);
}

/**
 * @brief Check if the underlying value of a Q object is a timestamp atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a timestamp atom, false otherwise.
 */
static inline bool qIsTimestamp(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_TIMESTAMP);
}

/**
 * @brief Check if the underlying value of a Q object is a month atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a month atom, false otherwise.
 */
static inline bool qIsMonth(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_MONTH);
}

/**
 * @brief Check if the underlying value of a Q object is a date atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a date atom, false otherwise.
 */
static inline bool qIsDate(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_DATE);
}

/**
 * @brief Check if the underlying value of a Q object is a datetime atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a datetime atom, false otherwise.
 */
static inline bool qIsDatetime(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_DATETIME);
}

/**
 * @brief Check if the underlying value of a Q object is a timespan atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a timespan atom, false otherwise.
 */
static inline bool qIsTimespan(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_TIMESPAN);
}

/**
 * @brief Check if the underlying value of a Q object is a minute atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a minute atom, false otherwise.
 */
static inline bool qIsMinute(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_MINUTE);
}

/**
 * @brief Check if the underlying value of a Q object is a second atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a second atom, false otherwise.
 */
static inline bool qIsSecond(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_SECOND);
}

/**
 * @brief Check if the underlying value of a Q object is a time atom.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a time atom, false otherwise.
 */
static inline bool qIsTime(const QObj *obj) {
    return qIsAtomType(obj, Q_TYPE_TIME);
}

///// List Type Checking /////

/**
 * @brief Check if the underlying value of a Q object is a list of any kind.
 *
 * Includes mixed lists (0), simple lists (1-19), enumerated lists (20-76), and mapped/nested lists
 * (77-97). Excludes tables (98) and dictionaries (99).
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a list, false otherwise.
 */
static inline bool qIsList(const QObj *obj) {
    return obj && Q_TYPE_MIXED <= obj->type && obj->type < Q_TYPE_TABLE;
}

/**
 * @brief Check if the underlying value of a Q object is a simple list (a homogeneous list of one of
 * the primitive types 1-19).
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a simple list, false otherwise.
 */
static inline bool qIsSimpleList(const QObj *obj) {
    // Type code 3 is unused by Q
    return obj && Q_TYPE_BOOLEAN <= obj->type && obj->type <= Q_TYPE_TIME && obj->type != 3;
}

/**
 * @brief Check if the underlying value of a Q object is a list of the given type.
 *
 * @param obj A pointer to a Q object to check.
 * @param type List type code to check for (0-97, e.g. Q_TYPE_MIXED or Q_TYPE_LONG).
 * @return true if the underlying value is a list of the given type, false otherwise (including
 * when type is not a list type code).
 */
static inline bool qIsListType(const QObj *obj, QTypeCode type) {
    return obj && Q_TYPE_MIXED <= type && type < Q_TYPE_TABLE && obj->type == type;
}

/**
 * @brief Check if the underlying value of a Q object is a boolean list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a boolean list, false otherwise.
 */
static inline bool qIsBooleanList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_BOOLEAN);
}

/**
 * @brief Check if the underlying value of a Q object is a guid list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a guid list, false otherwise.
 */
static inline bool qIsGuidList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_GUID);
}

/**
 * @brief Check if the underlying value of a Q object is a byte list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a byte list, false otherwise.
 */
static inline bool qIsByteList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_BYTE);
}

/**
 * @brief Check if the underlying value of a Q object is a short list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a short list, false otherwise.
 */
static inline bool qIsShortList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_SHORT);
}

/**
 * @brief Check if the underlying value of a Q object is an int list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is an int list, false otherwise.
 */
static inline bool qIsIntList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_INT);
}

/**
 * @brief Check if the underlying value of a Q object is a long list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a long list, false otherwise.
 */
static inline bool qIsLongList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_LONG);
}

/**
 * @brief Check if the underlying value of a Q object is a real list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a real list, false otherwise.
 */
static inline bool qIsRealList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_REAL);
}

/**
 * @brief Check if the underlying value of a Q object is a float list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a float list, false otherwise.
 */
static inline bool qIsFloatList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_FLOAT);
}

/**
 * @brief Check if the underlying value of a Q object is a char list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a char list, false otherwise.
 */
static inline bool qIsCharList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_CHAR);
}

/**
 * @brief Check if the underlying value of a Q object is a symbol list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a symbol list, false otherwise.
 */
static inline bool qIsSymbolList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_SYMBOL);
}

/**
 * @brief Check if the underlying value of a Q object is a timestamp list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a timestamp list, false otherwise.
 */
static inline bool qIsTimestampList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_TIMESTAMP);
}

/**
 * @brief Check if the underlying value of a Q object is a month list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a month list, false otherwise.
 */
static inline bool qIsMonthList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_MONTH);
}

/**
 * @brief Check if the underlying value of a Q object is a date list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a date list, false otherwise.
 */
static inline bool qIsDateList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_DATE);
}

/**
 * @brief Check if the underlying value of a Q object is a datetime list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a datetime list, false otherwise.
 */
static inline bool qIsDatetimeList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_DATETIME);
}

/**
 * @brief Check if the underlying value of a Q object is a timespan list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a timespan list, false otherwise.
 */
static inline bool qIsTimespanList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_TIMESPAN);
}

/**
 * @brief Check if the underlying value of a Q object is a minute list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a minute list, false otherwise.
 */
static inline bool qIsMinuteList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_MINUTE);
}

/**
 * @brief Check if the underlying value of a Q object is a second list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a second list, false otherwise.
 */
static inline bool qIsSecondList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_SECOND);
}

/**
 * @brief Check if the underlying value of a Q object is a time list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a time list, false otherwise.
 */
static inline bool qIsTimeList(const QObj *obj) {
    return qIsListType(obj, Q_TYPE_TIME);
}

///// Complex Type Checking /////

/**
 * @brief Check if the underlying value of a Q object is a mixed list.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a mixed list, false otherwise.
 */
static inline bool qIsMixedList(const QObj *obj) {
    return obj && obj->type == Q_TYPE_MIXED;
}

/**
 * @brief Check if the underlying value of a Q object is a simple table.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a simple table, false otherwise.
 */
static inline bool qIsTable(const QObj *obj) {
    return obj && obj->type == Q_TYPE_TABLE;
}

/**
 * @brief Check if the underlying value of a Q object is a keyed table.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a keyed table, false otherwise.
 */
static inline bool qIsKeyedTable(const QObj *obj) {
    return obj && obj->type == Q_TYPE_DICTIONARY && qIsTable(qGetDictKeys(obj)) &&
           qIsTable(qGetDictValues(obj));
}

/**
 * @brief Check if the underlying value of a Q object is a dictionary.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is a dictionary, false otherwise.
 */
static inline bool qIsDict(const QObj *obj) {
    return obj && obj->type == Q_TYPE_DICTIONARY && !qIsKeyedTable(obj);
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_IS_H
