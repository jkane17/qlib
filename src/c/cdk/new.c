
/**
 * @file new.c
 * @brief Q object creators.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "mem.h"
#include "new.h"
#include "obj.h"
#include "sym.h"
#include "type.h"

extern QObj *xD(QObj *, QObj *);
extern QObj *xT(QObj *);

// The values are copied with a single memcpy, which requires the value type and the list's storage
// type to have the same representation. memcpy is skipped for an empty list, as values may then be
// NULL (passing NULL to memcpy is undefined behaviour, even with a size of 0).
#define Q_NEW_LIST_IMPL(funcName, qType, storageType, valueType)                                   \
    static_assert(sizeof(storageType) == sizeof(valueType),                                        \
                  #funcName ": value and storage types must have the same size");                  \
    QObj *funcName(const valueType *values, QSize length) {                                        \
        if (!values && length > 0)                                                                 \
            return qNewError("domain");                                                            \
                                                                                                   \
        QObj *obj = qNewList(qType, length);                                                       \
        if (obj && length > 0)                                                                     \
            memcpy(obj->list, values, length * sizeof(storageType));                               \
        return obj;                                                                                \
    }

QObj *qNewBooleanList(const QBoolean *values, QSize length) {
    if (!values && length > 0)
        return qNewError("domain");

    QObj *obj = qNewList(Q_TYPE_BOOLEAN, length);
    if (obj) {
        QBoolean *list = (QBoolean *)obj->list;
        for (QSize i = 0; i < length; i++) {
            list[i] = values[i] == 0 ? 0 : 1;
        }
    }
    return obj;
}

QObj *qNewSymbolList(const QSymbol *values, QSize length) {
    if (!values && length > 0)
        return qNewError("domain");

    QObj *obj = qNewList(Q_TYPE_SYMBOL, length);
    if (obj) {
        QSymbol *list = (QSymbol *)obj->list;
        for (QSize i = 0; i < length; i++) {
            list[i] = qInternString(values[i]);
        }
    }
    return obj;
}

Q_NEW_LIST_IMPL(qNewGuidList, Q_TYPE_GUID, QGuid, QGuid)
Q_NEW_LIST_IMPL(qNewByteList, Q_TYPE_BYTE, QByte, QByte)
Q_NEW_LIST_IMPL(qNewShortList, Q_TYPE_SHORT, QShort, QShort)
Q_NEW_LIST_IMPL(qNewIntList, Q_TYPE_INT, QInt, QInt)
Q_NEW_LIST_IMPL(qNewLongList, Q_TYPE_LONG, QLong, QLong)
Q_NEW_LIST_IMPL(qNewRealList, Q_TYPE_REAL, QReal, QReal)
Q_NEW_LIST_IMPL(qNewFloatList, Q_TYPE_FLOAT, QFloat, QFloat)
Q_NEW_LIST_IMPL(qNewCharList, Q_TYPE_CHAR, QChar, QChar)
Q_NEW_LIST_IMPL(qNewTimestampList, Q_TYPE_TIMESTAMP, QLong, QTimestamp)
Q_NEW_LIST_IMPL(qNewMonthList, Q_TYPE_MONTH, QInt, QMonth)
Q_NEW_LIST_IMPL(qNewDateList, Q_TYPE_DATE, QInt, QDate)
Q_NEW_LIST_IMPL(qNewDatetimeList, Q_TYPE_DATETIME, QFloat, QDatetime)
Q_NEW_LIST_IMPL(qNewTimespanList, Q_TYPE_TIMESPAN, QLong, QTimespan)
Q_NEW_LIST_IMPL(qNewMinuteList, Q_TYPE_MINUTE, QInt, QMinute)
Q_NEW_LIST_IMPL(qNewSecondList, Q_TYPE_SECOND, QInt, QSecond)
Q_NEW_LIST_IMPL(qNewTimeList, Q_TYPE_TIME, QInt, QTime)

QObj *qNewDict(QObj *keys, QObj *values) {
    if (!keys || !values) {
        if (keys)
            decRef(keys);
        if (values)
            decRef(values);
        return qNewError("domain");
    }

    // Keys and values must be lists, dictionaries, or tables (not atoms or functions)
    if (keys->type < 0 || keys->type > Q_TYPE_DICTIONARY || values->type < 0 ||
        values->type > Q_TYPE_DICTIONARY) {
        decRef(keys);
        decRef(values);
        return qNewError("type");
    }

    if (qGetCount(keys) != qGetCount(values)) {
        decRef(keys);
        decRef(values);
        return qNewError("length");
    }

    return xD(keys, values);
}

QObj *qNewTableFromArray(QObj *header, QObj *const *columns, QSize count) {
    const QChar *error = NULL;
    if (!header || (!columns && count > 0))
        error = "domain";
    else if (!qIsSymbolList(header))
        error = "type";
    else if (header->length != count)
        error = "length";
    else {
        for (QSize i = 0; i < count; i++) {
            if (!columns[i]) {
                error = "domain";
                break;
            }
        }
    }

    // The column count is always known, so every argument can be released on error
    if (error) {
        if (header)
            decRef(header);
        for (QSize i = 0; columns && i < count; i++) {
            if (columns[i])
                decRef(columns[i]);
        }
        return qNewError(error);
    }

    QObj *list = qNewList(Q_TYPE_MIXED, count);
    if (!list) {
        decRef(header);
        for (QSize i = 0; i < count; i++)
            decRef(columns[i]);
        return list;
    }
    for (QSize i = 0; i < count; i++)
        ((QObj **)list->list)[i] = columns[i];

    QObj *dict = qNewDict(header, list);
    if (!dict)
        return dict;

    return xT(dict);
}

QObj *qNewTableVar(QObj *header, va_list args) {
    if (!header)
        return qNewError("domain");

    // The number of columns is only known from a valid header, so the columns cannot be released
    // if it is not one
    if (!qIsSymbolList(header)) {
        decRef(header);
        return qNewError("type");
    }

    QSize count = header->length;
    QObj **columns = malloc((count ? count : 1) * sizeof(QObj *));
    if (!columns) {
        for (QSize i = 0; i < count; i++) {
            QObj *column = va_arg(args, QObj *);
            if (column)
                decRef(column);
        }
        decRef(header);
        return qNewError("wsfull");
    }

    for (QSize i = 0; i < count; i++)
        columns[i] = va_arg(args, QObj *);

    QObj *table = qNewTableFromArray(header, columns, count);
    free(columns);
    return table;
}
