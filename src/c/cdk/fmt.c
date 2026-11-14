
/**
 * @file fmt.c
 * @brief String formatting.
 */

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fmt.h"
#include "internal/fmt_count.h"
#include "type.h"

// Fixed widths
#define BOOLEAN_WIDTH 1
#define GUID_WIDTH 36
#define BYTE_WIDTH 2
#define CHAR_TEXT_WIDTH 1
#define TIMESTAMP_WIDTH 29
#define MINUTE_WIDTH 5

#define SECS_PER_DAY 86400
#define SECS_PER_HOUR 3600
#define SECS_PER_MIN 60
#define MS_PER_HOUR 3600000
#define MS_PER_MIN 60000
#define MS_PER_SEC 1000
#define NS_PER_SEC 1000000000LL

#define EPOCH_DIFF 946684800LL // seconds between 1970 and 2000

#define WRITE_FIXED(buffer, size, width, fmt, ...)           \
    if (buffer == nullptr || size < (width) + 1)             \
        return Q_FMT_ERROR_SIZE;                             \
    int result = snprintf(buffer, size, fmt, ##__VA_ARGS__); \
    if (result < 0)                                          \
        return Q_FMT_ERROR_PRINTF;                           \
    return (QSize)result >= size ? Q_FMT_ERROR_SIZE : result

// Format common values once into a small scratch buffer. Oversized values use a
// second pass only after validating the caller's capacity, preserving no-write
// size failures. The scratch capacity is an optimization, not an output limit.
#define WRITE_DYNAMIC(buffer, size, fmt, ...)                               \
    if (buffer == nullptr || size == 0)                                     \
        return Q_FMT_ERROR_SIZE;                                            \
    char formatted[128];                                                    \
    int width = snprintf(formatted, sizeof(formatted), fmt, ##__VA_ARGS__); \
    if (width < 0)                                                          \
        return Q_FMT_ERROR_PRINTF;                                          \
    if ((QSize)width >= size)                                               \
        return Q_FMT_ERROR_SIZE;                                            \
    if ((QSize)width < sizeof(formatted)) {                                 \
        memcpy(buffer, formatted, (QSize)width + 1);                        \
        return width;                                                       \
    }                                                                       \
    WRITE_FIXED(buffer, size, (QSize)width, fmt, ##__VA_ARGS__)

#define WRITE_MONTH(buffer, size, month, fmt) \
    int32_t year = 2000 + month / 12;         \
    month %= 12;                              \
    if (month < 0) {                          \
        month += 12;                          \
        year -= 1;                            \
    }                                         \
    month += 1;                               \
    WRITE_DYNAMIC(buffer, size, fmt, year, month)

typedef enum {
    FMT_FINITE,
    FMT_NULL,
    FMT_POSITIVE_INFINITY,
    FMT_NEGATIVE_INFINITY,
} FmtSpecial;

static FmtSpecial integerSpecial(int64_t value, int64_t nullValue, int64_t infinity) {
    if (value == nullValue)
        return FMT_NULL;
    if (value == infinity)
        return FMT_POSITIVE_INFINITY;
    if (value == -infinity)
        return FMT_NEGATIVE_INFINITY;
    return FMT_FINITE;
}

static FmtSpecial floatingSpecial(double value) {
    if (isnan(value))
        return FMT_NULL;
    if (isinf(value))
        return signbit(value) ? FMT_NEGATIVE_INFINITY : FMT_POSITIVE_INFINITY;
    return FMT_FINITE;
}

// Called only with valid, aligned storage for the declared primitive type.
static FmtSpecial specialValue(const void *value, QTypeCode type) {
    switch (type) {
        case Q_TYPE_SHORT:
            return integerSpecial(*(const QShort *)value, Q_SHORT_NULL, Q_SHORT_INF);
        case Q_TYPE_INT:
        case Q_TYPE_MONTH:
        case Q_TYPE_DATE:
        case Q_TYPE_MINUTE:
        case Q_TYPE_SECOND:
        case Q_TYPE_TIME:
            return integerSpecial(*(const QInt *)value, Q_INT_NULL, Q_INT_INF);
        case Q_TYPE_LONG:
        case Q_TYPE_TIMESTAMP:
        case Q_TYPE_TIMESPAN:
            return integerSpecial(*(const QLong *)value, Q_LONG_NULL, Q_LONG_INF);
        case Q_TYPE_REAL:
            return floatingSpecial(*(const QReal *)value);
        case Q_TYPE_FLOAT:
        case Q_TYPE_DATETIME:
            return floatingSpecial(*(const QFloat *)value);
        default:
            return FMT_FINITE;
    }
}

static const char *specialSuffix(QTypeCode type) {
    switch (type) {
        case Q_TYPE_SHORT:
            return "h";
        case Q_TYPE_INT:
            return "i";
        case Q_TYPE_REAL:
            return "e";
        case Q_TYPE_TIMESTAMP:
            return "p";
        case Q_TYPE_MONTH:
            return "m";
        case Q_TYPE_DATE:
            return "d";
        case Q_TYPE_DATETIME:
            return "z";
        case Q_TYPE_TIMESPAN:
            return "n";
        case Q_TYPE_MINUTE:
            return "u";
        case Q_TYPE_SECOND:
            return "v";
        case Q_TYPE_TIME:
            return "t";
        default:
            return "";
    }
}

// An empty string is still a successful write requiring one byte for its terminator.
static int writeSpecial(
    char *buffer, QSize size, FmtSpecial value, QTypeCode type, bool literal, const char *suffix
) {
    const char *text;
    if (value == FMT_NULL) {
        text = literal ? (type == Q_TYPE_FLOAT ? "0n" : "0N") : "";
    } else {
        bool floating = type == Q_TYPE_REAL || type == Q_TYPE_FLOAT || type == Q_TYPE_DATETIME;
        text =
            value == FMT_NEGATIVE_INFINITY ? (floating ? "-0w" : "-0W") : (floating ? "0w" : "0W");
    }
    WRITE_DYNAMIC(buffer, size, "%s%s", text, suffix);
}

// Reject a narrowing or unsigned time_t conversion before using the calendar result.
static bool tryConvertSecondsToUtcCalendar(int64_t seconds, struct tm *calendar) {
    time_t t = (time_t)seconds;
    if ((seconds < 0 && t > 0) || (int64_t)t != seconds)
        return false;
    return gmtime_r(&t, calendar) != nullptr;
}

static bool isLeap(int64_t year) {
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

static void splitNS(
    uint64_t total, int64_t *days, int64_t *hours, int64_t *mins, int64_t *secs, int64_t *ns
) {
    // Both the quotient and remainder fit in int64_t, even for UINT64_MAX nanoseconds.
    int64_t s = (int64_t)(total / NS_PER_SEC);
    *ns = (int64_t)(total % NS_PER_SEC);
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
        return Q_FMT_ERROR_ARGUMENT;
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
    FmtSpecial special = specialValue(&short_, Q_TYPE_SHORT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_SHORT, false, "");

    WRITE_DYNAMIC(buffer, size, "%d", short_);
}

int qShortToLiteral(char *buffer, QSize size, QShort short_) {
    FmtSpecial special = specialValue(&short_, Q_TYPE_SHORT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_SHORT, true, specialSuffix(Q_TYPE_SHORT));

    WRITE_DYNAMIC(buffer, size, "%dh", short_);
}

int qIntToStr(char *buffer, QSize size, QInt int_) {
    FmtSpecial special = specialValue(&int_, Q_TYPE_INT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_INT, false, "");

    WRITE_DYNAMIC(buffer, size, "%d", int_);
}

int qIntToLiteral(char *buffer, QSize size, QInt int_) {
    FmtSpecial special = specialValue(&int_, Q_TYPE_INT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_INT, true, specialSuffix(Q_TYPE_INT));

    WRITE_DYNAMIC(buffer, size, "%di", int_);
}

int qLongToStr(char *buffer, QSize size, QLong long_) {
    FmtSpecial special = specialValue(&long_, Q_TYPE_LONG);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_LONG, false, "");

    WRITE_DYNAMIC(buffer, size, "%" PRId64, long_);
}

int qLongToLiteral(char *buffer, QSize size, QLong long_) {
    FmtSpecial special = specialValue(&long_, Q_TYPE_LONG);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_LONG, true, specialSuffix(Q_TYPE_LONG));

    return qLongToStr(buffer, size, long_);
}

// Q displays real subnormals as signed zero, including in a standalone caller
// whose floating-point environment does not flush subnormals during promotion.
static QFloat realForDisplay(QReal value) {
    if (fpclassify(value) == FP_SUBNORMAL)
        return signbit(value) ? -0.0 : 0.0;
    return (QFloat)value;
}

int qRealToStr(char *buffer, QSize size, QReal real, int precision) {
    if (precision < 0 || precision > Q_FMT_MAX_PRECISION)
        return Q_FMT_ERROR_RANGE;
    FmtSpecial special = specialValue(&real, Q_TYPE_REAL);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_REAL, false, "");

    // Q uses eight significant digits for real values when \P is zero.
    WRITE_DYNAMIC(buffer, size, "%.*g", precision == 0 ? 8 : precision, realForDisplay(real));
}

int qRealToLiteral(char *buffer, QSize size, QReal real, int precision) {
    if (precision < 0 || precision > Q_FMT_MAX_PRECISION)
        return Q_FMT_ERROR_RANGE;
    FmtSpecial special = specialValue(&real, Q_TYPE_REAL);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_REAL, true, specialSuffix(Q_TYPE_REAL));

    WRITE_DYNAMIC(buffer, size, "%.*ge", precision == 0 ? 8 : precision, realForDisplay(real));
}

int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision) {
    if (precision < 0 || precision > Q_FMT_MAX_PRECISION)
        return Q_FMT_ERROR_RANGE;
    FmtSpecial special = specialValue(&float_, Q_TYPE_FLOAT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_FLOAT, false, "");

    WRITE_DYNAMIC(buffer, size, "%.*g", precision == 0 ? 17 : precision, float_);
}

// Inspect the displayed text: rounding can make a fractional value look integral.
// A decimal separator, exponent, or special-value token already identifies a float.
static bool floatTextNeedsSuffix(const char *text) {
    if (*text == '-')
        text++;
    while (*text >= '0' && *text <= '9')
        text++;
    return *text == '\0';
}

int qFloatToLiteral(char *buffer, QSize size, QFloat float_, int precision) {
    if (precision < 0 || precision > Q_FMT_MAX_PRECISION)
        return Q_FMT_ERROR_RANGE;
    FmtSpecial special = specialValue(&float_, Q_TYPE_FLOAT);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_FLOAT, true, specialSuffix(Q_TYPE_FLOAT));

    // Stage the text so insufficient suffix capacity leaves the atom buffer untouched.
    char text[128];
    int bytes = qFloatToStr(text, sizeof(text), float_, precision);
    if (bytes < 0)
        return bytes;
    bool suffix = floatTextNeedsSuffix(text);
    QSize width = (QSize)bytes + suffix;
    if (buffer == nullptr || size <= width)
        return Q_FMT_ERROR_SIZE;
    memcpy(buffer, text, (QSize)bytes);
    if (suffix)
        buffer[bytes] = 'f';
    buffer[width] = '\0';
    return (int)width;
}

int qCharToStr(char *buffer, QSize size, QChar char_) {
    WRITE_FIXED(buffer, size, CHAR_TEXT_WIDTH, "%c", char_);
}

int qCharToLiteral(char *buffer, QSize size, QChar char_) {
    WRITE_FIXED(buffer, size, CHAR_TEXT_WIDTH + 2, "\"%c\"", char_);
}

int qSymbolToStr(char *buffer, QSize size, QSymbol symbol) {
    if (symbol == nullptr)
        return Q_FMT_ERROR_ARGUMENT;
    WRITE_DYNAMIC(buffer, size, "%s", symbol);
}

int qSymbolToLiteral(char *buffer, QSize size, QSymbol symbol) {
    if (symbol == nullptr)
        return Q_FMT_ERROR_ARGUMENT;
    WRITE_DYNAMIC(buffer, size, "`%s", symbol);
}

int qTimestampToStr(char *buffer, QSize size, QTimestamp timestamp) {
    FmtSpecial special = specialValue(&timestamp, Q_TYPE_TIMESTAMP);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_TIMESTAMP, false, "");

    int64_t sec = timestamp / NS_PER_SEC;
    int64_t ns = timestamp % NS_PER_SEC;

    if (ns < 0) {
        ns += NS_PER_SEC;
        sec--;
    }

    struct tm tm;
    if (!tryConvertSecondsToUtcCalendar(sec + EPOCH_DIFF, &tm))
        return Q_FMT_ERROR_RANGE;

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
        ns
    );
}

int qTimestampToLiteral(char *buffer, QSize size, QTimestamp timestamp) {
    FmtSpecial special = specialValue(&timestamp, Q_TYPE_TIMESTAMP);
    if (special != FMT_FINITE)
        return writeSpecial(
            buffer, size, special, Q_TYPE_TIMESTAMP, true, specialSuffix(Q_TYPE_TIMESTAMP)
        );

    return qTimestampToStr(buffer, size, timestamp);
}

int qMonthToStr(char *buffer, QSize size, QMonth month) {
    FmtSpecial special = specialValue(&month, Q_TYPE_MONTH);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_MONTH, false, "");

    WRITE_MONTH(buffer, size, month, "%04d.%02d");
}

int qMonthToLiteral(char *buffer, QSize size, QMonth month) {
    FmtSpecial special = specialValue(&month, Q_TYPE_MONTH);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_MONTH, true, specialSuffix(Q_TYPE_MONTH));

    WRITE_MONTH(buffer, size, month, "%04d.%02dm");
}

int qDateToStr(char *buffer, QSize size, QDate date) {
    FmtSpecial special = specialValue(&date, Q_TYPE_DATE);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_DATE, false, "");

    int64_t seconds = (int64_t)date * SECS_PER_DAY + EPOCH_DIFF;
    struct tm tm;
    if (!tryConvertSecondsToUtcCalendar(seconds, &tm))
        return Q_FMT_ERROR_RANGE;

    WRITE_DYNAMIC(buffer, size, "%04d.%02d.%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int qDateToLiteral(char *buffer, QSize size, QDate date) {
    FmtSpecial special = specialValue(&date, Q_TYPE_DATE);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_DATE, true, specialSuffix(Q_TYPE_DATE));

    return qDateToStr(buffer, size, date);
}

// The Gregorian calendar repeats every 400 years (146097 days). Start each cycle
// at a year divisible by 400, so conversion needs at most 399 year steps.
static void splitGregorianDays(int64_t days, int32_t *year, int32_t *month, int32_t *day) {
    int64_t cycles = days / 146097;
    days %= 146097;
    if (days < 0) {
        days += 146097;
        cycles--;
    }
    int64_t y = 2000 + cycles * 400;
    while (days >= (isLeap(y) ? 366 : 365)) {
        days -= isLeap(y) ? 366 : 365;
        y++;
    }

    const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int m = 0;
    while (true) {
        int length = monthDays[m] + (m == 1 && isLeap(y));
        if (days < length)
            break;
        days -= length;
        m++;
    }
    *year = (int32_t)y;
    *month = m + 1;
    *day = (int32_t)days + 1;
}

int qDatetimeToStr(char *buffer, QSize size, QDatetime datetime) {
    FmtSpecial special = specialValue(&datetime, Q_TYPE_DATETIME);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_DATETIME, false, "");

    // Days since 2000-01-01 spanning years INT32_MIN through INT32_MAX.
    const int64_t firstDay = -784353026790LL; // INT32_MIN-01-01
    const int64_t endDay = 784351565820LL;    // (INT32_MAX + 1)-01-01, exclusive
    const int64_t msPerDay = (int64_t)SECS_PER_DAY * MS_PER_SEC;
    if (!isfinite(datetime) || datetime < firstDay || datetime >= endDay)
        return Q_FMT_ERROR_RANGE;

    double wholeDays = floor(datetime);
    int64_t days = (int64_t)wholeDays;
    int64_t millis = (int64_t)round((datetime - wholeDays) * msPerDay);
    if (millis == msPerDay) {
        millis = 0;
        days++;
        if (days == endDay)
            return Q_FMT_ERROR_RANGE;
    }

    int32_t year, month, day;
    splitGregorianDays(days, &year, &month, &day);
    int64_t hours, mins, secs, ms;
    splitHMSMS(millis, &hours, &mins, &secs, &ms);
    WRITE_DYNAMIC(
        buffer,
        size,
        "%04" PRId32 ".%02" PRId32 ".%02" PRId32 "T%02" PRId64 ":%02" PRId64 ":%02" PRId64
        ".%03" PRId64,
        year,
        month,
        day,
        hours,
        mins,
        secs,
        ms
    );
}

int qDatetimeToLiteral(char *buffer, QSize size, QDatetime datetime) {
    FmtSpecial special = specialValue(&datetime, Q_TYPE_DATETIME);
    if (special != FMT_FINITE)
        return writeSpecial(
            buffer, size, special, Q_TYPE_DATETIME, true, specialSuffix(Q_TYPE_DATETIME)
        );

    return qDatetimeToStr(buffer, size, datetime);
}

int qTimespanToStr(char *buffer, QSize size, QTimespan timespan) {
    FmtSpecial special = specialValue(&timespan, Q_TYPE_TIMESPAN);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_TIMESPAN, false, "");

    bool neg = timespan < 0;
    // Unsigned subtraction also represents the magnitude of INT64_MIN without overflow.
    uint64_t total = neg ? UINT64_C(0) - (uint64_t)timespan : (uint64_t)timespan;

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
        ns
    );
}

int qTimespanToLiteral(char *buffer, QSize size, QTimespan timespan) {
    FmtSpecial special = specialValue(&timespan, Q_TYPE_TIMESPAN);
    if (special != FMT_FINITE)
        return writeSpecial(
            buffer, size, special, Q_TYPE_TIMESPAN, true, specialSuffix(Q_TYPE_TIMESPAN)
        );

    return qTimespanToStr(buffer, size, timespan);
}

int qMinuteToStr(char *buffer, QSize size, QMinute minute) {
    FmtSpecial special = specialValue(&minute, Q_TYPE_MINUTE);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_MINUTE, false, "");

    bool neg = minute < 0;

    int64_t total = minute;
    total = llabs(total);

    int64_t hours = total / SECS_PER_MIN;
    int64_t mins = total % SECS_PER_MIN;

    WRITE_DYNAMIC(buffer, size, "%s%02" PRId64 ":%02" PRId64, neg ? "-" : "", hours, mins);
}

int qMinuteToLiteral(char *buffer, QSize size, QMinute minute) {
    FmtSpecial special = specialValue(&minute, Q_TYPE_MINUTE);
    if (special != FMT_FINITE)
        return writeSpecial(
            buffer, size, special, Q_TYPE_MINUTE, true, specialSuffix(Q_TYPE_MINUTE)
        );

    return qMinuteToStr(buffer, size, minute);
}

int qSecondToStr(char *buffer, QSize size, QSecond second) {
    FmtSpecial special = specialValue(&second, Q_TYPE_SECOND);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_SECOND, false, "");

    bool neg = second < 0;

    int64_t total = second;
    total = llabs(total);

    int64_t hours, mins, secs;
    splitHMS(total, &hours, &mins, &secs);

    WRITE_DYNAMIC(
        buffer, size, "%s%02" PRId64 ":%02" PRId64 ":%02" PRId64, neg ? "-" : "", hours, mins, secs
    );
}

int qSecondToLiteral(char *buffer, QSize size, QSecond second) {
    FmtSpecial special = specialValue(&second, Q_TYPE_SECOND);
    if (special != FMT_FINITE)
        return writeSpecial(
            buffer, size, special, Q_TYPE_SECOND, true, specialSuffix(Q_TYPE_SECOND)
        );

    return qSecondToStr(buffer, size, second);
}

int qTimeToStr(char *buffer, QSize size, QTime time) {
    FmtSpecial special = specialValue(&time, Q_TYPE_TIME);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_TIME, false, "");

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
        ms
    );
}

int qTimeToLiteral(char *buffer, QSize size, QTime time) {
    FmtSpecial special = specialValue(&time, Q_TYPE_TIME);
    if (special != FMT_FINITE)
        return writeSpecial(buffer, size, special, Q_TYPE_TIME, true, specialSuffix(Q_TYPE_TIME));

    return qTimeToStr(buffer, size, time);
}

// Primitive type codes are contiguous except for the unused code between GUID and byte.
static bool isFormattingTypeSupported(QTypeCode type) {
    return type == Q_TYPE_BOOLEAN || type == Q_TYPE_GUID ||
           (type >= Q_TYPE_BYTE && type <= Q_TYPE_TIME);
}

int qAtomToStr(char *buffer, QSize size, const void *atom, QTypeCode type, int precision) {
    if (!isFormattingTypeSupported(type))
        return Q_FMT_ERROR_TYPE;
    if (atom == nullptr)
        return Q_FMT_ERROR_ARGUMENT;
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
    if (!isFormattingTypeSupported(type))
        return Q_FMT_ERROR_TYPE;
    if (atom == nullptr)
        return Q_FMT_ERROR_ARGUMENT;
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

typedef struct {
    char *position;
    QSize remaining;
    QSize count;
} FmtWriter;

// Advance only after validating both buffer capacity and the public output-count limit.
static int advanceWriter(FmtWriter *writer, QSize bytes) {
    if (bytes >= writer->remaining)
        return Q_FMT_ERROR_SIZE;
    if (!tryAddFormattedCount(&writer->count, bytes))
        return Q_FMT_ERROR_RANGE;
    writer->position += bytes;
    writer->remaining -= bytes;
    return 0;
}

// Prefixes, separators, and suffixes share the same checked bookkeeping.
static int appendText(FmtWriter *writer, const char *text) {
    QSize bytes = strlen(text);
    if (bytes == 0)
        return 0;
    char *destination = writer->position;
    int status = advanceWriter(writer, bytes);
    if (status < 0)
        return status;
    memcpy(destination, text, bytes + 1);
    return 0;
}

typedef int (*ItemFormatter)(char *buffer, QSize size, const void *item, int precision);

// These adapters preserve the scalar functions' typed arguments. The macro defines
// functions only; it never changes state or returns from a caller's scope.
#define DEFINE_ITEM_FORMATTER(name, expression)                                  \
    static int name(char *buffer, QSize size, const void *item, int precision) { \
        (void)precision;                                                         \
        return expression;                                                       \
    }

DEFINE_ITEM_FORMATTER(formatBooleanItem, qBooleanToStr(buffer, size, *(const QBoolean *)item))
DEFINE_ITEM_FORMATTER(formatGuidItem, qGuidToStr(buffer, size, (const QGuid *)item))
DEFINE_ITEM_FORMATTER(formatByteItem, qByteToStr(buffer, size, *(const QByte *)item))
DEFINE_ITEM_FORMATTER(formatShortItem, qShortToStr(buffer, size, *(const QShort *)item))
DEFINE_ITEM_FORMATTER(formatIntItem, qIntToStr(buffer, size, *(const QInt *)item))
DEFINE_ITEM_FORMATTER(formatLongItem, qLongToStr(buffer, size, *(const QLong *)item))
DEFINE_ITEM_FORMATTER(formatRealItem, qRealToStr(buffer, size, *(const QReal *)item, precision))
DEFINE_ITEM_FORMATTER(formatFloatItem, qFloatToStr(buffer, size, *(const QFloat *)item, precision))
DEFINE_ITEM_FORMATTER(formatCharItem, qCharToStr(buffer, size, *(const QChar *)item))
DEFINE_ITEM_FORMATTER(formatSymbolItem, qSymbolToStr(buffer, size, *(const QSymbol *)item))
DEFINE_ITEM_FORMATTER(formatTimestampItem, qTimestampToStr(buffer, size, *(const QTimestamp *)item))
DEFINE_ITEM_FORMATTER(formatMonthItem, qMonthToStr(buffer, size, *(const QMonth *)item))
DEFINE_ITEM_FORMATTER(formatDateItem, qDateToStr(buffer, size, *(const QDate *)item))
DEFINE_ITEM_FORMATTER(formatDatetimeItem, qDatetimeToStr(buffer, size, *(const QDatetime *)item))
DEFINE_ITEM_FORMATTER(formatTimespanItem, qTimespanToStr(buffer, size, *(const QTimespan *)item))
DEFINE_ITEM_FORMATTER(formatMinuteItem, qMinuteToStr(buffer, size, *(const QMinute *)item))
DEFINE_ITEM_FORMATTER(formatSecondItem, qSecondToStr(buffer, size, *(const QSecond *)item))
DEFINE_ITEM_FORMATTER(formatTimeItem, qTimeToStr(buffer, size, *(const QTime *)item))
DEFINE_ITEM_FORMATTER(
    formatSymbolLiteralItem, qSymbolToLiteral(buffer, size, *(const QSymbol *)item)
)

#undef DEFINE_ITEM_FORMATTER

static int formatListWithType(
    char *buffer,
    QSize size,
    const void *list,
    QSize length,
    QSize itemSize,
    ItemFormatter formatItem,
    int precision,
    const char *prefix,
    bool spaced,
    const char *suffix,
    QTypeCode literalType
) {
    if (length == 0)
        return 0;
    if (list == nullptr)
        return Q_FMT_ERROR_ARGUMENT;
    if (buffer == nullptr)
        return Q_FMT_ERROR_SIZE;

    if ((literalType == Q_TYPE_REAL || literalType == Q_TYPE_FLOAT) &&
        (precision < 0 || precision > Q_FMT_MAX_PRECISION))
        return Q_FMT_ERROR_RANGE;

    bool allSpecial = literalType != 0;
    bool needsFloatSuffix = literalType == Q_TYPE_FLOAT;
    FmtWriter writer = {buffer, size, 0};
    int status = appendText(&writer, prefix);
    if (status < 0)
        return status;

    const unsigned char *item = list;
    for (QSize i = 0; i < length; i++) {
        if (spaced && i != 0) {
            status = appendText(&writer, " ");
            if (status < 0)
                return status;
        }
        FmtSpecial special = literalType ? specialValue(item, literalType) : FMT_FINITE;
        allSpecial = allSpecial && special != FMT_FINITE;
        int bytes =
            special != FMT_FINITE
                ? writeSpecial(writer.position, writer.remaining, special, literalType, true, "")
                : formatItem(writer.position, writer.remaining, item, precision);
        if (bytes < 0)
            return bytes;
        // A single non-integral token identifies the entire float vector's type.
        needsFloatSuffix = needsFloatSuffix && floatTextNeedsSuffix(writer.position);
        status = advanceWriter(&writer, (QSize)bytes);
        if (status < 0)
            return status;
        if (i != length - 1)
            item += itemSize;
    }

    // A finite temporal item identifies the vector's type. An all-special vector
    // needs the suffix on its last token (month already has a fixed list suffix).
    if (allSpecial && literalType >= Q_TYPE_TIMESTAMP && suffix[0] == '\0')
        suffix = specialSuffix(literalType);
    if (needsFloatSuffix)
        suffix = "f";
    status = appendText(&writer, suffix);
    return status < 0 ? status : (int)writer.count;
}

static int formatList(
    char *buffer,
    QSize size,
    const void *list,
    QSize length,
    QSize itemSize,
    ItemFormatter formatItem,
    int precision,
    const char *prefix,
    bool spaced,
    const char *suffix
) {
    return formatListWithType(
        buffer, size, list, length, itemSize, formatItem, precision, prefix, spaced, suffix, 0
    );
}

int qBooleanListToStr(char *buffer, QSize size, const QBoolean *booleans, QSize length) {
    return formatList(
        buffer, size, booleans, length, sizeof(*booleans), formatBooleanItem, 0, "", true, ""
    );
}

int qBooleanListToLiteral(char *buffer, QSize size, const QBoolean *booleans, QSize length) {
    return formatList(
        buffer, size, booleans, length, sizeof(*booleans), formatBooleanItem, 0, "", false, "b"
    );
}

int qGuidListToStr(char *buffer, QSize size, const QGuid *guids, QSize length) {
    return formatList(buffer, size, guids, length, sizeof(*guids), formatGuidItem, 0, "", true, "");
}

int qGuidListToLiteral(char *buffer, QSize size, const QGuid *guids, QSize length) {
    return formatList(buffer, size, guids, length, sizeof(*guids), formatGuidItem, 0, "", true, "");
}

int qByteListToStr(char *buffer, QSize size, const QByte *bytes, QSize length) {
    return formatList(buffer, size, bytes, length, sizeof(*bytes), formatByteItem, 0, "", true, "");
}

int qByteListToLiteral(char *buffer, QSize size, const QByte *bytes, QSize length) {
    return formatList(
        buffer, size, bytes, length, sizeof(*bytes), formatByteItem, 0, "0x", false, ""
    );
}

int qShortListToStr(char *buffer, QSize size, const QShort *shorts, QSize length) {
    return formatList(
        buffer, size, shorts, length, sizeof(*shorts), formatShortItem, 0, "", true, ""
    );
}

int qShortListToLiteral(char *buffer, QSize size, const QShort *shorts, QSize length) {
    return formatListWithType(
        buffer,
        size,
        shorts,
        length,
        sizeof(*shorts),
        formatShortItem,
        0,
        "",
        true,
        "h",
        Q_TYPE_SHORT
    );
}

int qIntListToStr(char *buffer, QSize size, const QInt *ints, QSize length) {
    return formatList(buffer, size, ints, length, sizeof(*ints), formatIntItem, 0, "", true, "");
}

int qIntListToLiteral(char *buffer, QSize size, const QInt *ints, QSize length) {
    return formatListWithType(
        buffer, size, ints, length, sizeof(*ints), formatIntItem, 0, "", true, "i", Q_TYPE_INT
    );
}

int qLongListToStr(char *buffer, QSize size, const QLong *longs, QSize length) {
    return formatList(buffer, size, longs, length, sizeof(*longs), formatLongItem, 0, "", true, "");
}

int qLongListToLiteral(char *buffer, QSize size, const QLong *longs, QSize length) {
    return formatListWithType(
        buffer, size, longs, length, sizeof(*longs), formatLongItem, 0, "", true, "", Q_TYPE_LONG
    );
}

int qRealListToStr(char *buffer, QSize size, const QReal *reals, QSize length, int precision) {
    return formatList(
        buffer, size, reals, length, sizeof(*reals), formatRealItem, precision, "", true, ""
    );
}

int qRealListToLiteral(char *buffer, QSize size, const QReal *reals, QSize length, int precision) {
    return formatListWithType(
        buffer,
        size,
        reals,
        length,
        sizeof(*reals),
        formatRealItem,
        precision,
        "",
        true,
        "e",
        Q_TYPE_REAL
    );
}

int qFloatListToStr(char *buffer, QSize size, const QFloat *floats, QSize length, int precision) {
    return formatList(
        buffer, size, floats, length, sizeof(*floats), formatFloatItem, precision, "", true, ""
    );
}

int qFloatListToLiteral(
    char *buffer, QSize size, const QFloat *floats, QSize length, int precision
) {
    return formatListWithType(
        buffer,
        size,
        floats,
        length,
        sizeof(*floats),
        formatFloatItem,
        precision,
        "",
        true,
        "",
        Q_TYPE_FLOAT
    );
}

int qCharListToStr(char *buffer, QSize size, const QChar *chars, QSize length) {
    return formatList(buffer, size, chars, length, sizeof(*chars), formatCharItem, 0, "", true, "");
}

int qCharListToLiteral(char *buffer, QSize size, const QChar *chars, QSize length) {
    return formatList(
        buffer, size, chars, length, sizeof(*chars), formatCharItem, 0, "\"", false, "\""
    );
}

int qSymbolListToStr(char *buffer, QSize size, const QSymbol *symbols, QSize length) {
    return formatList(
        buffer, size, symbols, length, sizeof(*symbols), formatSymbolItem, 0, "", true, ""
    );
}

int qSymbolListToLiteral(char *buffer, QSize size, const QSymbol *symbols, QSize length) {
    return formatList(
        buffer, size, symbols, length, sizeof(*symbols), formatSymbolLiteralItem, 0, "", false, ""
    );
}

int qTimestampListToStr(char *buffer, QSize size, const QTimestamp *timestamps, QSize length) {
    return formatList(
        buffer, size, timestamps, length, sizeof(*timestamps), formatTimestampItem, 0, "", true, ""
    );
}

int qTimestampListToLiteral(char *buffer, QSize size, const QTimestamp *timestamps, QSize length) {
    return formatListWithType(
        buffer,
        size,
        timestamps,
        length,
        sizeof(*timestamps),
        formatTimestampItem,
        0,
        "",
        true,
        "",
        Q_TYPE_TIMESTAMP
    );
}

int qMonthListToStr(char *buffer, QSize size, const QMonth *months, QSize length) {
    return formatList(
        buffer, size, months, length, sizeof(*months), formatMonthItem, 0, "", true, ""
    );
}

int qMonthListToLiteral(char *buffer, QSize size, const QMonth *months, QSize length) {
    return formatListWithType(
        buffer,
        size,
        months,
        length,
        sizeof(*months),
        formatMonthItem,
        0,
        "",
        true,
        "m",
        Q_TYPE_MONTH
    );
}

int qDateListToStr(char *buffer, QSize size, const QDate *dates, QSize length) {
    return formatList(buffer, size, dates, length, sizeof(*dates), formatDateItem, 0, "", true, "");
}

int qDateListToLiteral(char *buffer, QSize size, const QDate *dates, QSize length) {
    return formatListWithType(
        buffer, size, dates, length, sizeof(*dates), formatDateItem, 0, "", true, "", Q_TYPE_DATE
    );
}

int qDatetimeListToStr(char *buffer, QSize size, const QDatetime *datetimes, QSize length) {
    return formatList(
        buffer, size, datetimes, length, sizeof(*datetimes), formatDatetimeItem, 0, "", true, ""
    );
}

int qDatetimeListToLiteral(char *buffer, QSize size, const QDatetime *datetimes, QSize length) {
    return formatListWithType(
        buffer,
        size,
        datetimes,
        length,
        sizeof(*datetimes),
        formatDatetimeItem,
        0,
        "",
        true,
        "",
        Q_TYPE_DATETIME
    );
}

int qTimespanListToStr(char *buffer, QSize size, const QTimespan *timespans, QSize length) {
    return formatList(
        buffer, size, timespans, length, sizeof(*timespans), formatTimespanItem, 0, "", true, ""
    );
}

int qTimespanListToLiteral(char *buffer, QSize size, const QTimespan *timespans, QSize length) {
    return formatListWithType(
        buffer,
        size,
        timespans,
        length,
        sizeof(*timespans),
        formatTimespanItem,
        0,
        "",
        true,
        "",
        Q_TYPE_TIMESPAN
    );
}

int qMinuteListToStr(char *buffer, QSize size, const QMinute *minutes, QSize length) {
    return formatList(
        buffer, size, minutes, length, sizeof(*minutes), formatMinuteItem, 0, "", true, ""
    );
}

int qMinuteListToLiteral(char *buffer, QSize size, const QMinute *minutes, QSize length) {
    return formatListWithType(
        buffer,
        size,
        minutes,
        length,
        sizeof(*minutes),
        formatMinuteItem,
        0,
        "",
        true,
        "",
        Q_TYPE_MINUTE
    );
}

int qSecondListToStr(char *buffer, QSize size, const QSecond *seconds, QSize length) {
    return formatList(
        buffer, size, seconds, length, sizeof(*seconds), formatSecondItem, 0, "", true, ""
    );
}

int qSecondListToLiteral(char *buffer, QSize size, const QSecond *seconds, QSize length) {
    return formatListWithType(
        buffer,
        size,
        seconds,
        length,
        sizeof(*seconds),
        formatSecondItem,
        0,
        "",
        true,
        "",
        Q_TYPE_SECOND
    );
}

int qTimeListToStr(char *buffer, QSize size, const QTime *time, QSize length) {
    return formatList(buffer, size, time, length, sizeof(*time), formatTimeItem, 0, "", true, "");
}

int qTimeListToLiteral(char *buffer, QSize size, const QTime *time, QSize length) {
    return formatListWithType(
        buffer, size, time, length, sizeof(*time), formatTimeItem, 0, "", true, "", Q_TYPE_TIME
    );
}

int qListToStr(
    char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision
) {
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
    char *buffer, QSize size, const void *list, QSize length, QTypeCode type, int precision
) {
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
