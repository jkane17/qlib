
/**
 * @file fmt.c
 * @brief String formatting.
 */

#include <inttypes.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fmt.h"
#include "type.h"
#include "util.h"

// Fixed widths
#define BOOLEAN_WIDTH 1
#define GUID_WIDTH 36
#define BYTE_WIDTH 2
#define CHAR_WIDTH 1
#define TIMESTAMP_WIDTH 29
#define MONTH_WIDTH 7
#define DATE_WIDTH 10
#define DATETIME_WIDTH 23
#define MINUTE_WIDTH 5

#define SECS_PER_DAY 86400
#define SECS_PER_HOUR 3600
#define SECS_PER_MIN 60
#define MS_PER_HOUR 3600000
#define MS_PER_MIN 60000
#define MS_PER_SEC 1000
#define NS_PER_SEC 1000000000LL

#define EPOCH_DIFF 946684800LL // seconds between 1970 and 2000

#define WRITE_FIXED(buffer, size, width, fmt, ...)                                                 \
    if (buffer == nullptr || size < (width) + 1)                                                   \
        return Q_FMT_ERROR_SIZE;                                                                   \
    int result = snprintf(buffer, size, fmt, ##__VA_ARGS__);                                       \
    return result < 0 ? Q_FMT_ERROR_PRINTF : result

#define WRITE_DYNAMIC(buffer, size, fmt, ...)                                                      \
    int width = snprintf(nullptr, 0, fmt, ##__VA_ARGS__);                                          \
    if (width < 0)                                                                                 \
        return Q_FMT_ERROR_PRINTF;                                                                 \
    WRITE_FIXED(buffer, size, (QSize)width, fmt, ##__VA_ARGS__)

// Item accessors for WRITE_LIST_AT: pass each item by value, or by address (for guids)
#define ITEM_VALUE(list, i) ((list)[i])
#define ITEM_ADDRESS(list, i) (&(list)[i])

#define WRITE_LIST(buffer, size, func, list, length, spaced, result)                               \
    WRITE_LIST_AT(buffer, size, func, list, length, spaced, result, ITEM_VALUE)

#define WRITE_LIST_AT(buffer, size, func, list, length, spaced, result, item)                      \
    char *bufferPosition = buffer;                                                                 \
                                                                                                   \
    for (QSize i = 0; i < length; i++) {                                                           \
        int nbytes = func(bufferPosition, size, item(list, i));                                    \
        if (nbytes < 0)                                                                            \
            return nbytes;                                                                         \
                                                                                                   \
        bufferPosition += nbytes;                                                                  \
        result += nbytes;                                                                          \
        size = saturatedSub(size, nbytes);                                                         \
                                                                                                   \
        if (spaced && i != length - 1) {                                                           \
            if (size < 1)                                                                          \
                return Q_FMT_ERROR_SIZE;                                                           \
            *bufferPosition = ' ';                                                                 \
            bufferPosition += 1;                                                                   \
            result += 1;                                                                           \
            size = saturatedSub(size, 1);                                                          \
        }                                                                                          \
    }

#define WRITE_PRECISION_LIST(buffer, size, func, list, length, spaced, result, precision)          \
    char *bufferPosition = buffer;                                                                 \
                                                                                                   \
    for (QSize i = 0; i < length; i++) {                                                           \
        int nbytes = func(bufferPosition, size, (list)[i], precision);                             \
        if (nbytes < 0)                                                                            \
            return nbytes;                                                                         \
                                                                                                   \
        bufferPosition += nbytes;                                                                  \
        result += nbytes;                                                                          \
        size = saturatedSub(size, nbytes);                                                         \
                                                                                                   \
        if (spaced && i != length - 1) {                                                           \
            if (size < 1)                                                                          \
                return Q_FMT_ERROR_SIZE;                                                           \
            *bufferPosition = ' ';                                                                 \
            bufferPosition += 1;                                                                   \
            result += 1;                                                                           \
            size = saturatedSub(size, 1);                                                          \
        }                                                                                          \
    }

#define WRITE_MONTH(buffer, size, month, width, fmt)                                               \
    int32_t year = 2000 + month / 12;                                                              \
    month %= 12;                                                                                   \
    if (month < 0) {                                                                               \
        month += 12;                                                                               \
        year -= 1;                                                                                 \
    }                                                                                              \
    month += 1;                                                                                    \
    WRITE_FIXED(buffer, size, width, fmt, year, month)

static bool isLeap(int32_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static void splitHMS(int64_t total, int64_t *hours, int64_t *mins, int64_t *secs) {
    *hours = total / SECS_PER_HOUR;
    total %= SECS_PER_HOUR;
    *mins = total / SECS_PER_MIN;
    *secs = total % SECS_PER_MIN;
}

static void splitDHMS(int64_t total, int64_t *days, int64_t *hours, int64_t *mins, int64_t *secs) {
    *days = total / SECS_PER_DAY;
    total %= SECS_PER_DAY;
    splitHMS(total, hours, mins, secs);
}

static void splitHMSMS(int64_t total, int64_t *hours, int64_t *mins, int64_t *secs, int64_t *ms) {
    *hours = total / MS_PER_HOUR;
    total %= MS_PER_HOUR;

    *mins = total / MS_PER_MIN;
    total %= MS_PER_MIN;

    *secs = total / MS_PER_SEC;
    *ms = total % MS_PER_SEC;
}

static void
splitNS(int64_t total, int64_t *days, int64_t *hours, int64_t *mins, int64_t *secs, int64_t *ns) {
    int64_t s = total / NS_PER_SEC;
    *ns = total % NS_PER_SEC;
    splitDHMS(s, days, hours, mins, secs);
}

int qBooleanToStr(char *buffer, QSize size, QBoolean boolean) {
    WRITE_FIXED(buffer, size, BOOLEAN_WIDTH, "%hhu", boolean);
}

int qBooleanToLiteral(char *buffer, QSize size, QBoolean boolean) {
    WRITE_FIXED(buffer, size, BOOLEAN_WIDTH + 1, "%hhub", boolean);
}

int qGuidToStr(char *buffer, QSize size, const QGuid *guid) {
    static const char hex[] = "0123456789abcdef";

    if (guid == nullptr)
        return 0; // Nothing to write
    if (buffer == nullptr || size < GUID_WIDTH + 1)
        return Q_FMT_ERROR_SIZE;

    QSize pos = 0;

    for (QSize i = 0; i < 16; i++) {
        buffer[pos++] = hex[guid->bytes[i] >> 4];
        buffer[pos++] = hex[guid->bytes[i] & 0x0F];
        if (i == 3 || i == 5 || i == 7 || i == 9)
            buffer[pos++] = '-';
    }

    buffer[pos] = '\0';
    return pos;
}

int qGuidToLiteral(char *buffer, QSize size, const QGuid *guid) {
    return qGuidToStr(buffer, size, guid);
}

int qByteToStr(char *buffer, QSize size, QByte byte) {
    WRITE_FIXED(buffer, size, BYTE_WIDTH, "%02x", byte);
}

int qByteToLiteral(char *buffer, QSize size, QByte byte) {
    WRITE_FIXED(buffer, size, BYTE_WIDTH + 2, "0x%02x", byte);
}

int qShortToStr(char *buffer, QSize size, QShort short_) {
    WRITE_DYNAMIC(buffer, size, "%d", short_);
}

int qShortToLiteral(char *buffer, QSize size, QShort short_) {
    WRITE_DYNAMIC(buffer, size, "%dh", short_);
}

int qIntToStr(char *buffer, QSize size, QInt int_) {
    WRITE_DYNAMIC(buffer, size, "%d", int_);
}

int qIntToLiteral(char *buffer, QSize size, QInt int_) {
    WRITE_DYNAMIC(buffer, size, "%di", int_);
}

int qLongToStr(char *buffer, QSize size, QLong long_) {
    WRITE_DYNAMIC(buffer, size, "%" PRId64, long_);
}

int qLongToLiteral(char *buffer, QSize size, QLong long_) {
    return qLongToStr(buffer, size, long_);
}

int qRealToStr(char *buffer, QSize size, QReal real, int precision) {
    WRITE_DYNAMIC(buffer, size, "%.*f", precision, (QFloat)real);
}

int qRealToLiteral(char *buffer, QSize size, QReal real, int precision) {
    WRITE_DYNAMIC(buffer, size, "%.*fe", precision, (QFloat)real);
}

int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision) {
    WRITE_DYNAMIC(buffer, size, "%.*f", precision, float_);
}

int qFloatToLiteral(char *buffer, QSize size, QFloat float_, int precision) {
    return qFloatToStr(buffer, size, float_, precision);
}

int qCharToStr(char *buffer, QSize size, QChar char_) {
    WRITE_FIXED(buffer, size, CHAR_WIDTH, "%c", char_);
}

int qCharToLiteral(char *buffer, QSize size, QChar char_) {
    WRITE_FIXED(buffer, size, CHAR_WIDTH + 2, "\"%c\"", char_);
}

int qSymbolToStr(char *buffer, QSize size, QSymbol symbol) {
    WRITE_DYNAMIC(buffer, size, "%s", symbol);
}

int qSymbolToLiteral(char *buffer, QSize size, QSymbol symbol) {
    WRITE_DYNAMIC(buffer, size, "`%s", symbol);
}

int qTimestampToStr(char *buffer, QSize size, QTimestamp timestamp) {
    int64_t sec = timestamp / NS_PER_SEC;
    int64_t ns = timestamp % NS_PER_SEC;

    if (ns < 0) {
        ns += NS_PER_SEC;
        sec--;
    }

    time_t t = (time_t)(sec + EPOCH_DIFF);
    struct tm tm;
    gmtime_r(&t, &tm);

    WRITE_FIXED(
        buffer,
        size,
        TIMESTAMP_WIDTH,
        "%04d.%02d.%02dD%02d:%02d:%02d.%09" PRId64,
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        ns);
}

int qTimestampToLiteral(char *buffer, QSize size, QTimestamp timestamp) {
    return qTimestampToStr(buffer, size, timestamp);
}

int qMonthToStr(char *buffer, QSize size, QMonth month) {
    WRITE_MONTH(buffer, size, month, MONTH_WIDTH, "%04d.%02d");
}

int qMonthToLiteral(char *buffer, QSize size, QMonth month) {
    WRITE_MONTH(buffer, size, month, MONTH_WIDTH + 1, "%04d.%02dm");
}

int qDateToStr(char *buffer, QSize size, QDate date) {
    time_t t = (time_t)(date * SECS_PER_DAY + EPOCH_DIFF);

    struct tm tm;
    gmtime_r(&t, &tm);

    WRITE_FIXED(
        buffer, size, DATE_WIDTH, "%04d.%02d.%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int qDateToLiteral(char *buffer, QSize size, QDate date) {
    return qDateToStr(buffer, size, date);
}

int qDatetimeToStr(char *buffer, QSize size, QDatetime datetime) {
    int64_t days = (int64_t)floor(datetime);
    double frac = datetime - days;
    int32_t year = 2000;

    if (days >= 0) {
        while (true) {
            int32_t dy = isLeap(year) ? 366 : 365;
            if (days >= dy) {
                days -= dy;
                year++;
            } else {
                break;
            }
        }
    } else {
        while (days < 0) {
            year--;
            days += isLeap(year) ? 366 : 365;
        }
    }

    int32_t mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeap(year)) {
        mdays[1] = 29;
    }

    int32_t month = 0;
    while (days >= mdays[month]) {
        days -= mdays[month];
        month++;
    }

    int32_t day = (int32_t)days + 1;

    double total_seconds = frac * SECS_PER_DAY;

    int32_t hour = (int32_t)(total_seconds / SECS_PER_HOUR);
    total_seconds -= hour * SECS_PER_HOUR;

    int32_t minute = (int32_t)(total_seconds / SECS_PER_MIN);
    total_seconds -= minute * SECS_PER_MIN;

    int32_t second = (int32_t)total_seconds;
    double rem = total_seconds - second;

    int32_t millis = (int32_t)round(rem * 1000.0);

    if (millis == 1000) {
        millis = 0;
        second++;

        if (second == 60) {
            second = 0;
            minute++;

            if (minute == 60) {
                minute = 0;
                hour++;

                if (hour == 24) {
                    hour = 0;
                    day++;

                    if (day > mdays[month]) {
                        day = 1;
                        month++;

                        if (month == 12) {
                            month = 0;
                            year++;

                            mdays[1] = isLeap(year) ? 29 : 28;
                        }
                    }
                }
            }
        }
    }

    WRITE_FIXED(
        buffer,
        size,
        DATETIME_WIDTH,
        "%04d.%02d.%02dT%02d:%02d:%02d.%03d",
        year,
        month + 1,
        day,
        hour,
        minute,
        second,
        millis);
}

int qDatetimeToLiteral(char *buffer, QSize size, QDatetime datetime) {
    return qDatetimeToStr(buffer, size, datetime);
}

int qTimespanToStr(char *buffer, QSize size, QTimespan timespan) {
    bool neg = timespan < 0;
    int64_t total = llabs(timespan);

    int64_t days, hours, mins, secs, ns;
    splitNS(total, &days, &hours, &mins, &secs, &ns);

    WRITE_DYNAMIC(
        buffer,
        size,
        "%s"
        "%" PRId64 "D%02" PRId64 ":%02" PRId64 ":%02" PRId64 ".%09" PRId64,
        neg ? "-" : "",
        days,
        hours,
        mins,
        secs,
        ns);
}

int qTimespanToLiteral(char *buffer, QSize size, QTimespan timespan) {
    return qTimespanToStr(buffer, size, timespan);
}

int qMinuteToStr(char *buffer, QSize size, QMinute minute) {
    bool neg = minute < 0;

    int64_t total = minute;
    total = llabs(total);

    int64_t hours = total / SECS_PER_MIN;
    int64_t mins = total % SECS_PER_MIN;

    WRITE_DYNAMIC(buffer, size, "%s%02" PRId64 ":%02" PRId64, neg ? "-" : "", hours, mins);
}

int qMinuteToLiteral(char *buffer, QSize size, QMinute minute) {
    return qMinuteToStr(buffer, size, minute);
}

int qSecondToStr(char *buffer, QSize size, QSecond second) {
    bool neg = second < 0;

    int64_t total = second;
    total = llabs(total);

    int64_t hours, mins, secs;
    splitHMS(total, &hours, &mins, &secs);

    WRITE_DYNAMIC(
        buffer,
        size,
        "%s%02" PRId64 ":%02" PRId64 ":%02" PRId64,
        neg ? "-" : "",
        hours,
        mins,
        secs);
}

int qSecondToLiteral(char *buffer, QSize size, QSecond second) {
    return qSecondToStr(buffer, size, second);
}

int qTimeToStr(char *buffer, QSize size, QTime time) {
    bool neg = time < 0;

    int64_t total = time;
    total = llabs(total);

    int64_t hours, mins, secs, ms;
    splitHMSMS(total, &hours, &mins, &secs, &ms);

    WRITE_DYNAMIC(
        buffer,
        size,
        "%s%02" PRId64 ":%02" PRId64 ":%02" PRId64 ".%03" PRId64,
        neg ? "-" : "",
        hours,
        mins,
        secs,
        ms);
}

int qTimeToLiteral(char *buffer, QSize size, QTime time) {
    return qTimeToStr(buffer, size, time);
}

int qAtomToStr(char *buffer, QSize size, const void *atom, QTypeCode type, int precision) {
    switch (type) {
        case Q_TYPE_BOOLEAN:
            return qBooleanToStr(buffer, size, *(const QBoolean *)atom);
        case Q_TYPE_GUID:
            return qGuidToStr(buffer, size, (const QGuid *)atom);
        case Q_TYPE_BYTE:
            return qByteToStr(buffer, size, *(const QByte *)atom);
        case Q_TYPE_SHORT:
            return qShortToStr(buffer, size, *(const QShort *)atom);
        case Q_TYPE_INT:
            return qIntToStr(buffer, size, *(const QInt *)atom);
        case Q_TYPE_LONG:
            return qLongToStr(buffer, size, *(const QLong *)atom);
        case Q_TYPE_REAL:
            return qRealToStr(buffer, size, *(const QReal *)atom, precision);
        case Q_TYPE_FLOAT:
            return qFloatToStr(buffer, size, *(const QFloat *)atom, precision);
        case Q_TYPE_CHAR:
            return qCharToStr(buffer, size, *(const QChar *)atom);
        case Q_TYPE_SYMBOL:
            return qSymbolToStr(buffer, size, (const QSymbol)atom);
        case Q_TYPE_TIMESTAMP:
            return qTimestampToStr(buffer, size, *(const QTimestamp *)atom);
        case Q_TYPE_MONTH:
            return qMonthToStr(buffer, size, *(const QMonth *)atom);
        case Q_TYPE_DATE:
            return qDateToStr(buffer, size, *(const QDate *)atom);
        case Q_TYPE_DATETIME:
            return qDatetimeToStr(buffer, size, *(const QDatetime *)atom);
        case Q_TYPE_TIMESPAN:
            return qTimespanToStr(buffer, size, *(const QTimespan *)atom);
        case Q_TYPE_MINUTE:
            return qMinuteToStr(buffer, size, *(const QMinute *)atom);
        case Q_TYPE_SECOND:
            return qSecondToStr(buffer, size, *(const QSecond *)atom);
        case Q_TYPE_TIME:
            return qTimeToStr(buffer, size, *(const QTime *)atom);

        default:
            return Q_FMT_ERROR_TYPE;
    }
}

int qAtomToLiteral(char *buffer, QSize size, const void *atom, QTypeCode type, int precision) {
    switch (type) {
        case Q_TYPE_BOOLEAN:
            return qBooleanToLiteral(buffer, size, *(const QBoolean *)atom);
        case Q_TYPE_GUID:
            return qGuidToLiteral(buffer, size, (const QGuid *)atom);
        case Q_TYPE_BYTE:
            return qByteToLiteral(buffer, size, *(const QByte *)atom);
        case Q_TYPE_SHORT:
            return qShortToLiteral(buffer, size, *(const QShort *)atom);
        case Q_TYPE_INT:
            return qIntToLiteral(buffer, size, *(const QInt *)atom);
        case Q_TYPE_LONG:
            return qLongToLiteral(buffer, size, *(const QLong *)atom);
        case Q_TYPE_REAL:
            return qRealToLiteral(buffer, size, *(const QReal *)atom, precision);
        case Q_TYPE_FLOAT:
            return qFloatToLiteral(buffer, size, *(const QFloat *)atom, precision);
        case Q_TYPE_CHAR:
            return qCharToLiteral(buffer, size, *(const QChar *)atom);
        case Q_TYPE_SYMBOL:
            return qSymbolToLiteral(buffer, size, (const QSymbol)atom);
        case Q_TYPE_TIMESTAMP:
            return qTimestampToLiteral(buffer, size, *(const QTimestamp *)atom);
        case Q_TYPE_MONTH:
            return qMonthToLiteral(buffer, size, *(const QMonth *)atom);
        case Q_TYPE_DATE:
            return qDateToLiteral(buffer, size, *(const QDate *)atom);
        case Q_TYPE_DATETIME:
            return qDatetimeToLiteral(buffer, size, *(const QDatetime *)atom);
        case Q_TYPE_TIMESPAN:
            return qTimespanToLiteral(buffer, size, *(const QTimespan *)atom);
        case Q_TYPE_MINUTE:
            return qMinuteToLiteral(buffer, size, *(const QMinute *)atom);
        case Q_TYPE_SECOND:
            return qSecondToLiteral(buffer, size, *(const QSecond *)atom);
        case Q_TYPE_TIME:
            return qTimeToLiteral(buffer, size, *(const QTime *)atom);

        default:
            return Q_FMT_ERROR_TYPE;
    }
}

int qBooleanListToStr(char *buffer, QSize size, const QBoolean *booleans, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qBooleanToStr, booleans, length, true, result);
    return result;
}

int qBooleanListToLiteral(char *buffer, QSize size, const QBoolean *booleans, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;

    if (length > 1) {
        WRITE_LIST(buffer, size, qBooleanToStr, booleans, length - 1, false, result);
        if (result < 0)
            return result;
    }

    int tailResult = qBooleanToLiteral(buffer + result, size, booleans[length - 1]);
    if (tailResult < 0)
        return tailResult;

    return result + tailResult;
}

int qGuidListToStr(char *buffer, QSize size, const QGuid *guids, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST_AT(buffer, size, qGuidToStr, guids, length, true, result, ITEM_ADDRESS);
    return result;
}

int qGuidListToLiteral(char *buffer, QSize size, const QGuid *guids, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST_AT(buffer, size, qGuidToLiteral, guids, length, true, result, ITEM_ADDRESS);
    return result;
}

int qByteListToStr(char *buffer, QSize size, const QByte *bytes, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qByteToStr, bytes, length, true, result);
    return result;
}

int qByteListToLiteral(char *buffer, QSize size, const QByte *bytes, QSize length) {
    if (length == 0)
        return 0;

    int headResult = qByteToLiteral(buffer, size, bytes[0]);
    if (headResult < 0)
        return headResult;

    int result = 0;

    if (length > 1) {
        WRITE_LIST(buffer + headResult, size, qByteToStr, bytes + 1, length - 1, false, result);
        if (result < 0)
            return result;
    }

    return headResult + result;
}

int qShortListToStr(char *buffer, QSize size, const QShort *shorts, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qShortToStr, shorts, length, true, result);
    return result;
}

int qShortListToLiteral(char *buffer, QSize size, const QShort *shorts, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;

    if (length > 1) {
        WRITE_LIST(buffer, size, qShortToStr, shorts, length - 1, true, result);
        if (result < 0)
            return result;

        // Append a space before the final value is appended
        if (size < 1)
            return Q_FMT_ERROR_SIZE;
        buffer[result] = ' ';
        result += 1;
        size = saturatedSub(size, 1);
    }

    int tailResult = qShortToLiteral(buffer + result, size, shorts[length - 1]);
    if (tailResult < 0)
        return tailResult;

    return result + tailResult;
}

int qIntListToStr(char *buffer, QSize size, const QInt *ints, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qIntToStr, ints, length, true, result);
    return result;
}

int qIntListToLiteral(char *buffer, QSize size, const QInt *ints, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;

    if (length > 1) {
        WRITE_LIST(buffer, size, qIntToStr, ints, length - 1, true, result);
        if (result < 0)
            return result;

        // Append a space before the final value is appended
        if (size < 1)
            return Q_FMT_ERROR_SIZE;
        buffer[result] = ' ';
        result += 1;
        size = saturatedSub(size, 1);
    }

    int tailResult = qIntToLiteral(buffer + result, size, ints[length - 1]);
    if (tailResult < 0)
        return tailResult;

    return result + tailResult;
}

int qLongListToStr(char *buffer, QSize size, const QLong *longs, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qLongToStr, longs, length, true, result);
    return result;
}

int qLongListToLiteral(char *buffer, QSize size, const QLong *longs, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qLongToLiteral, longs, length, true, result);
    return result;
}

int qRealListToStr(char *buffer, QSize size, const QReal *reals, QSize length, int precision) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_PRECISION_LIST(buffer, size, qRealToStr, reals, length, true, result, precision);
    return result;
}

int qRealListToLiteral(char *buffer, QSize size, const QReal *reals, QSize length, int precision) {
    if (length == 0)
        return 0;

    int result = 0;

    if (length > 1) {
        WRITE_PRECISION_LIST(buffer, size, qRealToStr, reals, length - 1, true, result, precision);
        if (result < 0)
            return result;

        // Append a space before the final value is appended
        if (size < 1)
            return Q_FMT_ERROR_SIZE;
        buffer[result] = ' ';
        result += 1;
        size = saturatedSub(size, 1);
    }

    int tailResult = qRealToLiteral(buffer + result, size, reals[length - 1], precision);
    if (tailResult < 0)
        return tailResult;

    return result + tailResult;
}

int qFloatListToStr(char *buffer, QSize size, const QFloat *floats, QSize length, int precision) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_PRECISION_LIST(buffer, size, qFloatToStr, floats, length, true, result, precision);
    return result;
}

int qFloatListToLiteral(
    char *buffer, QSize size, const QFloat *floats, QSize length, int precision) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_PRECISION_LIST(buffer, size, qFloatToLiteral, floats, length, true, result, precision);
    return result;
}

int qCharListToStr(char *buffer, QSize size, const QChar *chars, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qCharToStr, chars, length, true, result);
    return result;
}

int qCharListToLiteral(char *buffer, QSize size, const QChar *chars, QSize length) {
    if (length == 0)
        return 0;

    // Opening quote
    int headResult = qCharToStr(buffer, size, '"');
    if (headResult < 0)
        return headResult;
    size = saturatedSub(size, headResult);

    // Characters
    int result = 0;
    WRITE_LIST(buffer + headResult, size, qCharToStr, chars, length, false, result);
    if (result < 0)
        return result;
    size = saturatedSub(size, result);

    // Closing quote
    int tailResult = qCharToStr(buffer + headResult + result, size, '"');
    if (tailResult < 0)
        return tailResult;

    return headResult + result + tailResult;
}

int qSymbolListToStr(char *buffer, QSize size, const QSymbol *symbols, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qSymbolToStr, symbols, length, true, result);
    return result;
}

int qSymbolListToLiteral(char *buffer, QSize size, const QSymbol *symbols, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qSymbolToLiteral, symbols, length, false, result);
    return result;
}

int qTimestampListToStr(char *buffer, QSize size, const QTimestamp *timestamps, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimestampToStr, timestamps, length, true, result);
    return result;
}

int qTimestampListToLiteral(char *buffer, QSize size, const QTimestamp *timestamps, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimestampToLiteral, timestamps, length, true, result);
    return result;
}

int qMonthListToStr(char *buffer, QSize size, const QMonth *months, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qMonthToStr, months, length, true, result);
    return result;
}

int qMonthListToLiteral(char *buffer, QSize size, const QMonth *months, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;

    if (length > 1) {
        WRITE_LIST(buffer, size, qMonthToStr, months, length - 1, true, result);
        if (result < 0)
            return result;

        // Append a space before the final value is appended
        if (size < 1)
            return Q_FMT_ERROR_SIZE;
        buffer[result] = ' ';
        result += 1;
        size = saturatedSub(size, 1);
    }

    int tailResult = qMonthToLiteral(buffer + result, size, months[length - 1]);
    if (tailResult < 0)
        return tailResult;

    return result + tailResult;
}

int qDateListToStr(char *buffer, QSize size, const QDate *dates, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qDateToStr, dates, length, true, result);
    return result;
}

int qDateListToLiteral(char *buffer, QSize size, const QDate *dates, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qDateToLiteral, dates, length, true, result);
    return result;
}

int qDatetimeListToStr(char *buffer, QSize size, const QDatetime *datetimes, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qDatetimeToStr, datetimes, length, true, result);
    return result;
}

int qDatetimeListToLiteral(char *buffer, QSize size, const QDatetime *datetimes, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qDatetimeToLiteral, datetimes, length, true, result);
    return result;
}

int qTimespanListToStr(char *buffer, QSize size, const QTimespan *timespans, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimespanToStr, timespans, length, true, result);
    return result;
}

int qTimespanListToLiteral(char *buffer, QSize size, const QTimespan *timespans, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimespanToLiteral, timespans, length, true, result);
    return result;
}

int qMinuteListToStr(char *buffer, QSize size, const QMinute *minutes, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qMinuteToStr, minutes, length, true, result);
    return result;
}

int qMinuteListToLiteral(char *buffer, QSize size, const QMinute *minutes, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qMinuteToLiteral, minutes, length, true, result);
    return result;
}

int qSecondListToStr(char *buffer, QSize size, const QSecond *seconds, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qSecondToStr, seconds, length, true, result);
    return result;
}

int qSecondListToLiteral(char *buffer, QSize size, const QSecond *seconds, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qSecondToLiteral, seconds, length, true, result);
    return result;
}

int qTimeListToStr(char *buffer, QSize size, const QTime *time, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimeToStr, time, length, true, result);
    return result;
}

int qTimeListToLiteral(char *buffer, QSize size, const QTime *time, QSize length) {
    if (length == 0)
        return 0;

    int result = 0;
    WRITE_LIST(buffer, size, qTimeToLiteral, time, length, true, result);
    return result;
}

int qListToStr(
    char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision) {
    switch (type) {
        case Q_TYPE_BOOLEAN:
            return qBooleanListToStr(buffer, size, (const QBoolean *)list, length);
        case Q_TYPE_GUID:
            return qGuidListToStr(buffer, size, (const QGuid *)list, length);
        case Q_TYPE_BYTE:
            return qByteListToStr(buffer, size, (const QByte *)list, length);
        case Q_TYPE_SHORT:
            return qShortListToStr(buffer, size, (const QShort *)list, length);
        case Q_TYPE_INT:
            return qIntListToStr(buffer, size, (const QInt *)list, length);
        case Q_TYPE_LONG:
            return qLongListToStr(buffer, size, (const QLong *)list, length);
        case Q_TYPE_REAL:
            return qRealListToStr(buffer, size, (const QReal *)list, length, precision);
        case Q_TYPE_FLOAT:
            return qFloatListToStr(buffer, size, (const QFloat *)list, length, precision);
        case Q_TYPE_CHAR:
            return qCharListToStr(buffer, size, (const QChar *)list, length);
        case Q_TYPE_SYMBOL:
            return qSymbolListToStr(buffer, size, (const QSymbol *)list, length);
        case Q_TYPE_TIMESTAMP:
            return qTimestampListToStr(buffer, size, (const QTimestamp *)list, length);
        case Q_TYPE_MONTH:
            return qMonthListToStr(buffer, size, (const QMonth *)list, length);
        case Q_TYPE_DATE:
            return qDateListToStr(buffer, size, (const QDate *)list, length);
        case Q_TYPE_DATETIME:
            return qDatetimeListToStr(buffer, size, (const QDatetime *)list, length);
        case Q_TYPE_TIMESPAN:
            return qTimespanListToStr(buffer, size, (const QTimespan *)list, length);
        case Q_TYPE_MINUTE:
            return qMinuteListToStr(buffer, size, (const QMinute *)list, length);
        case Q_TYPE_SECOND:
            return qSecondListToStr(buffer, size, (const QSecond *)list, length);
        case Q_TYPE_TIME:
            return qTimeListToStr(buffer, size, (const QTime *)list, length);

        default:
            return Q_FMT_ERROR_TYPE;
    }
}

int qListToLiteral(
    char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision) {
    switch (type) {
        case Q_TYPE_BOOLEAN:
            return qBooleanListToLiteral(buffer, size, (const QBoolean *)list, length);
        case Q_TYPE_GUID:
            return qGuidListToLiteral(buffer, size, (const QGuid *)list, length);
        case Q_TYPE_BYTE:
            return qByteListToLiteral(buffer, size, (const QByte *)list, length);
        case Q_TYPE_SHORT:
            return qShortListToLiteral(buffer, size, (const QShort *)list, length);
        case Q_TYPE_INT:
            return qIntListToLiteral(buffer, size, (const QInt *)list, length);
        case Q_TYPE_LONG:
            return qLongListToLiteral(buffer, size, (const QLong *)list, length);
        case Q_TYPE_REAL:
            return qRealListToLiteral(buffer, size, (const QReal *)list, length, precision);
        case Q_TYPE_FLOAT:
            return qFloatListToLiteral(buffer, size, (const QFloat *)list, length, precision);
        case Q_TYPE_CHAR:
            return qCharListToLiteral(buffer, size, (const QChar *)list, length);
        case Q_TYPE_SYMBOL:
            return qSymbolListToLiteral(buffer, size, (const QSymbol *)list, length);
        case Q_TYPE_TIMESTAMP:
            return qTimestampListToLiteral(buffer, size, (const QTimestamp *)list, length);
        case Q_TYPE_MONTH:
            return qMonthListToLiteral(buffer, size, (const QMonth *)list, length);
        case Q_TYPE_DATE:
            return qDateListToLiteral(buffer, size, (const QDate *)list, length);
        case Q_TYPE_DATETIME:
            return qDatetimeListToLiteral(buffer, size, (const QDatetime *)list, length);
        case Q_TYPE_TIMESPAN:
            return qTimespanListToLiteral(buffer, size, (const QTimespan *)list, length);
        case Q_TYPE_MINUTE:
            return qMinuteListToLiteral(buffer, size, (const QMinute *)list, length);
        case Q_TYPE_SECOND:
            return qSecondListToLiteral(buffer, size, (const QSecond *)list, length);
        case Q_TYPE_TIME:
            return qTimeListToLiteral(buffer, size, (const QTime *)list, length);

        default:
            return Q_FMT_ERROR_TYPE;
    }
}

// TODO: list, mixed
