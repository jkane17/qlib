
/**
 * @file new.h
 * @brief Q object creators.
 */

#ifndef QLIB_NEW_H
#define QLIB_NEW_H

#include <stdint.h>

#include "err.h"
#include "get.h"
#include "is.h"
#include "mem.h"
#include "obj.h"
#include "type.h"

#ifdef __cplusplus
#include <cstdarg>
extern "C" {
#else
#include <stdarg.h>
#endif

///// Atom Creation /////

/**
 * @brief Create a Q object containing a boolean atom.
 *
 * @param value Value to create the boolean atom from (zero is false, non-zero is true).
 * @return A pointer to a Q object containing a boolean atom.
 */
static inline QObj *qNewBoolean(QBoolean value) {
    extern QObj *kb(QInt);
    return kb(value != 0);
}

/**
 * @brief Create a Q object containing a guid atom.
 *
 * @param value Raw value to create the guid atom from.
 * @return A pointer to a Q object containing a guid atom.
 */
static inline QObj *qNewGuid(QGuid value) {
    extern QObj *ku(QGuid);
    return ku(value);
}

/**
 * @brief Create a Q object containing a byte atom.
 *
 * @param value Value to create the byte atom from.
 * @return A pointer to a Q object containing a byte atom.
 */
static inline QObj *qNewByte(QByte value) {
    extern QObj *kg(QInt);
    return kg((QInt)value);
}

/**
 * @brief Create a Q object containing a short atom.
 *
 * @param value Value to create the short atom from.
 * @return A pointer to a Q object containing a short atom.
 */
static inline QObj *qNewShort(QShort value) {
    extern QObj *kh(QInt);
    return kh((QInt)value);
}

/**
 * @brief Create a Q object containing an int atom.
 *
 * @param value Value to create the int atom from.
 * @return Q object containing an int atom.
 */
static inline QObj *qNewInt(QInt value) {
    extern QObj *ki(QInt);
    return ki(value);
}

/**
 * @brief Create a Q object containing a long atom.
 *
 * @param value Value to create the long atom from.
 * @return A pointer to a Q object containing a long atom.
 */
static inline QObj *qNewLong(QLong value) {
    extern QObj *kj(QLong);
    return kj(value);
}

/**
 * @brief Create a Q object containing a real atom.
 *
 * @param value Value to create the real atom from.
 * @return A pointer to a Q object containing a real atom.
 */
static inline QObj *qNewReal(QReal value) {
    extern QObj *ke(QFloat);
    return ke((QFloat)value);
}

/**
 * @brief Create a Q object containing a float atom.
 *
 * @param value Value to create the float atom from.
 * @return A pointer to a Q object containing a float atom.
 */
static inline QObj *qNewFloat(QFloat value) {
    extern QObj *kf(QFloat);
    return kf(value);
}

/**
 * @brief Create a Q object containing a char atom.
 *
 * @param value Value to create the char atom from.
 * @return A pointer to a Q object containing a char atom.
 */
static inline QObj *qNewChar(QChar value) {
    extern QObj *kc(QInt);
    return kc((QInt)value);
}

/**
 * @brief Create a Q object containing a symbol atom.
 *
 * @param value Value to create the symbol atom from.
 * @return A pointer to a Q object containing a symbol atom.
 */
static inline QObj *qNewSymbol(const QChar *value) {
    extern QObj *ks(const QChar *);
    return ks(value);
}

/**
 * @brief Create a Q object containing a timestamp atom.
 *
 * @param value Value to create the timestamp atom from (nanoseconds since
 * 2000.01.01D00:00:00.000000000).
 * @return A pointer to a Q object containing a timestamp atom.
 */
static inline QObj *qNewTimestamp(QTimestamp value) {
    extern QObj *ktj(QInt, QLong);
    return ktj(-Q_TYPE_TIMESTAMP, (QLong)value);
}

/**
 * @brief Create a Q object containing a month atom.
 *
 * @param value Value to create the month atom from.
 * @return A pointer to a Q object containing a month atom.
 */
static inline QObj *qNewMonth(QMonth value) {
    extern QObj *ka(QInt);
    QObj *obj = ka(-Q_TYPE_MONTH);
    if (obj)
        obj->qint = (QInt)value;
    return obj;
}

/**
 * @brief Create a Q object containing a date atom.
 *
 * @param value Value to create the date atom from (days since 2000.01.01).
 * @return A pointer to a Q object containing a date atom.
 */
static inline QObj *qNewDate(QDate value) {
    extern QObj *kd(QInt);
    return kd((QInt)value);
}

/**
 * @brief Create a Q object containing a datetime atom.
 *
 * @param value Value to create the datetime atom from.
 * @return A pointer to a Q object containing a datetime atom.
 */
static inline QObj *qNewDatetime(QDatetime value) {
    extern QObj *kz(QFloat);
    return kz((QFloat)value);
}

/**
 * @brief Create a Q object containing a timespan atom.
 *
 * @param value Value (nanoseconds) to create the timespan atom from.
 * @return A pointer to a Q object containing a timespan atom.
 */
static inline QObj *qNewTimespan(QTimespan value) {
    extern QObj *ktj(QInt, QLong);
    return ktj(-Q_TYPE_TIMESPAN, (QLong)value);
}

/**
 * @brief Create a Q object containing a minute atom.
 *
 * @param value Value to create the minute atom from (minutes since 00:00).
 * @return A pointer to a Q object containing a minute atom.
 */
static inline QObj *qNewMinute(QMinute value) {
    extern QObj *ka(QInt);
    QObj *obj = ka(-Q_TYPE_MINUTE);
    if (obj)
        obj->qint = (QInt)value;
    return obj;
}

/**
 * @brief Create a Q object containing a second atom.
 *
 * @param value Value to create the second atom from (seconds since 00:00:00).
 * @return A pointer to a Q object containing a second atom.
 */
static inline QObj *qNewSecond(QSecond value) {
    extern QObj *ka(QInt);
    QObj *obj = ka(-Q_TYPE_SECOND);
    if (obj)
        obj->qint = (QInt)value;
    return obj;
}

/**
 * @brief Create a Q object containing a time atom.
 *
 * @param value Value to create the time atom from (milliseconds since 00:00:00.000).
 * @return A pointer to a Q object containing a time atom.
 */
static inline QObj *qNewTime(QTime value) {
    extern QObj *kt(QInt);
    return kt((QInt)value);
}

///// List Creation /////

/**
 * @brief Allocate a Q list of the given type and length without initialising its elements.
 *
 * @param type Type code for the list elements.
 * @param length Number of elements to allocate.
 * @return Q object containing an allocated list.
 */
static inline QObj *qNewList(QTypeCode type, QSize length) {
    extern QObj *ktn(QInt, QLong);
    if (length > Q_SIZE_MAX)
        return qNewError("domain");
    return ktn((QInt)type, (QLong)length);
}

/**
 * @brief Create a Q object containing a boolean list.
 *
 * @param values Array of values where zero represents false and non-zero values represent true.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a boolean list.
 */
QObj *qNewBooleanList(const QBoolean *values, QSize length);

/**
 * @brief Create a Q object containing a guid list.
 *
 * @param values Array of guid values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a guid list.
 */
QObj *qNewGuidList(const QGuid *values, QSize length);

/**
 * @brief Create a Q object containing a byte list.
 *
 * @param values Array of byte values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a byte list.
 */
QObj *qNewByteList(const QByte *values, QSize length);

/**
 * @brief Create a Q object containing a short list.
 *
 * @param values Array of short values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a short list.
 */
QObj *qNewShortList(const QShort *values, QSize length);

/**
 * @brief Create a Q object containing an int list.
 *
 * @param values Array of int values to populate the list.
 * @param length Number of elements in the list.
 * @return Q object containing an int list.
 */
QObj *qNewIntList(const QInt *values, QSize length);

/**
 * @brief Create a Q object containing a long list.
 *
 * @param values Array of long values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a long list.
 */
QObj *qNewLongList(const QLong *values, QSize length);

/**
 * @brief Create a Q object containing a real list.
 *
 * @param values Array of real values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a real list.
 */
QObj *qNewRealList(const QReal *values, QSize length);

/**
 * @brief Create a Q object containing a float list.
 *
 * @param values Array of float values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a float list.
 */
QObj *qNewFloatList(const QFloat *values, QSize length);

/**
 * @brief Create a Q object containing a char list.
 *
 * @param values Array of char values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a char list.
 */
QObj *qNewCharList(const QChar *values, QSize length);

/**
 * @brief Create a Q object containing a char list from a null-terminated string.
 *
 * @param string Null-terminated string to convert to char array.
 * @return A pointer to a Q object containing a char list.
 */
static inline QObj *qNewCharListFromString(const QChar *string) {
    extern QObj *kp(const QChar *);
    if (!string)
        return qNewError("domain");
    return kp(string);
}

/**
 * @brief Create a Q object containing a char list from a fixed length string.
 *
 * @param string Fixed length string to convert to char array.
 * @param length Number of characters to copy (excluding null terminator).
 * @return A pointer to a Q object containing a char list.
 */
static inline QObj *qNewCharListFromFixedString(const QChar *string, QSize length) {
    extern QObj *kpn(const QChar *, QLong);
    if (length > Q_SIZE_MAX || (!string && length > 0))
        return qNewError("domain");
    return kpn(string, (QLong)length);
}

/**
 * @brief Create a Q object containing a symbol list.
 *
 * @param values Array of symbol values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a symbol list.
 */
QObj *qNewSymbolList(const QSymbol *values, QSize length);

/**
 * @brief Create a Q object containing a timestamp list.
 *
 * @param values Array of timestamp values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a timestamp list.
 */
QObj *qNewTimestampList(const QTimestamp *values, QSize length);

/**
 * @brief Create a Q object containing a month list.
 *
 * @param values Array of month values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a month list.
 */
QObj *qNewMonthList(const QMonth *values, QSize length);

/**
 * @brief Create a Q object containing a date list.
 *
 * @param values Array of date values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a date list.
 */
QObj *qNewDateList(const QDate *values, QSize length);

/**
 * @brief Create a Q object containing a datetime list.
 *
 * @param values Array of datetime values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a datetime list.
 */
QObj *qNewDatetimeList(const QDatetime *values, QSize length);

/**
 * @brief Create a Q object containing a timespan list.
 *
 * @param values Array of timespan values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a timespan list.
 */
QObj *qNewTimespanList(const QTimespan *values, QSize length);

/**
 * @brief Create a Q object containing a minute list.
 *
 * @param values Array of minute values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a minute list.
 */
QObj *qNewMinuteList(const QMinute *values, QSize length);

/**
 * @brief Create a Q object containing a second list.
 *
 * @param values Array of second values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a second list.
 */
QObj *qNewSecondList(const QSecond *values, QSize length);

/**
 * @brief Create a Q object containing a time list.
 *
 * @param values Array of time values to populate the list.
 * @param length Number of elements in the list.
 * @return A pointer to a Q object containing a time list.
 */
QObj *qNewTimeList(const QTime *values, QSize length);

///// Complex Creation /////

/**
 * @brief Create a Q object containing a mixed list.
 *
 * @param length Number of elements in the list, which must equal the number of arguments that
 * follow.
 * @param ... Q object pointers to populate the list (takes ownership).
 * @return A pointer to a Q object containing a mixed list.
 *
 * @note Returns a domain error if any item is null, in which case the other items are released.
 * `length` must not exceed INT32_MAX, otherwise a domain error is returned before any items are
 * read (so they are not released).
 */
QObj *qNewMixedList(QSize length, ...);

/**
 * @brief Create a Q object containing a mixed list from an existing va_list.
 *
 * Intended for writing variadic wrappers. The caller still owns `args` and must call `va_end` on
 * it.
 *
 * @param length Number of elements in the list, which must equal the number of arguments in
 * `args`.
 * @param args Q object pointers to populate the list (takes ownership).
 * @return A pointer to a Q object containing a mixed list.
 *
 * @note See `qNewMixedList` for the errors returned.
 */
QObj *qNewMixedListVar(QSize length, va_list args);

/**
 * @brief Collapse a mixed list into a simple list or a table, where possible.
 *
 * A mixed list whose items are atoms of the same type becomes a simple list of that type (for
 * example, three long atoms become a long list). A mixed list whose items are conforming
 * dictionaries (the same symbol keys, in the same order) becomes a table, with one row per
 * dictionary. Any other mixed list (including an empty one) is returned unchanged, as is any object
 * that is not a mixed list.
 *
 * Only available inside a q process (for example, in a shared library loaded with 2:), as kdb+'s
 * vk is not provided by the standalone C library (c.o).
 *
 * @param obj A pointer to a Q object (takes ownership). Its items must not be null. If obj is null,
 * null is returned.
 * @return A pointer to the collapsed list or table, in which case obj has been released, or obj
 * itself if it was not collapsed.
 */
static inline QObj *qCollapseMixedList(QObj *obj) {
    extern QObj *vk(QObj *);
    // vk reads every item as a QObj pointer, so it must only be given a mixed list
    if (!obj || obj->type != Q_TYPE_MIXED)
        return obj;
    return vk(obj);
}

/**
 * @brief Create a Q object containing a dictionary.
 *
 * @param keys A pointer to a Q object containing a list of keys or a table (takes ownership).
 * @param values A pointer to a Q object containing a list of values or a table (takes ownership).
 * @return A pointer to a Q object containing a dictionary.
 *
 * @note keys and values must not be null, must be lists, dictionaries, or tables (not atoms or
 * functions), and must have the same count (see `qGetCount`). Otherwise a domain, type, or length
 * error is returned and both are released.
 */
QObj *qNewDict(QObj *keys, QObj *values);

/**
 * @brief Create a Q object containing a table from an array of columns.
 *
 * @param header A pointer to a Q object containing a symbol list of column names (takes ownership).
 * @param columns Array of `count` Q object pointers, each containing a single column (takes
 * ownership of each column, but not of the array itself). All columns should have equal lengths.
 * @param count Number of columns, which must equal the number of column names.
 * @return A pointer to a Q object containing a table.
 *
 * @note Returns a domain error if `header`, `columns` (with a non-zero `count`) or any column is
 * null, a type error if `header` is not a symbol list, or a length error if `count` is not the
 * number of column names. On error, `header` and all non-null columns are released.
 */
QObj *qNewTableFromArray(QObj *header, QObj *const *columns, QSize count);

#ifndef __cplusplus
/**
 * @brief Create a Q object containing a table.
 *
 * A macro that counts its column arguments and calls `qNewTableFromArray`, so the number of
 * columns is always known: on error, `header` and all columns are released, and a column count
 * that does not match the header is reported as a length error. Each column must be a `QObj *`.
 *
 * @param header A pointer to a Q object containing a symbol list of column names (takes ownership).
 * @param ... Q object pointers, each containing a single column (takes ownership). All columns
 * should have equal lengths, and there must be one per column name.
 * @return A pointer to a Q object containing a table.
 *
 * @note See `qNewTableFromArray` for the errors returned.
 */
#define qNewTable(header, ...)                                                                     \
    qNewTableFromArray((header), (QObj *[]){__VA_ARGS__ __VA_OPT__(, ) NULL},                      \
                       sizeof((QObj *[]){__VA_ARGS__ __VA_OPT__(, ) NULL}) / sizeof(QObj *) - 1)
#endif

/**
 * @brief Create a Q object containing a table from an existing va_list.
 *
 * Intended for writing variadic wrappers. Prefer `qNewTable` or `qNewTableFromArray`, which always
 * know the number of columns.
 *
 * @param header A pointer to a Q object containing a symbol list of column names (takes ownership).
 * @param args Q object pointers, each containing a single column (takes ownership). All columns
 * should have equal lengths. The number of columns read is the number of column names, so there
 * must be exactly one per column name.
 * @return A pointer to a Q object containing a table.
 *
 * @note Returns a domain error if `header` or any column is null, or a type error if `header` is
 * not a symbol list. `header` is released on error. The columns are also released, except when
 * `header` is null or not a symbol list, as the number of columns is then unknown.
 */
QObj *qNewTableVar(QObj *header, va_list args);

/**
 * @brief Create a Q object containing a keyed table.
 *
 * @param keys A pointer to a Q object containing a table that contains the key columns (takes
 * ownership).
 * @param values A pointer to a Q object containing a table that contains the value columns (takes
 * ownership).
 * @return A pointer to a Q object containing a keyed table.
 *
 * @note `keys` and `values` must not be null, must be tables, and must have the same row count.
 * Otherwise a domain, type, or length error is returned and both are released.
 */
static inline QObj *qNewKeyedTable(QObj *keys, QObj *values) {
    // Null inputs are reported (and released) by qNewDict
    if (keys && values && (!qIsTable(keys) || !qIsTable(values))) {
        decRef(keys);
        decRef(values);
        return qNewError("type");
    }
    return qNewDict(keys, values);
}

/**
 * @brief Create a Q object containing a keyed table from a Q object containing a simple table.
 *
 * @param nkeys Number of leading columns to use as the key columns.
 * @param table A pointer to a Q object containing the table to key (does not take ownership).
 * @return A pointer to a Q object containing a keyed table or a null pointer if an error occurred.
 *
 * @note Does not take ownership of table. The first nkeys columns become the key table, and the
 * remaining columns become the value table.
 * @note keys and values must be tables and have the same row count.
 */
static inline QObj *qKeyTable(QSize nkeys, QObj *table) {
    extern QObj *knt(QLong, QObj *);
    if (nkeys > Q_SIZE_MAX)
        return qNewError("domain");
    if (!qIsTable(table))
        return qNewError("type");
    return knt((QLong)nkeys, table);
}

/**
 * @brief Create a Q object containing a simple table from a Q object containing a keyed table.
 *
 * @param keyedTable A pointer to a Q object containing the keyed table to unkey (takes ownership).
 * @return A pointer to a Q object containing a simple table.
 */
static inline QObj *qUnkeyTable(QObj *keyedTable) {
    extern QObj *ktd(QObj *);
    return ktd(keyedTable);
}

#ifdef __cplusplus
}

/**
 * @brief Create a Q object containing a table (C++).
 *
 * The C++ equivalent of the `qNewTable` macro: the number of columns is known at compile time and
 * each column must be convertible to `QObj *`.
 *
 * @note See `qNewTableFromArray` for the errors returned.
 */
template <typename... Columns>
inline QObj *qNewTable(QObj *header, Columns... columns) {
    QObj *array[] = {static_cast<QObj *>(columns)..., nullptr};
    return qNewTableFromArray(header, array, sizeof...(columns));
}
#endif

#endif // QLIB_NEW_H
