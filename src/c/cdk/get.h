
/**
 * @file get.h
 * @brief Q object accessors.
 */

#ifndef QLIB_GET_H
#define QLIB_GET_H

#include <assert.h>

#include "obj.h"
#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

///// Atom Accessors /////

/**
 * @brief Extract the underlying boolean atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying boolean atom.
 */
static inline QBoolean qGetBoolean(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_BOOLEAN);
    return (QBoolean)obj->qbyte;
}

/**
 * @brief Extract the underlying guid atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to the underlying guid atom.
 */
static inline const QGuid *qGetGuid(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_GUID);
    return (const QGuid *)obj->list;
}

/**
 * @brief Extract the underlying byte atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying byte atom.
 */
static inline QByte qGetByte(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_BYTE);
    return (QByte)obj->qbyte;
}

/**
 * @brief Extract the underlying short atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying short atom.
 */
static inline QShort qGetShort(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_SHORT);
    return (QShort)obj->qshort;
}

/**
 * @brief Extract the underlying int atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying int atom.
 */
static inline QInt qGetInt(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_INT);
    return obj->qint;
}

/**
 * @brief Extract the underlying long atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying long atom.
 */
static inline QLong qGetLong(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_LONG);
    return obj->qlong;
}

/**
 * @brief Extract the underlying real atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying real atom.
 */
static inline QReal qGetReal(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_REAL);
    return obj->qreal;
}

/**
 * @brief Extract the underlying float atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying float atom.
 */
static inline QFloat qGetFloat(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_FLOAT);
    return obj->qfloat;
}

/**
 * @brief Extract the underlying char atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying char atom.
 */
static inline QChar qGetChar(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_CHAR);
    return (QChar)obj->qbyte;
}

/**
 * @brief Extract the underlying symbol atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying symbol atom.
 */
static inline QSymbol qGetSymbol(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_SYMBOL);
    return obj->qsymbol;
}

/**
 * @brief Extract the underlying timestamp atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying timestamp atom.
 */
static inline QTimestamp qGetTimestamp(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_TIMESTAMP);
    return (QTimestamp)obj->qlong;
}

/**
 * @brief Extract the underlying month atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying month atom.
 */
static inline QMonth qGetMonth(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_MONTH);
    return (QMonth)obj->qint;
}

/**
 * @brief Extract the underlying date atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying date atom.
 */
static inline QDate qGetDate(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_DATE);
    return (QDate)obj->qint;
}

/**
 * @brief Extract the underlying datetime atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying datetime atom.
 */
static inline QDatetime qGetDatetime(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_DATETIME);
    return (QDatetime)obj->qfloat;
}

/**
 * @brief Extract the underlying timespan atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying timespan atom.
 */
static inline QTimespan qGetTimespan(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_TIMESPAN);
    return (QTimespan)obj->qlong;
}

/**
 * @brief Extract the underlying minute atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying minute atom.
 */
static inline QMinute qGetMinute(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_MINUTE);
    return (QMinute)obj->qint;
}

/**
 * @brief Extract the underlying second atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying second atom.
 */
static inline QSecond qGetSecond(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_SECOND);
    return (QSecond)obj->qint;
}

/**
 * @brief Extract the underlying time atom from a Q object.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying time atom.
 */
static inline QTime qGetTime(const QObj *obj) {
    assert(obj && obj->type == -Q_TYPE_TIME);
    return (QTime)obj->qint;
}

///// List Accessors /////

/**
 * @brief Extract a list item from a Q object containing a boolean list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The boolean list item.
 */
static inline QBoolean qGetBooleanAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_BOOLEAN && index < obj->length);
    return ((const QBoolean *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a guid list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return A pointer to the guid list item.
 */
static inline const QGuid *qGetGuidAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_GUID && index < obj->length);
    return &((const QGuid *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a byte list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The byte list item.
 */
static inline QByte qGetByteAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_BYTE && index < obj->length);
    return ((const QByte *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a short list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The short list item.
 */
static inline QShort qGetShortAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_SHORT && index < obj->length);
    return ((const QShort *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing an int list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The int list item.
 */
static inline QInt qGetIntAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_INT && index < obj->length);
    return ((const QInt *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a long list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The long list item.
 */
static inline QLong qGetLongAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_LONG && index < obj->length);
    return ((const QLong *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a real list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The real list item.
 */
static inline QReal qGetRealAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_REAL && index < obj->length);
    return ((const QReal *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a float list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The float list item.
 */
static inline QFloat qGetFloatAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_FLOAT && index < obj->length);
    return ((const QFloat *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a char list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The char list item.
 */
static inline QChar qGetCharAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_CHAR && index < obj->length);
    return ((const QChar *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a symbol list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The symbol list item.
 */
static inline QSymbol qGetSymbolAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_SYMBOL && index < obj->length);
    return ((const QSymbol *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a timestamp list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The timestamp list item.
 */
static inline QTimestamp qGetTimestampAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_TIMESTAMP && index < obj->length);
    return ((const QTimestamp *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a month list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The month list item.
 */
static inline QMonth qGetMonthAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_MONTH && index < obj->length);
    return ((const QMonth *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a date list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The date list item.
 */
static inline QDate qGetDateAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_DATE && index < obj->length);
    return ((const QDate *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a datetime list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The datetime list item.
 */
static inline QDatetime qGetDatetimeAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_DATETIME && index < obj->length);
    return ((const QDatetime *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a timespan list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The timespan list item.
 */
static inline QTimespan qGetTimespanAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_TIMESPAN && index < obj->length);
    return ((const QTimespan *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a minute list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The minute list item.
 */
static inline QMinute qGetMinuteAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_MINUTE && index < obj->length);
    return ((const QMinute *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a second list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The second list item.
 */
static inline QSecond qGetSecondAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_SECOND && index < obj->length);
    return ((const QSecond *)obj->list)[index];
}

/**
 * @brief Extract a list item from a Q object containing a time list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return The time list item.
 */
static inline QTime qGetTimeAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_TIME && index < obj->length);
    return ((const QTime *)obj->list)[index];
}

///// Complex Accessors /////

static inline QSize qGetCount(const QObj *obj);

/**
 * @brief Extract a list item from a Q object containing a mixed list.
 *
 * @param obj A pointer to a Q object to extract from.
 * @param index Index of the item to extract.
 * @return A pointer to the Q object at the given index.
 */
static inline QObj *qGetMixedAtIndex(const QObj *obj, QSize index) {
    assert(obj && obj->type == Q_TYPE_MIXED && index < obj->length);
    return ((QObj *const *)obj->list)[index];
}

/**
 * @brief Extract the keys of a Q object containing a dictionary.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to a Q object containing the dictionary keys.
 */
static inline QObj *qGetDictKeys(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_DICTIONARY);
    return ((QObj *const *)obj->list)[0];
}

/**
 * @brief Extract the values of a Q object containing a dictionary.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to a Q object containing the dictionary values.
 */
static inline QObj *qGetDictValues(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_DICTIONARY);
    return ((QObj *const *)obj->list)[1];
}

/**
 * @brief Extract the number of key-value pairs from a Q object containing a dictionary.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The number of key-value pairs in the dictionary.
 */
static inline QSize qGetDictCount(const QObj *obj) {
    return qGetCount(qGetDictKeys(obj));
}

/**
 * @brief Extract the header (list of column names) of a Q object containing a table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to a Q object containing the header (a list of the column names).
 */
static inline QObj *qGetTableHeader(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_TABLE);
    return ((QObj *const *)obj->nested->list)[0];
}

/**
 * @brief Extract a list of Q objects each containing a single column of a table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return An array of Q object pointers each containing a single column of a table.
 */
static inline QObj *qGetTableColumns(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_TABLE);
    return ((QObj *const *)obj->nested->list)[1];
}

/**
 * @brief Extract the number of columns from a Q object containing a table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The number of columns in the table.
 */
static inline QSize qGetTableColumnCount(const QObj *obj) {
    return qGetTableHeader(obj)->length;
}

/**
 * @brief Extract the number of rows from a Q object containing a table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The number of rows in the table.
 */
static inline QSize qGetTableRowCount(const QObj *obj) {
    const QObj *columns = qGetTableColumns(obj);
    return columns->length ? qGetCount(((QObj *const *)columns->list)[0]) : 0;
}

/**
 * @brief Extract the keys of a Q object containing a keyed table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to a Q object containing a table of the keys.
 */
static inline QObj *qGetKeyedTableKeys(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_DICTIONARY);
    return ((QObj *const *)obj->list)[0];
}

/**
 * @brief Extract the values of a Q object containing a keyed table.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return A pointer to a Q object containing a table of the values.
 */
static inline QObj *qGetKeyedTableValues(const QObj *obj) {
    assert(obj && obj->type == Q_TYPE_DICTIONARY);
    return ((QObj *const *)obj->list)[1];
}

///// Generic Accessors /////

/**
 * @brief Extract the number of items in a Q object (equivalent to Q's `count`).
 *
 * @param obj A pointer to a Q object to count.
 * @return The number of rows for a table or keyed table, the number of key-value pairs for a
 * dictionary, the number of elements for any other list, 1 for an atom or function, and 0 for a
 * null pointer.
 */
static inline QSize qGetCount(const QObj *obj) {
    if (!obj)
        return 0;
    if (obj->type < 0 || obj->type > Q_TYPE_DICTIONARY)
        return 1;
    if (obj->type == Q_TYPE_TABLE)
        return qGetTableRowCount(obj);
    if (obj->type == Q_TYPE_DICTIONARY)
        return qGetDictCount(obj);
    return obj->length;
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_GET_H
