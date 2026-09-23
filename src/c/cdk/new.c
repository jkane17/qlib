
/**
 * @file new.c
 * @brief Q object creators.
 */

#include <stdarg.h>

#include "err.h"
#include "mem.h"
#include "new.h"
#include "obj.h"
#include "type.h"

extern QSymbol ss(QSymbol str);
extern QObj *xD(QObj *, QObj *);
extern QObj *xT(QObj *);

#define Q_NEW_LIST_IMPL(funcName, qType, storageType, valueType) \
    QObj *funcName(const valueType *values, QSize length) {      \
        if (!values && length > 0)                               \
            return qNewError("domain");                          \
                                                                 \
        QObj *obj = qNewList(qType, length);                     \
        if (obj) {                                               \
            storageType *list = (storageType *)obj->list;        \
            for (QSize i = 0; i < length; i++) {                 \
                list[i] = values[i];                             \
            }                                                    \
        }                                                        \
        return obj;                                              \
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
            list[i] = ss(values[i]);
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

QObj *qNewTable(QObj *header, ...) {
    va_list args;
    va_start(args, header);
    QObj *table = qNewTableVar(header, args);
    va_end(args);
    return table;
}

QObj *qNewTableVar(QObj *header, va_list args) {
    if (!header)
        return qNewError("domain");

    if (!qIsSymbolList(header)) {
        decRef(header);
        return qNewError("type");
    }

    // Scan a copy first so that a missing column can be reported before any objects are consumed
    bool hasNullColumn = false;
    va_list scan;
    va_copy(scan, args);
    for (QSize i = 0; i < header->length; i++) {
        if (!va_arg(scan, QObj *))
            hasNullColumn = true;
    }
    va_end(scan);

    if (hasNullColumn) {
        for (QSize i = 0; i < header->length; i++) {
            QObj *column = va_arg(args, QObj *);
            if (column)
                decRef(column);
        }
        decRef(header);
        return qNewError("domain");
    }

    QObj *columns = qNewMixedListVar(header->length, args);
    if (!columns) {
        decRef(header);
        return columns;
    }

    QObj *dict = qNewDict(header, columns);
    if (!dict)
        return dict;

    return xT(dict);
}
