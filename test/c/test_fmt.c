
/**
 * @file test_fmt.c
 * @brief Unit tests for fmt.h and fmt.c.
 *
 * Q compatibility expectations use string for atoms, space-joined string for
 * lists, and .Q.s1 for literals. Expanded temporal fields are deliberate exceptions.
 * See doc/c/formatting.md (Compatibility checks) for the reference runtime and
 * comparison settings. These tests do not require Q at runtime.
 */

#include <float.h>
#include <math.h>
#include <string.h>

#include "fmt.h"
#include "internal/fmt_count.h"
#include "q.h"
#include "type.h"
#include "unity.h"

#define SIZE 128
static char buffer[SIZE];

static void assertOutputBounds(
    const unsigned char *guarded, QSize capacity, int result, const char *expected
) {
    TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
    for (QSize i = capacity + 1; i < SIZE + 2; i++) {
        TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
    }
    if (capacity <= strlen(expected)) {
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
    } else {
        TEST_ASSERT_EQUAL_INT(strlen(expected), result);
        TEST_ASSERT_EQUAL_STRING(expected, (const char *)guarded + 1);
    }
}

#define ASSERT_LIST(func, list, length, fullStr, singleStr)                               \
    int result = func(buffer, SIZE, list, length);                                        \
    TEST_ASSERT_EQUAL_INT(strlen(fullStr), result);                                       \
    TEST_ASSERT_EQUAL_STRING(fullStr, buffer);                                            \
    for (int single = 0; single <= 1; single++) {                                         \
        const char *expected = single ? singleStr : fullStr;                              \
        QSize testedLength = single ? 1 : length;                                         \
        QSize required = strlen(expected) + 1;                                            \
        TEST_ASSERT_TRUE(required <= SIZE);                                               \
        for (QSize capacity = 0; capacity <= required; capacity++) {                      \
            unsigned char guarded[SIZE + 2];                                              \
            memset(guarded, 0xA5, sizeof(guarded));                                       \
            int boundaryResult = func((char *)guarded + 1, capacity, list, testedLength); \
            assertOutputBounds(guarded, capacity, boundaryResult, expected);              \
        }                                                                                 \
    }                                                                                     \
    memset(buffer, 0xA5, SIZE);                                                           \
    result = func(buffer, SIZE, nullptr, length);                                         \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, result);                                  \
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, buffer, SIZE);                                      \
    result = func(nullptr, SIZE, list, length);                                           \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);                                      \
    result = func(buffer, 0, list, length);                                               \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result)

#define ASSERT_PRECISION_LIST(func, list, length, precision, fullStr, singleStr)    \
    int result = func(buffer, SIZE, list, length, precision);                       \
    TEST_ASSERT_EQUAL_INT(strlen(fullStr), result);                                 \
    TEST_ASSERT_EQUAL_STRING(fullStr, buffer);                                      \
    for (int single = 0; single <= 1; single++) {                                   \
        const char *expected = single ? singleStr : fullStr;                        \
        QSize testedLength = single ? 1 : length;                                   \
        QSize required = strlen(expected) + 1;                                      \
        TEST_ASSERT_TRUE(required <= SIZE);                                         \
        for (QSize capacity = 0; capacity <= required; capacity++) {                \
            unsigned char guarded[SIZE + 2];                                        \
            memset(guarded, 0xA5, sizeof(guarded));                                 \
            int boundaryResult =                                                    \
                func((char *)guarded + 1, capacity, list, testedLength, precision); \
            assertOutputBounds(guarded, capacity, boundaryResult, expected);        \
        }                                                                           \
    }                                                                               \
    memset(buffer, 0xA5, SIZE);                                                     \
    result = func(buffer, SIZE, nullptr, length, precision);                        \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, result);                            \
    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, buffer, SIZE);                                \
    result = func(nullptr, SIZE, list, length, precision);                          \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);                                \
    result = func(buffer, 0, list, length, precision);                              \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result)

static const QGuid guids[] = {
    {{
        0x12,
        0x34,
        0x56,
        0x78,
        0x9A,
        0xBC,
        0xDE,
        0xF0,
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
    }},
    {{
        0x00,
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x99,
        0xAA,
        0xBB,
        0xCC,
        0xDD,
        0xEE,
        0xFF,
    }},
    {{
        0xFF,
        0xEE,
        0xDD,
        0xCC,
        0xBB,
        0xAA,
        0x99,
        0x88,
        0x77,
        0x66,
        0x55,
        0x44,
        0x33,
        0x22,
        0x11,
        0x00,
    }},
};

static const char guidSingletonLiteral[] = ",12345678-9abc-def0-1122-334455667788";
static const char guidString[] = "12345678-9abc-def0-1122-334455667788";
static const char guidPairString[] =
    "12345678-9abc-def0-1122-334455667788 00112233-4455-6677-8899-aabbccddeeff";
static const char guidListString[] =
    "12345678-9abc-def0-1122-334455667788 00112233-4455-6677-8899-aabbccddeeff "
    "ffeeddcc-bbaa-9988-7766-554433221100";

void setUp() {
}
void tearDown() {
}

void testBooleanToStr() {
    QBoolean boolean = 1;
    int result = qBooleanToStr(buffer, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    boolean = 0;
    result = qBooleanToStr(buffer, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("0", buffer);

    result = qBooleanToStr(nullptr, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qBooleanToStr(buffer, 0, boolean);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testBooleanToLiteral() {
    QBoolean boolean = 1;
    int result = qBooleanToLiteral(buffer, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1b", buffer);

    boolean = 0;
    result = qBooleanToLiteral(buffer, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("0b", buffer);

    result = qBooleanToLiteral(nullptr, SIZE, boolean);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qBooleanToLiteral(buffer, 0, boolean);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

static void assertGuidFormatting(int (*format)(char *, QSize, const QGuid *)) {
    int result = format(buffer, SIZE, &guids[0]);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING(guidString, buffer);

    result = format(buffer, SIZE, nullptr);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, result);

    result = format(nullptr, SIZE, &guids[0]);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, &guids[0]);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testGuidToStr() {
    assertGuidFormatting(qGuidToStr);
}

void testGuidToLiteral() {
    assertGuidFormatting(qGuidToLiteral);
}

void testByteToStr() {
    QByte byte = 1;
    int result = qByteToStr(buffer, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("01", buffer);

    byte = 123;
    result = qByteToStr(buffer, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("7b", buffer);

    result = qByteToStr(nullptr, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qByteToStr(buffer, 0, byte);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testByteToLiteral() {
    QByte byte = 1;
    int result = qByteToLiteral(buffer, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("0x01", buffer);

    byte = 123;
    result = qByteToLiteral(buffer, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("0x7b", buffer);

    result = qByteToLiteral(nullptr, SIZE, byte);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qByteToLiteral(buffer, 0, byte);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testShortToStr() {
    QShort short_ = 1;
    int result = qShortToStr(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    short_ = 12345;
    result = qShortToStr(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("12345", buffer);

    // Require size >= 6
    result = qShortToStr(buffer, 5, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    short_ = -12345;
    result = qShortToStr(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("-12345", buffer);

    // Require size >= 7
    result = qShortToStr(buffer, 6, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qShortToStr(nullptr, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qShortToStr(buffer, 0, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testShortToLiteral() {
    QShort short_ = 1;
    int result = qShortToLiteral(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1h", buffer);

    short_ = 12345;
    result = qShortToLiteral(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("12345h", buffer);

    // Require size >= 7
    result = qShortToLiteral(buffer, 6, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    short_ = -12345;
    result = qShortToLiteral(buffer, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(7, result);
    TEST_ASSERT_EQUAL_STRING("-12345h", buffer);

    // Require size >= 8
    result = qShortToLiteral(buffer, 7, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qShortToLiteral(nullptr, SIZE, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qShortToLiteral(buffer, 0, short_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testIntToStr() {
    QInt int_ = 1;
    int result = qIntToStr(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    int_ = 1234567890;
    result = qIntToStr(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("1234567890", buffer);

    // Require size >= 11
    result = qIntToStr(buffer, 10, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    int_ = -1234567890;
    result = qIntToStr(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(11, result);
    TEST_ASSERT_EQUAL_STRING("-1234567890", buffer);

    // Require size >= 12
    result = qIntToStr(buffer, 11, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qIntToStr(nullptr, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qIntToStr(buffer, 0, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testIntToLiteral() {
    QInt int_ = 1;
    int result = qIntToLiteral(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1i", buffer);

    int_ = 1234567890;
    result = qIntToLiteral(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(11, result);
    TEST_ASSERT_EQUAL_STRING("1234567890i", buffer);

    // Require size >= 12
    result = qIntToLiteral(buffer, 11, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    int_ = -1234567890;
    result = qIntToLiteral(buffer, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("-1234567890i", buffer);

    // Require size >= 13
    result = qIntToLiteral(buffer, 12, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qIntToLiteral(nullptr, SIZE, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qIntToLiteral(buffer, 0, int_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

static void assertLongFormatting(int (*format)(char *, QSize, QLong)) {
    QLong long_ = 1;
    int result = format(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    long_ = 1234567890123456789;
    result = format(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(19, result);
    TEST_ASSERT_EQUAL_STRING("1234567890123456789", buffer);

    // Require size >= 20
    result = format(buffer, 19, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    long_ = -1234567890123456789;
    result = format(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(20, result);
    TEST_ASSERT_EQUAL_STRING("-1234567890123456789", buffer);

    // Require size >= 21
    result = format(buffer, 20, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(nullptr, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testLongToStr() {
    assertLongFormatting(qLongToStr);
}

void testLongToLiteral() {
    assertLongFormatting(qLongToLiteral);
}

static void assertRealFormatting(int (*format)(char *, QSize, QReal, int), bool literal) {
    const struct {
        QReal value;
        int precision;
        const char *text;
    } cases[] = {
        {3.14f, 2, "3.1"},
        {3.14f, 4, "3.14"},
        {1.25f, 10, "1.25"},
        {-3.14f, 3, "-3.14"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        char expected[32];
        snprintf(expected, sizeof(expected), "%s%s", cases[i].text, literal ? "e" : "");
        QSize width = strlen(expected);
        TEST_ASSERT_EQUAL_INT(width, format(buffer, width + 1, cases[i].value, cases[i].precision));
        TEST_ASSERT_EQUAL_STRING(expected, buffer);
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, format(buffer, width, cases[i].value, cases[i].precision)
        );
    }
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, format(nullptr, SIZE, 1.25f, Q_FMT_DEFAULT_PRECISION));
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, format(buffer, 0, 1.25f, Q_FMT_DEFAULT_PRECISION));
}

void testRealToStr() {
    assertRealFormatting(qRealToStr, false);
}

void testRealToLiteral() {
    assertRealFormatting(qRealToLiteral, true);
}

static void assertFloatFormatting(int (*format)(char *, QSize, QFloat, int)) {
    const struct {
        QFloat value;
        int precision;
        const char *text;
    } cases[] = {
        {3.14, 2, "3.1"},
        {3.14, 4, "3.14"},
        {1.25, 17, "1.25"},
        {-3.14, 3, "-3.14"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        QSize width = strlen(cases[i].text);
        TEST_ASSERT_EQUAL_INT(width, format(buffer, width + 1, cases[i].value, cases[i].precision));
        TEST_ASSERT_EQUAL_STRING(cases[i].text, buffer);
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, format(buffer, width, cases[i].value, cases[i].precision)
        );
    }
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, format(nullptr, SIZE, 1.25, Q_FMT_DEFAULT_PRECISION));
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, format(buffer, 0, 1.25, Q_FMT_DEFAULT_PRECISION));
}

void testFloatToStr() {
    assertFloatFormatting(qFloatToStr);
}

void testFloatToLiteral() {
    assertFloatFormatting(qFloatToLiteral);
}

void testCharToStr() {
    QChar char_ = 'a';
    int result = qCharToStr(buffer, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("a", buffer);

    char_ = '\n';
    result = qCharToStr(buffer, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("\n", buffer);

    result = qCharToStr(nullptr, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qCharToStr(buffer, 0, char_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testCharToLiteral() {
    QChar char_ = 'a';
    int result = qCharToLiteral(buffer, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_STRING("\"a\"", buffer);

    char_ = '\n';
    result = qCharToLiteral(buffer, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("\"\\n\"", buffer);

    result = qCharToLiteral(nullptr, SIZE, char_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qCharToLiteral(buffer, 0, char_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testSymbolToStr() {
    QSymbol symbol = "abc";
    int result = qSymbolToStr(buffer, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_STRING("abc", buffer);

    symbol = "";
    result = qSymbolToStr(buffer, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_STRING("", buffer);

    result = qSymbolToStr(nullptr, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qSymbolToStr(buffer, 0, symbol);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testSymbolToLiteral() {
    QSymbol symbol = "abc";
    int result = qSymbolToLiteral(buffer, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("`abc", buffer);

    symbol = "";
    result = qSymbolToLiteral(buffer, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("`", buffer);

    result = qSymbolToLiteral(nullptr, SIZE, symbol);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qSymbolToLiteral(buffer, 0, symbol);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

static void assertTimestampFormatting(int (*format)(char *, QSize, QTimestamp)) {
    QTimestamp timestamp = 821977817123456789;
    int result = format(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    // Require size >= 30
    result = format(buffer, 29, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timestamp = -124793382876543211;
    result = format(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17D15:10:17.123456789", buffer);

    result = format(nullptr, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimestampToStr() {
    assertTimestampFormatting(qTimestampToStr);
}

void testTimestampToLiteral() {
    assertTimestampFormatting(qTimestampToLiteral);
}

void testMonthToStr() {
    QMonth month = 312;
    int result = qMonthToStr(buffer, SIZE, month);
    TEST_ASSERT_EQUAL_INT(7, result);
    TEST_ASSERT_EQUAL_STRING("2026.01", buffer);

    // Require size >= 8
    result = qMonthToStr(buffer, 7, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    month = -48;
    result = qMonthToStr(buffer, SIZE, month);
    TEST_ASSERT_EQUAL_INT(7, result);
    TEST_ASSERT_EQUAL_STRING("1996.01", buffer);

    result = qMonthToStr(nullptr, SIZE, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qMonthToStr(buffer, 0, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testMonthToLiteral() {
    QMonth month = 312;
    int result = qMonthToLiteral(buffer, SIZE, month);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("2026.01m", buffer);

    // Require size >= 9
    result = qMonthToLiteral(buffer, 8, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    month = -48;
    result = qMonthToLiteral(buffer, SIZE, month);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("1996.01m", buffer);

    result = qMonthToLiteral(nullptr, SIZE, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qMonthToLiteral(buffer, 0, month);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

static void assertDateFormatting(int (*format)(char *, QSize, QDate)) {
    QDate date = 9513;
    int result = format(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    // Require size >= 11
    result = format(buffer, 10, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    date = -1445;
    result = format(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17", buffer);

    result = format(nullptr, SIZE, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testDateToStr() {
    assertDateFormatting(qDateToStr);
}

void testDateToLiteral() {
    assertDateFormatting(qDateToLiteral);
}

static void assertDatetimeFormatting(int (*format)(char *, QSize, QDatetime)) {
    QDatetime datetime = 9513.6321426273153;
    int result = format(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    // Require size >= 24
    result = format(buffer, 23, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    datetime = -1444.3678573726852;
    result = format(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17T15:10:17.123", buffer);

    result = format(nullptr, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testDatetimeToStr() {
    assertDatetimeFormatting(qDatetimeToStr);
}

void testDatetimeToLiteral() {
    assertDatetimeFormatting(qDatetimeToLiteral);
}

void testDatetimeInvalidRange() {
    const QDatetime values[] = {
        DBL_MAX,
        -DBL_MAX,
        784351565820.0,
        nextafter(-784353026790.0, -INFINITY),
    };
    for (QSize i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        strcpy(buffer, "unchanged");
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qDatetimeToStr(buffer, SIZE, values[i]));
        TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qDatetimeToLiteral(buffer, SIZE, values[i]));
        TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
        const QDatetime list[] = {0, values[i]};
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qDatetimeListToStr(buffer, SIZE, list, 2));
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qDatetimeListToLiteral(buffer, SIZE, list, 2));
    }
}

void testDatetimeCalendarAndDisplay() {
    const double msPerDay = 86400000.0;
    const struct {
        QDatetime value;
        const char *expected;
    } cases[] = {
        {-784353026790.0, "-2147483648.01.01T00:00:00.000"},
        {784351565819.0, "2147483647.12.31T00:00:00.000"},
        {-784353026790.0 + 0.5, "-2147483648.01.01T12:00:00.000"},
        {nextafter(-784353026790.0, INFINITY), "-2147483648.01.01T00:00:10.546"},
        {nextafter(784351565820.0, -INFINITY), "2147483647.12.31T23:59:49.453"},
        {0.0, "2000.01.01T00:00:00.000"},
        {-0.0, "2000.01.01T00:00:00.000"},
        {0.0004 / 86400.0, "2000.01.01T00:00:00.000"},
        {0.0006 / 86400.0, "2000.01.01T00:00:00.000"},
        {-0.0004 / 86400.0, "1999.12.31T23:59:59.999"},
        {-0.0006 / 86400.0, "1999.12.31T23:59:59.999"},
        {0.5 / msPerDay, "2000.01.01T00:00:00.000"},
        {59999.6 / msPerDay, "2000.01.01T00:00:59.999"},
        {3599999.6 / msPerDay, "2000.01.01T00:59:59.999"},
        {1.0 - 0.4 / msPerDay, "2000.01.01T23:59:59.999"},
        {31.0 - 0.4 / msPerDay, "2000.01.31T23:59:59.999"},
        {59.0 - 0.4 / msPerDay, "2000.02.28T23:59:59.999"},
        {60.0 - 0.4 / msPerDay, "2000.02.29T23:59:59.999"},
        {366.0 - 0.4 / msPerDay, "2000.12.31T23:59:59.999"},
        {-36466.0, "1900.02.28T00:00:00.000"},
        {-36465.0, "1900.03.01T00:00:00.000"},
        {36583.0, "2100.02.28T00:00:00.000"},
        {36584.0, "2100.03.01T00:00:00.000"},
        {146156.0, "2400.02.29T00:00:00.000"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        QSize required = strlen(cases[i].expected) + 1;
        int result = qDatetimeToStr(buffer, required, cases[i].value);
        TEST_ASSERT_EQUAL_INT(required - 1, result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        result = qDatetimeToLiteral(buffer, required, cases[i].value);
        TEST_ASSERT_EQUAL_INT(required - 1, result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, qDatetimeToStr(buffer, required - 1, cases[i].value)
        );
    }
}

void testDatetimeMicrosecondBoundaries() {
    const double msPerDay = 86400000.0;
    const double microsPerDay = 86400000000.0;
    // Golden outputs from Q. Include both sides of microsecond ties, epoch and
    // calendar boundaries, and values where splitting the day first loses fidelity.
    const struct {
        QDatetime value;
        const char *expected;
    } cases[] = {
        {0.0004 / msPerDay, "2000.01.01T00:00:00.000"},
        {0.0005 / msPerDay, "2000.01.01T00:00:00.000"},
        {0.0006 / msPerDay, "2000.01.01T00:00:00.000"},
        {-0.0004 / msPerDay, "2000.01.01T00:00:00.000"},
        {-0.0005 / msPerDay, "1999.12.31T23:59:59.999"},
        {-0.0006 / msPerDay, "1999.12.31T23:59:59.999"},
        {0.9994 / msPerDay, "2000.01.01T00:00:00.000"},
        {0.9995 / msPerDay, "2000.01.01T00:00:00.001"},
        {0.9996 / msPerDay, "2000.01.01T00:00:00.001"},
        {-0.9994 / msPerDay, "1999.12.31T23:59:59.999"},
        {-0.9995 / msPerDay, "1999.12.31T23:59:59.999"},
        {-0.9996 / msPerDay, "1999.12.31T23:59:59.999"},
        {1.0004 / msPerDay, "2000.01.01T00:00:00.001"},
        {1.0005 / msPerDay, "2000.01.01T00:00:00.001"},
        {1.0006 / msPerDay, "2000.01.01T00:00:00.001"},
        {-1.0004 / msPerDay, "1999.12.31T23:59:59.999"},
        {-1.0005 / msPerDay, "1999.12.31T23:59:59.998"},
        {-1.0006 / msPerDay, "1999.12.31T23:59:59.998"},
        {1.0 - 0.4 / microsPerDay, "2000.01.02T00:00:00.000"},
        {1.0 - 0.6 / microsPerDay, "2000.01.01T23:59:59.999"},
        {31.0 - 0.4 / microsPerDay, "2000.02.01T00:00:00.000"},
        {31.0 - 0.6 / microsPerDay, "2000.01.31T23:59:59.999"},
        {59.0 - 0.4 / microsPerDay, "2000.02.29T00:00:00.000"},
        {59.0 - 0.6 / microsPerDay, "2000.02.28T23:59:59.999"},
        {60.0 - 0.4 / microsPerDay, "2000.03.01T00:00:00.000"},
        {60.0 - 0.6 / microsPerDay, "2000.02.29T23:59:59.999"},
        {366.0 - 0.4 / microsPerDay, "2001.01.01T00:00:00.000"},
        {366.0 - 0.6 / microsPerDay, "2000.12.31T23:59:59.999"},
        {-1.0 - 0.4 / microsPerDay, "1999.12.31T00:00:00.000"},
        {-1.0 - 0.6 / microsPerDay, "1999.12.30T23:59:59.999"},
        {nextafter(1.0 / msPerDay, -INFINITY), "2000.01.01T00:00:00.001"},
        {nextafter(-1.0, -INFINITY), "1999.12.31T00:00:00.000"},
        {9814.041666666664, "2026.11.14T01:00:00.000"},
        {2084645.8630976505, "7707.07.26T20:42:51.636"},
        {DBL_TRUE_MIN, "2000.01.01T00:00:00.000"},
        {-DBL_TRUE_MIN, "2000.01.01T00:00:00.000"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        for (int literal = 0; literal <= 1; literal++) {
            for (QSize capacity = 0; capacity <= strlen(cases[i].expected) + 1; capacity++) {
                unsigned char guarded[SIZE + 2];
                memset(guarded, 0xA5, sizeof(guarded));
                int result = (literal ? qAtomToLiteral : qAtomToStr)(
                    (char *)guarded + 1, capacity, &cases[i].value, Q_TYPE_DATETIME, 0
                );
                assertOutputBounds(guarded, capacity, result, cases[i].expected);
                if (result < 0)
                    TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, guarded, sizeof(guarded));
            }
        }
        QDatetime values[] = {cases[i].value, 0};
        char fullText[SIZE];
        snprintf(fullText, sizeof(fullText), "%s 2000.01.01T00:00:00.000", cases[i].expected);
        {
            ASSERT_LIST(qDatetimeListToStr, values, 2, fullText, cases[i].expected);
        }
        char singleton[32];
        snprintf(singleton, sizeof(singleton), ",%s", cases[i].expected);
        {
            ASSERT_LIST(qDatetimeListToLiteral, values, 2, fullText, singleton);
        }
    }
}

static void assertTimespanFormatting(int (*format)(char *, QSize, QTimespan)) {
    QTimespan timespan = 1473576123456789;
    int result = format(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(21, result);
    TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    // Require size >= 22
    result = format(buffer, 21, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timespan = -1473576123456789;
    result = format(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(22, result);
    TEST_ASSERT_EQUAL_STRING("-17D01:19:36.123456789", buffer);

    result = format(nullptr, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimespanToStr() {
    assertTimespanFormatting(qTimespanToStr);
}

void testTimespanToLiteral() {
    assertTimespanFormatting(qTimespanToLiteral);
}

void testTimespanExtremes() {
    const struct {
        QTimespan value;
        const char *expected;
    } cases[] = {
        {INT64_MIN + 2, "-106751D23:47:16.854775806"},
        {INT64_MAX - 1, "106751D23:47:16.854775806"},
        {-1, "-0D00:00:00.000000001"},
    };
    int (*formatters[])(char *, QSize, QTimespan) = {qTimespanToStr, qTimespanToLiteral};

    for (QSize f = 0; f < sizeof(formatters) / sizeof(formatters[0]); f++) {
        for (QSize c = 0; c < sizeof(cases) / sizeof(cases[0]); c++) {
            QSize required = strlen(cases[c].expected) + 1;
            for (QSize capacity = 0; capacity <= required; capacity++) {
                unsigned char guarded[SIZE];
                memset(guarded, 0xA5, sizeof(guarded));
                char *destination = (char *)&guarded[1];
                int result = formatters[f](destination, capacity, cases[c].value);

                TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
                for (QSize i = capacity + 1; i < sizeof(guarded); i++) {
                    TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
                }
                if (capacity < required) {
                    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
                } else {
                    TEST_ASSERT_EQUAL_INT(required - 1, result);
                    TEST_ASSERT_EQUAL_STRING(cases[c].expected, destination);
                }
            }
        }
    }
}

static void assertMinuteFormatting(int (*format)(char *, QSize, QMinute)) {
    QMinute minute = 980;
    int result = format(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    // Require size >= 6
    result = format(buffer, 5, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    minute = -980;
    result = format(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("-16:20", buffer);

    minute = 6000;
    result = format(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("100:00", buffer);

    result = format(nullptr, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testMinuteToStr() {
    assertMinuteFormatting(qMinuteToStr);
}

void testMinuteToLiteral() {
    assertMinuteFormatting(qMinuteToLiteral);
}

static void assertSecondFormatting(int (*format)(char *, QSize, QSecond)) {
    QSecond second = 58817;
    int result = format(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    // Require size >= 9
    result = format(buffer, 8, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    second = -58817;
    result = format(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17", buffer);

    second = 360000;
    result = format(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00", buffer);

    result = format(nullptr, SIZE, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testSecondToStr() {
    assertSecondFormatting(qSecondToStr);
}

void testSecondToLiteral() {
    assertSecondFormatting(qSecondToLiteral);
}

static void assertTimeFormatting(int (*format)(char *, QSize, QTime)) {
    QTime time = 58817123;
    int result = format(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);

    // Require size >= 13
    result = format(buffer, 12, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    time = -58817123;
    result = format(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17.123", buffer);

    time = 360000000;
    result = format(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00.000", buffer);

    result = format(nullptr, SIZE, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = format(buffer, 0, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimeToStr() {
    assertTimeFormatting(qTimeToStr);
}

void testTimeToLiteral() {
    assertTimeFormatting(qTimeToLiteral);
}

void testAtomToStr() {
    int result;

    QBoolean boolean = 1;
    result = qAtomToStr(buffer, SIZE, &boolean, Q_TYPE_BOOLEAN, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    result = qAtomToStr(buffer, SIZE, &guids[0], Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING(guidString, buffer);

    QByte byte = 1;
    result = qAtomToStr(buffer, SIZE, &byte, Q_TYPE_BYTE, 0);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("01", buffer);

    QShort short_ = 1;
    result = qAtomToStr(buffer, SIZE, &short_, Q_TYPE_SHORT, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    QInt int_ = 1;
    result = qAtomToStr(buffer, SIZE, &int_, Q_TYPE_INT, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    QLong long_ = 1;
    result = qAtomToStr(buffer, SIZE, &long_, Q_TYPE_LONG, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    QReal real = 3.14159;
    result = qAtomToStr(buffer, SIZE, &real, Q_TYPE_REAL, 3);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer); // Precision rounding

    QFloat float_ = 3.14159;
    result = qAtomToStr(buffer, SIZE, &float_, Q_TYPE_FLOAT, 3);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer);

    QChar char_ = 'a';
    result = qAtomToStr(buffer, SIZE, &char_, Q_TYPE_CHAR, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("a", buffer);

    QSymbol symbol = "abc";
    result = qAtomToStr(buffer, SIZE, symbol, Q_TYPE_SYMBOL, 0);
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_STRING("abc", buffer);

    QTimestamp timestamp = 821977817123456789;
    result = qAtomToStr(buffer, SIZE, &timestamp, Q_TYPE_TIMESTAMP, 0);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    QMonth month = 312;
    result = qAtomToStr(buffer, SIZE, &month, Q_TYPE_MONTH, 0);
    TEST_ASSERT_EQUAL_INT(7, result);
    TEST_ASSERT_EQUAL_STRING("2026.01", buffer);

    QDate date = 9513;
    result = qAtomToStr(buffer, SIZE, &date, Q_TYPE_DATE, 0);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    QDatetime datetime = 9513.6321426273153;
    result = qAtomToStr(buffer, SIZE, &datetime, Q_TYPE_DATETIME, 0);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    QTimespan timespan = 1473576123456789;
    result = qAtomToStr(buffer, SIZE, &timespan, Q_TYPE_TIMESPAN, 0);
    TEST_ASSERT_EQUAL_INT(21, result);
    TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    QMinute minute = 980;
    result = qAtomToStr(buffer, SIZE, &minute, Q_TYPE_MINUTE, 0);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    QSecond second = 58817;
    result = qAtomToStr(buffer, SIZE, &second, Q_TYPE_SECOND, 0);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    QTime time = 58817123;
    result = qAtomToStr(buffer, SIZE, &time, Q_TYPE_TIME, 0);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);
}

void testAtomToLiteral() {
    int result;

    QBoolean boolean = 1;
    result = qAtomToLiteral(buffer, SIZE, &boolean, Q_TYPE_BOOLEAN, 0);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1b", buffer);

    result = qAtomToLiteral(buffer, SIZE, &guids[0], Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING(guidString, buffer);

    QByte byte = 1;
    result = qAtomToLiteral(buffer, SIZE, &byte, Q_TYPE_BYTE, 0);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("0x01", buffer);

    QShort short_ = 1;
    result = qAtomToLiteral(buffer, SIZE, &short_, Q_TYPE_SHORT, 0);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1h", buffer);

    QInt int_ = 1;
    result = qAtomToLiteral(buffer, SIZE, &int_, Q_TYPE_INT, 0);
    TEST_ASSERT_EQUAL_INT(2, result);
    TEST_ASSERT_EQUAL_STRING("1i", buffer);

    QLong long_ = 1;
    result = qAtomToLiteral(buffer, SIZE, &long_, Q_TYPE_LONG, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    QReal real = 3.14159;
    result = qAtomToLiteral(buffer, SIZE, &real, Q_TYPE_REAL, 3);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("3.14e", buffer);

    QFloat float_ = 3.14159;
    result = qAtomToLiteral(buffer, SIZE, &float_, Q_TYPE_FLOAT, 3);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer);

    QChar char_ = 'a';
    result = qAtomToLiteral(buffer, SIZE, &char_, Q_TYPE_CHAR, 0);
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_STRING("\"a\"", buffer);

    QSymbol symbol = "abc";
    result = qAtomToLiteral(buffer, SIZE, symbol, Q_TYPE_SYMBOL, 0);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("`abc", buffer);

    QTimestamp timestamp = 821977817123456789;
    result = qAtomToLiteral(buffer, SIZE, &timestamp, Q_TYPE_TIMESTAMP, 0);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    QMonth month = 312;
    result = qAtomToLiteral(buffer, SIZE, &month, Q_TYPE_MONTH, 0);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("2026.01m", buffer);

    QDate date = 9513;
    result = qAtomToLiteral(buffer, SIZE, &date, Q_TYPE_DATE, 0);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    QDatetime datetime = 9513.6321426273153;
    result = qAtomToLiteral(buffer, SIZE, &datetime, Q_TYPE_DATETIME, 0);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    QTimespan timespan = 1473576123456789;
    result = qAtomToLiteral(buffer, SIZE, &timespan, Q_TYPE_TIMESPAN, 0);
    TEST_ASSERT_EQUAL_INT(21, result);
    TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    QMinute minute = 980;
    result = qAtomToLiteral(buffer, SIZE, &minute, Q_TYPE_MINUTE, 0);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    QSecond second = 58817;
    result = qAtomToLiteral(buffer, SIZE, &second, Q_TYPE_SECOND, 0);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    QTime time = 58817123;
    result = qAtomToLiteral(buffer, SIZE, &time, Q_TYPE_TIME, 0);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);
}

void testBooleanListToStr() {
    QBoolean booleans[] = {1, 0, 1};
    ASSERT_LIST(qBooleanListToStr, booleans, 3, "1 0 1", "1");
}

void testBooleanListToLiteral() {
    QBoolean booleans[] = {1, 0, 1};
    ASSERT_LIST(qBooleanListToLiteral, booleans, 3, "101b", ",1b");
}

void testGuidListToStr() {
    ASSERT_LIST(
        qGuidListToStr,
        guids,
        3,
        guidListString,
        guidString
    );
}

void testGuidListToLiteral() {
    ASSERT_LIST(qGuidListToLiteral, guids, 3, guidListString, guidSingletonLiteral);
}

void testByteListToStr() {
    QByte bytes[] = {1, 123, 255};
    ASSERT_LIST(qByteListToStr, bytes, 3, "01 7b ff", "01");
}

void testByteListToLiteral() {
    QByte bytes[] = {1, 123, 255};
    ASSERT_LIST(qByteListToLiteral, bytes, 3, "0x017bff", ",0x01");
}

void testByteListToLiteralBufferBounds() {
    const QByte bytes[] = {1, 123, 255};
    const char *expected[] = {",0x01", "0x017b", "0x017bff"};

    for (QSize length = 1; length <= 3; length++) {
        QSize required = strlen(expected[length - 1]) + 1;
        for (QSize capacity = 0; capacity <= required; capacity++) {
            unsigned char guarded[16];
            memset(guarded, 0xA5, sizeof(guarded));
            char *destination = (char *)&guarded[1];

            int result = qByteListToLiteral(destination, capacity, bytes, length);

            TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
            for (QSize i = capacity + 1; i < sizeof(guarded); i++) {
                TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
            }

            if (capacity < required) {
                TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
            } else {
                TEST_ASSERT_EQUAL_INT(required - 1, result);
                TEST_ASSERT_EQUAL_STRING(expected[length - 1], destination);
            }
        }
    }
}

void testShortListToStr() {
    QShort shorts[] = {1, -234, 567};
    ASSERT_LIST(qShortListToStr, shorts, 3, "1 -234 567", "1");
}

void testShortListToLiteral() {
    QShort shorts[] = {1, -234, 567};
    ASSERT_LIST(qShortListToLiteral, shorts, 3, "1 -234 567h", ",1h");
}

void testIntListToStr() {
    QInt ints[] = {1, -234, 567};
    ASSERT_LIST(qIntListToStr, ints, 3, "1 -234 567", "1");
}

void testIntListToLiteral() {
    QInt ints[] = {1, -234, 567};
    ASSERT_LIST(qIntListToLiteral, ints, 3, "1 -234 567i", ",1i");
}

void testLongListToStr() {
    QLong longs[] = {1, -234, 567};
    ASSERT_LIST(qLongListToStr, longs, 3, "1 -234 567", "1");
}

void testLongListToLiteral() {
    QLong longs[] = {1, -234, 567};
    ASSERT_LIST(qLongListToLiteral, longs, 3, "1 -234 567", ",1");
}

void testRealListToStr() {
    QReal reals[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISION_LIST(qRealListToStr, reals, 3, 2, "3.1 -3.1 0.16", "3.1");
}

void testRealListToLiteral() {
    QReal reals[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISION_LIST(qRealListToLiteral, reals, 3, 2, "3.1 -3.1 0.16e", ",3.1e");
}

void testFloatListToStr() {
    QFloat floats[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISION_LIST(qFloatListToStr, floats, 3, 2, "3.1 -3.1 0.16", "3.1");
}

void testFloatListToLiteral() {
    QFloat floats[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISION_LIST(qFloatListToLiteral, floats, 3, 2, "3.1 -3.1 0.16", ",3.1");
}

void testCharListToStr() {
    QChar chars[] = {'a', 'b', 'c'};
    ASSERT_LIST(qCharListToStr, chars, 3, "a b c", "a");
}

void testCharListToLiteral() {
    QChar chars[] = {'a', 'b', 'c'};
    ASSERT_LIST(qCharListToLiteral, chars, 3, "\"abc\"", ",\"a\"");
}

void testCharListToLiteralBufferBounds() {
    const QChar chars[] = {'a', 'b', 'c'};
    const char *expected[] = {",\"a\"", "\"ab\"", "\"abc\""};

    for (QSize length = 1; length <= 3; length++) {
        QSize required = strlen(expected[length - 1]) + 1;
        for (QSize capacity = 0; capacity <= required; capacity++) {
            unsigned char guarded[16];
            memset(guarded, 0xA5, sizeof(guarded));
            char *destination = (char *)&guarded[1];

            int result = qCharListToLiteral(destination, capacity, chars, length);

            TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
            for (QSize i = capacity + 1; i < sizeof(guarded); i++) {
                TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
            }

            if (capacity < required) {
                TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
            } else {
                TEST_ASSERT_EQUAL_INT(required - 1, result);
                TEST_ASSERT_EQUAL_STRING(expected[length - 1], destination);
            }
        }
    }
}

void testSymbolListToStr() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    ASSERT_LIST(qSymbolListToStr, symbols, 3, "abc def ghi", "abc");
}

void testSymbolListToLiteral() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    ASSERT_LIST(qSymbolListToLiteral, symbols, 3, "`abc`def`ghi", ",`abc");
}

void testTimestampListToStr() {
    QTimestamp timestamps[] = {821977817123456789, 0, -124793382876543211};
    ASSERT_LIST(
        qTimestampListToStr,
        timestamps,
        3,
        "2026.01.17D15:10:17.123456789 2000.01.01D00:00:00.000000000 1996.01.17D15:10:17.123456789",
        "2026.01.17D15:10:17.123456789"
    );
}

void testTimestampListToLiteral() {
    QTimestamp timestamps[] = {821977817123456789, 0, -124793382876543211};
    ASSERT_LIST(
        qTimestampListToLiteral,
        timestamps,
        3,
        "2026.01.17D15:10:17.123456789 2000.01.01D00:00:00.000000000 1996.01.17D15:10:17.123456789",
        ",2026.01.17D15:10:17.123456789"
    );
}

void testMonthListToStr() {
    QMonth months[] = {312, 0, -48};
    ASSERT_LIST(qMonthListToStr, months, 3, "2026.01 2000.01 1996.01", "2026.01");
}

void testMonthListToLiteral() {
    QMonth months[] = {312, 0, -48};
    ASSERT_LIST(qMonthListToLiteral, months, 3, "2026.01 2000.01 1996.01m", ",2026.01m");
}

void testDateListToStr() {
    QDate dates[] = {9513, 0, -1445};
    ASSERT_LIST(qDateListToStr, dates, 3, "2026.01.17 2000.01.01 1996.01.17", "2026.01.17");
}

void testDateListToLiteral() {
    QDate dates[] = {9513, 0, -1445};
    ASSERT_LIST(qDateListToLiteral, dates, 3, "2026.01.17 2000.01.01 1996.01.17", ",2026.01.17");
}

void testDatetimeListToStr() {
    QDatetime datetimes[] = {9513.6321426273153, 0, -1444.3678573726852};
    ASSERT_LIST(
        qDatetimeListToStr,
        datetimes,
        3,
        "2026.01.17T15:10:17.123 2000.01.01T00:00:00.000 1996.01.17T15:10:17.123",
        "2026.01.17T15:10:17.123"
    );
}

void testDatetimeListToLiteral() {
    QDatetime datetimes[] = {9513.6321426273153, 0, -1444.3678573726852};
    ASSERT_LIST(
        qDatetimeListToLiteral,
        datetimes,
        3,
        "2026.01.17T15:10:17.123 2000.01.01T00:00:00.000 1996.01.17T15:10:17.123",
        ",2026.01.17T15:10:17.123"
    );
}

void testTimespanListToStr() {
    QTimespan timespans[] = {1473576123456789, 0, -1473576123456789};
    ASSERT_LIST(
        qTimespanListToStr,
        timespans,
        3,
        "17D01:19:36.123456789 0D00:00:00.000000000 -17D01:19:36.123456789",
        "17D01:19:36.123456789"
    );
}

void testTimespanListToLiteral() {
    QTimespan timespans[] = {1473576123456789, 0, -1473576123456789};
    ASSERT_LIST(
        qTimespanListToLiteral,
        timespans,
        3,
        "17D01:19:36.123456789 0D00:00:00.000000000 -17D01:19:36.123456789",
        ",17D01:19:36.123456789"
    );
}

void testTimespanListExtremes() {
    const QTimespan values[] = {INT64_MIN + 2, INT64_MAX - 1};
    const char *expected = "-106751D23:47:16.854775806 "
                           "106751D23:47:16.854775806";
    int result = qTimespanListToStr(buffer, SIZE, values, 2);
    TEST_ASSERT_EQUAL_INT(strlen(expected), result);
    TEST_ASSERT_EQUAL_STRING(expected, buffer);
    result = qTimespanListToLiteral(buffer, SIZE, values, 2);
    TEST_ASSERT_EQUAL_INT(strlen(expected), result);
    TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

void testMinuteListToStr() {
    QMinute minutes[] = {980, 0, -980};
    ASSERT_LIST(qMinuteListToStr, minutes, 3, "16:20 00:00 -16:20", "16:20");
}

void testMinuteListToLiteral() {
    QMinute minutes[] = {980, 0, -980};
    ASSERT_LIST(qMinuteListToLiteral, minutes, 3, "16:20 00:00 -16:20", ",16:20");
}

void testSecondListToStr() {
    QSecond seconds[] = {58817, 0, -58817};
    ASSERT_LIST(qSecondListToStr, seconds, 3, "16:20:17 00:00:00 -16:20:17", "16:20:17");
}

void testSecondListToLiteral() {
    QSecond seconds[] = {58817, 0, -58817};
    ASSERT_LIST(qSecondListToLiteral, seconds, 3, "16:20:17 00:00:00 -16:20:17", ",16:20:17");
}

void testTimeListToStr() {
    QTime times[] = {58817123, 0, -58817123};
    ASSERT_LIST(
        qTimeListToStr, times, 3, "16:20:17.123 00:00:00.000 -16:20:17.123", "16:20:17.123"
    );
}

void testTimeListToLiteral() {
    QTime times[] = {58817123, 0, -58817123};
    ASSERT_LIST(
        qTimeListToLiteral, times, 3, "16:20:17.123 00:00:00.000 -16:20:17.123", ",16:20:17.123"
    );
}

void testGuidListFromQObject() {
    // The items of a Q guid list are stored contiguously and can be formatted directly
    QObj *list = qNewGuidList(guids, 2);
    TEST_ASSERT_NOT_NULL(list);

    int result = qListToStr(buffer, SIZE, list->list, list->length, Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(73, result);
    TEST_ASSERT_EQUAL_STRING(guidPairString, buffer);

    result = qGuidListToLiteral(buffer, SIZE, (const QGuid *)list->list, list->length);
    TEST_ASSERT_EQUAL_INT(73, result);

    TEST_ASSERT_EQUAL_STRING(guidPairString, buffer);
    qDecRef(list);
}

static void assertGenericListFormatting(bool literal) {
    const QBoolean booleans[] = {1, 0};
    const QByte bytes[] = {1, 255};
    const QShort shorts[] = {-123, 456};
    const QInt ints[] = {-123456, 789012};
    const QLong longs[] = {-1234567890123LL, 4567890123456LL};
    const QReal reals[] = {1.25f, -2.5f};
    const QFloat floats[] = {1.25, -2.5};
    const QChar chars[] = {'a', 'z'};
    const QSymbol symbols[] = {"abc", "xyz"};
    const QTimestamp timestamps[] = {-1, 1};
    const QMonth months[] = {-1, -13};
    const QDate dates[] = {-1, 1};
    const QDatetime datetimes[] = {-0.5, 1.5};
    const QTimespan timespans[] = {-1, 1};
    const QMinute minutes[] = {-1, 61};
    const QSecond seconds[] = {-1, 3661};
    const QTime times[] = {-1, 3661001};
    const struct {
        QTypeCode type;
        const void *values;
        const char *string;
        const char *literal;
    } cases[] = {
        {Q_TYPE_BOOLEAN, booleans, "1 0", "10b"},
        {Q_TYPE_GUID, guids, guidPairString, guidPairString},
        {Q_TYPE_BYTE, bytes, "01 ff", "0x01ff"},
        {Q_TYPE_SHORT, shorts, "-123 456", "-123 456h"},
        {Q_TYPE_INT, ints, "-123456 789012", "-123456 789012i"},
        {Q_TYPE_LONG, longs, "-1234567890123 4567890123456", "-1234567890123 4567890123456"},
        {Q_TYPE_REAL, reals, "1.25 -2.5", "1.25 -2.5e"},
        {Q_TYPE_FLOAT, floats, "1.25 -2.5", "1.25 -2.5"},
        {Q_TYPE_CHAR, chars, "a z", "\"az\""},
        {Q_TYPE_SYMBOL, symbols, "abc xyz", "`abc`xyz"},
        {Q_TYPE_TIMESTAMP,
         timestamps,
         "1999.12.31D23:59:59.999999999 2000.01.01D00:00:00.000000001",
         "1999.12.31D23:59:59.999999999 2000.01.01D00:00:00.000000001"},
        {Q_TYPE_MONTH, months, "1999.12 1998.12", "1999.12 1998.12m"},
        {Q_TYPE_DATE, dates, "1999.12.31 2000.01.02", "1999.12.31 2000.01.02"},
        {Q_TYPE_DATETIME,
         datetimes,
         "1999.12.31T12:00:00.000 2000.01.02T12:00:00.000",
         "1999.12.31T12:00:00.000 2000.01.02T12:00:00.000"},
        {Q_TYPE_TIMESPAN,
         timespans,
         "-0D00:00:00.000000001 0D00:00:00.000000001",
         "-0D00:00:00.000000001 0D00:00:00.000000001"},
        {Q_TYPE_MINUTE, minutes, "-00:01 01:01", "-00:01 01:01"},
        {Q_TYPE_SECOND, seconds, "-00:00:01 01:01:01", "-00:00:01 01:01:01"},
        {Q_TYPE_TIME, times, "-00:00:00.001 01:01:01.001", "-00:00:00.001 01:01:01.001"},
    };
    int (*format)(char *, QSize, const void *, QSize, QTypeCode, int) =
        literal ? qListToLiteral : qListToStr;
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        const char *expected = literal ? cases[i].literal : cases[i].string;
        QSize width = strlen(expected);
        memset(buffer, 0xA5, SIZE);
        int result = format(buffer, width + 1, cases[i].values, 2, cases[i].type, 3);
        TEST_ASSERT_EQUAL_INT(width, result);
        TEST_ASSERT_EQUAL_STRING(expected, buffer);
        TEST_ASSERT_EQUAL_HEX8(0xA5, (unsigned char)buffer[width + 1]);
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, format(buffer, width, cases[i].values, 2, cases[i].type, 3)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, format(nullptr, SIZE, cases[i].values, 2, cases[i].type, 3)
        );
    }
    const QTypeCode unsupported[] = {Q_TYPE_MIXED, 3, 20, -Q_TYPE_LONG, Q_TYPE_TABLE};
    for (QSize i = 0; i < sizeof(unsupported) / sizeof(unsupported[0]); i++) {
        strcpy(buffer, "unchanged");
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_TYPE, format(buffer, SIZE, ints, 2, unsupported[i], 3));
        TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
    }
}

void testListToStr() {
    assertGenericListFormatting(false);
}

void testListToLiteral() {
    assertGenericListFormatting(true);
}

void testMonthBoundaries() {
    const struct {
        QMonth value;
        const char *expected;
    } cases[] = {
        {-1, "1999.12"},
        {-11, "1999.02"},
        {-12, "1999.01"},
        {-13, "1998.12"},
        {INT32_MIN + 2, "-178954971.07"},
        {INT32_MAX - 1, "178958970.07"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        int result = qMonthToStr(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        char expected[32];
        strcpy(expected, cases[i].expected);
        strcat(expected, "m");
        result = qMonthToLiteral(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(expected), result);
        TEST_ASSERT_EQUAL_STRING(expected, buffer);
    }
}

void testDurationIntegerExtremes() {
    const struct {
        QTypeCode type;
        int32_t value;
        const char *expected;
    } cases[] = {
        {Q_TYPE_MINUTE, INT32_MIN + 2, "-35791394:06"},
        {Q_TYPE_MINUTE, INT32_MAX - 1, "35791394:06"},
        {Q_TYPE_SECOND, INT32_MIN + 2, "-596523:14:06"},
        {Q_TYPE_SECOND, INT32_MAX - 1, "596523:14:06"},
        {Q_TYPE_TIME, INT32_MIN + 2, "-596:31:23.646"},
        {Q_TYPE_TIME, INT32_MAX - 1, "596:31:23.646"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        for (int literal = 0; literal <= 1; literal++) {
            int result =
                (literal ? qAtomToLiteral
                         : qAtomToStr)(buffer, SIZE, &cases[i].value, cases[i].type, 0);
            TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
            TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        }
    }
}

void testYearFormattingBufferBounds() {
    const QMonth months[] = {96000, -36000};               // January in years 10000 and -1000
    const QDatetime datetimes[] = {2921940.0, -1095727.0}; // January 1 in the same years
    const QDate date = 0;
    const QDate dates[] = {2921940, -1095727}; // January 1 in years 10000 and -1000
    const struct {
        const void *values;
        QTypeCode type;
        QSize length; // Zero selects an atom; otherwise format a list
        const char *string;
        const char *literal;
    } cases[] = {
        {months, Q_TYPE_MONTH, 0, "10000.01", "10000.01m"},
        {months + 1, Q_TYPE_MONTH, 0, "-1000.01", "-1000.01m"},
        {datetimes, Q_TYPE_DATETIME, 0, "10000.01.01T00:00:00.000", "10000.01.01T00:00:00.000"},
        {datetimes + 1, Q_TYPE_DATETIME, 0, "-1000.01.01T00:00:00.000", "-1000.01.01T00:00:00.000"},
        {&date, Q_TYPE_DATE, 0, "2000.01.01", "2000.01.01"},
        {dates, Q_TYPE_DATE, 0, "10000.01.01", "10000.01.01"},
        {dates + 1, Q_TYPE_DATE, 0, "-1000.01.01", "-1000.01.01"},
        {dates, Q_TYPE_DATE, 2, "10000.01.01 -1000.01.01", "10000.01.01 -1000.01.01"},
        {months, Q_TYPE_MONTH, 2, "10000.01 -1000.01", "10000.01 -1000.01m"},
        {datetimes,
         Q_TYPE_DATETIME,
         2,
         "10000.01.01T00:00:00.000 -1000.01.01T00:00:00.000",
         "10000.01.01T00:00:00.000 -1000.01.01T00:00:00.000"},
    };

    for (QSize c = 0; c < sizeof(cases) / sizeof(cases[0]); c++) {
        for (int literal = 0; literal <= 1; literal++) {
            const char *expected = literal ? cases[c].literal : cases[c].string;
            QSize required = strlen(expected) + 1;
            for (QSize capacity = 0; capacity <= required; capacity++) {
                unsigned char guarded[SIZE];
                memset(guarded, 0xA5, sizeof(guarded));
                char *destination = (char *)&guarded[1];
                int result;
                if (cases[c].length == 0) {
                    result = (literal ? qAtomToLiteral : qAtomToStr)(
                        destination, capacity, cases[c].values, cases[c].type, 0
                    );
                } else {
                    result = (literal ? qListToLiteral : qListToStr)(
                        destination, capacity, cases[c].values, cases[c].length, cases[c].type, 0
                    );
                }

                TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
                for (QSize i = capacity + 1; i < sizeof(guarded); i++) {
                    TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
                }
                if (capacity < required) {
                    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
                    // Atom sizing must reject insufficient capacity before writing any output.
                    if (cases[c].length == 0) {
                        for (QSize i = 1; i <= capacity; i++) {
                            TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
                        }
                    }
                } else {
                    TEST_ASSERT_EQUAL_INT(required - 1, result);
                    TEST_ASSERT_EQUAL_STRING(expected, destination);
                }
            }
        }
    }
}

void testDateArithmeticBoundaries() {
    const struct {
        QDate value;
        const char *expected;
    } cases[] = {
        {-24856, "1931.12.13"},
        {-24855, "1931.12.14"},
        {24855, "2068.01.19"},
        {24856, "2068.01.20"},
        {-1, "1999.12.31"},
        {0, "2000.01.01"},
        {-36466, "1900.02.28"},
        {-36465, "1900.03.01"},
        {59, "2000.02.29"},
        {60, "2000.03.01"},
        {36583, "2100.02.28"},
        {36584, "2100.03.01"},
        {146156, "2400.02.29"},
        {-2147483646, "-5877611.06.24"},
        {2147483646, "5881610.07.10"},
    };

    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        int result = qDateToStr(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        result = qDateToLiteral(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
    }
}

void testTimestampRangeAndNegativeFraction() {
    const struct {
        QTimestamp value;
        const char *expected;
    } cases[] = {
        {-9223372036854775806LL, "1707.09.22D00:12:43.145224194"},
        {9223372036854775806LL, "2292.04.10D23:47:16.854775806"},
        {-1, "1999.12.31D23:59:59.999999999"},
    };

    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        int result = qTimestampToStr(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        result = qTimestampToLiteral(buffer, SIZE, cases[i].value);
        TEST_ASSERT_EQUAL_INT(strlen(cases[i].expected), result);
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
    }
}

void testFormattedCountLimits() {
    QSize count = 0;
    TEST_ASSERT_TRUE(tryAddFormattedCount(&count, INT_MAX));
    TEST_ASSERT_EQUAL_UINT64(INT_MAX, count);
    TEST_ASSERT_TRUE(tryAddFormattedCount(&count, 0));
    TEST_ASSERT_FALSE(tryAddFormattedCount(&count, 1));
    TEST_ASSERT_EQUAL_UINT64(INT_MAX, count);

    count = (QSize)INT_MAX - 3;
    TEST_ASSERT_TRUE(tryAddFormattedCount(&count, 2));
    TEST_ASSERT_FALSE(tryAddFormattedCount(&count, 2));
    TEST_ASSERT_EQUAL_UINT64((QSize)INT_MAX - 1, count);
    TEST_ASSERT_TRUE(tryAddFormattedCount(&count, 1));
    TEST_ASSERT_EQUAL_UINT64(INT_MAX, count);

    count = 1;
    TEST_ASSERT_FALSE(tryAddFormattedCount(&count, UINT64_MAX));
    TEST_ASSERT_EQUAL_UINT64(1, count);
    count = (QSize)INT_MAX + 1;
    TEST_ASSERT_FALSE(tryAddFormattedCount(&count, 0));
    TEST_ASSERT_EQUAL_UINT64((QSize)INT_MAX + 1, count);
}

void testGenericNullInputs() {
    const QTypeCode types[] = {
        Q_TYPE_BOOLEAN,
        Q_TYPE_GUID,
        Q_TYPE_BYTE,
        Q_TYPE_SHORT,
        Q_TYPE_INT,
        Q_TYPE_LONG,
        Q_TYPE_REAL,
        Q_TYPE_FLOAT,
        Q_TYPE_CHAR,
        Q_TYPE_SYMBOL,
        Q_TYPE_TIMESTAMP,
        Q_TYPE_MONTH,
        Q_TYPE_DATE,
        Q_TYPE_DATETIME,
        Q_TYPE_TIMESPAN,
        Q_TYPE_MINUTE,
        Q_TYPE_SECOND,
        Q_TYPE_TIME,
    };
    for (QSize i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        strcpy(buffer, "unchanged");
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, qAtomToStr(buffer, SIZE, nullptr, types[i], 2));
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT, qAtomToLiteral(buffer, SIZE, nullptr, types[i], 2)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT, qListToStr(buffer, SIZE, nullptr, 1, types[i], 2)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT, qListToLiteral(buffer, SIZE, nullptr, 1, types[i], 2)
        );
        TEST_ASSERT_EQUAL_INT(0, qListToStr(buffer, SIZE, nullptr, 0, types[i], 2));
        TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
        TEST_ASSERT_EQUAL_INT(0, qListToStr(nullptr, 0, nullptr, 0, types[i], 2));
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_SIZE, qListToLiteral(nullptr, 0, nullptr, 0, types[i], 2)
        );
    }

    const QTypeCode unsupported[] = {Q_TYPE_MIXED, 3, 20, -Q_TYPE_LONG, Q_TYPE_TABLE};
    for (QSize i = 0; i < sizeof(unsupported) / sizeof(unsupported[0]); i++) {
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_TYPE, qAtomToStr(nullptr, 0, nullptr, unsupported[i], 0));
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_TYPE, qAtomToLiteral(nullptr, 0, nullptr, unsupported[i], 0)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_TYPE, qListToStr(nullptr, 0, nullptr, 0, unsupported[i], 0)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_TYPE, qListToLiteral(nullptr, 0, nullptr, 0, unsupported[i], 0)
        );
    }
}

void testNullSymbolInputs() {
    strcpy(buffer, "unchanged");
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, qSymbolToStr(buffer, SIZE, nullptr));
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_ARGUMENT, qSymbolToLiteral(buffer, SIZE, nullptr));
    TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
    const QSymbol symbols[] = {"valid", nullptr};
    for (QSize start = 0; start < 2; start++) {
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT, qSymbolListToStr(buffer, SIZE, symbols + start, 2 - start)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT, qSymbolListToLiteral(buffer, SIZE, symbols + start, 2 - start)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT,
            qListToStr(buffer, SIZE, symbols + start, 2 - start, Q_TYPE_SYMBOL, 0)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_ARGUMENT,
            qListToLiteral(buffer, SIZE, symbols + start, 2 - start, Q_TYPE_SYMBOL, 0)
        );
    }
}

void testPrecisionRange() {
    const int invalid[] = {-1, INT_MIN, Q_FMT_MAX_PRECISION + 1, INT_MAX};
    const QReal real = 1.25f;
    const QFloat float_ = 1.25;
    for (QSize i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {
        int precision = invalid[i];
        strcpy(buffer, "unchanged");
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qRealToStr(buffer, SIZE, real, precision));
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qRealToLiteral(buffer, SIZE, real, precision));
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qFloatToStr(buffer, SIZE, float_, precision));
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qFloatToLiteral(buffer, SIZE, float_, precision));
        TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_RANGE, qRealListToStr(buffer, SIZE, &real, 1, precision));
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_RANGE, qRealListToLiteral(buffer, SIZE, &real, 1, precision)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_RANGE, qFloatListToStr(buffer, SIZE, &float_, 1, precision)
        );
        TEST_ASSERT_EQUAL_INT(
            Q_FMT_ERROR_RANGE, qFloatListToLiteral(buffer, SIZE, &float_, 1, precision)
        );
        for (int isReal = 0; isReal <= 1; isReal++) {
            const void *value = isReal ? (const void *)&real : (const void *)&float_;
            QTypeCode type = isReal ? Q_TYPE_REAL : Q_TYPE_FLOAT;
            TEST_ASSERT_EQUAL_INT(
                Q_FMT_ERROR_RANGE, qAtomToStr(buffer, SIZE, value, type, precision)
            );
            TEST_ASSERT_EQUAL_INT(
                Q_FMT_ERROR_RANGE, qAtomToLiteral(buffer, SIZE, value, type, precision)
            );
            TEST_ASSERT_EQUAL_INT(
                Q_FMT_ERROR_RANGE, qListToStr(buffer, SIZE, value, 1, type, precision)
            );
            TEST_ASSERT_EQUAL_INT(
                Q_FMT_ERROR_RANGE, qListToLiteral(buffer, SIZE, value, 1, type, precision)
            );
            TEST_ASSERT_EQUAL_INT(0, qListToStr(nullptr, 0, nullptr, 0, type, precision));
            TEST_ASSERT_EQUAL_INT(
                Q_FMT_ERROR_SIZE, qListToLiteral(nullptr, 0, nullptr, 0, type, precision)
            );
        }
        TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
        QInt integer = 42;
        TEST_ASSERT_EQUAL_INT(2, qAtomToStr(buffer, SIZE, &integer, Q_TYPE_INT, precision));
        TEST_ASSERT_EQUAL_STRING("42", buffer);
    }
}

void testMaximumPrecision() {
    const QReal real = 1.23456789f;
    const QFloat float_ = 1.23456789123456789;
    // Golden output from Q at every supported setting, including its type-specific P 0.
    const char *realExpected[] = {
        "1.2345679",
        "1",
        "1.2",
        "1.23",
        "1.235",
        "1.2346",
        "1.23457",
        "1.234568",
        "1.2345679",
        "1.23456788",
        "1.234567881",
        "1.2345678806",
        "1.23456788063",
        "1.23456788063",
        "1.2345678806305",
        "1.23456788063049",
        "1.234567880630493",
        "1.2345678806304932"
    };
    const char *floatExpected[] = {
        "1.2345678912345679",
        "1",
        "1.2",
        "1.23",
        "1.235",
        "1.2346",
        "1.23457",
        "1.234568",
        "1.2345679",
        "1.23456789",
        "1.234567891",
        "1.2345678912",
        "1.23456789123",
        "1.234567891235",
        "1.2345678912346",
        "1.23456789123457",
        "1.234567891234568",
        "1.2345678912345679"
    };
    TEST_ASSERT_EQUAL_INT(7, Q_FMT_DEFAULT_PRECISION);
    TEST_ASSERT_EQUAL_INT(17, Q_FMT_MAX_PRECISION);
    for (int precision = 0; precision <= Q_FMT_MAX_PRECISION; precision++) {
        for (int isReal = 0; isReal <= 1; isReal++) {
            const void *value = isReal ? (const void *)&real : (const void *)&float_;
            QTypeCode type = isReal ? Q_TYPE_REAL : Q_TYPE_FLOAT;
            for (int literal = 0; literal <= 1; literal++) {
                char expected[32];
                snprintf(
                    expected,
                    sizeof(expected),
                    "%s%s",
                    isReal ? realExpected[precision] : floatExpected[precision],
                    literal ? (isReal ? "e" : (precision == 1 ? "f" : "")) : ""
                );
                QSize width = strlen(expected);
                TEST_ASSERT_EQUAL_INT(
                    width,
                    (literal ? qAtomToLiteral
                             : qAtomToStr)(buffer, width + 1, value, type, precision)
                );
                TEST_ASSERT_EQUAL_STRING(expected, buffer);
                char listExpected[40];
                snprintf(listExpected, sizeof(listExpected), "%s%s", literal ? "," : "", expected);
                QSize listWidth = strlen(listExpected);
                TEST_ASSERT_EQUAL_INT(
                    listWidth,
                    (literal ? qListToLiteral
                             : qListToStr)(buffer, listWidth + 1, value, 1, type, precision)
                );
                TEST_ASSERT_EQUAL_STRING(listExpected, buffer);
                TEST_ASSERT_EQUAL_INT(
                    Q_FMT_ERROR_SIZE,
                    (literal ? qAtomToLiteral : qAtomToStr)(buffer, width, value, type, precision)
                );
            }
        }
    }
}

void testPrecisionRounding() {
    const struct {
        QFloat value;
        int precision;
        const char *expected;
        const char *floatLiteral;
    } cases[] = {
        {1.49, 1, "1", "1f"},
        {1.51, 1, "2", "2f"},
        {-1.51, 1, "-2", "-2f"},
        {1.1249, 3, "1.12", "1.12"},
        {1.1251, 3, "1.13", "1.13"},
        {-1.1249, 3, "-1.12", "-1.12"},
        {-1.1251, 3, "-1.13", "-1.13"},
        {9.9996, 4, "10", "10f"},
        {2.5, 1, "2", "2f"},
        {3.5, 1, "4", "4f"},
        {-2.5, 1, "-2", "-2f"},
        {1.25, 2, "1.2", "1.2"},
        {1.75, 2, "1.8", "1.8"},
        {0.0, 7, "0", "0f"},
        {-0.0, 7, "-0", "-0f"},
        {0.00001, 7, "1e-05", "1e-05"},
        {0.0001, 7, "0.0001", "0.0001"},
        {1000000, 7, "1000000", "1000000f"},
        {10000000, 7, "1e+07", "1e+07"},
        {999.5, 3, "1e+03", "1e+03"},
        {999.5, 4, "999.5", "999.5"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        QReal real = (QReal)cases[i].value;
        for (int literal = 0; literal <= 1; literal++) {
            for (int isReal = 0; isReal <= 1; isReal++) {
                char expected[32];
                strcpy(expected, literal && !isReal ? cases[i].floatLiteral : cases[i].expected);
                if (literal && isReal)
                    strcat(expected, "e");
                const void *value = isReal ? (const void *)&real : (const void *)&cases[i].value;
                int result = (literal ? qAtomToLiteral : qAtomToStr)(
                    buffer, SIZE, value, isReal ? Q_TYPE_REAL : Q_TYPE_FLOAT, cases[i].precision
                );
                TEST_ASSERT_EQUAL_INT(strlen(expected), result);
                TEST_ASSERT_EQUAL_STRING(expected, buffer);
            }
        }
    }
}

void testDisplayPrecisionLists() {
    const QReal subnormals[] = {-FLT_TRUE_MIN, FLT_MIN, FLT_TRUE_MIN};
    {
        ASSERT_PRECISION_LIST(qRealListToLiteral, subnormals, 3, 0, "-0 1.1754944e-38 0e", ",-0e");
    }

    const QReal reals[] = {1.25f, 1e20f, -0.0f, NAN};
    const QFloat floats[] = {1.25, 1e20, -0.0, NAN};
    {
        ASSERT_PRECISION_LIST(qRealListToStr, reals, 4, 2, "1.2 1e+20 -0 ", "1.2");
    }
    {
        ASSERT_PRECISION_LIST(qRealListToLiteral, reals, 4, 2, "1.2 1e+20 -0 0Ne", ",1.2e");
    }
    {
        ASSERT_PRECISION_LIST(qFloatListToStr, floats, 4, 2, "1.2 1e+20 -0 ", "1.2");
    }
    {
        ASSERT_PRECISION_LIST(qFloatListToLiteral, floats, 4, 2, "1.2 1e+20 -0 0n", ",1.2");
    }
    {
        ASSERT_PRECISION_LIST(qRealListToLiteral, reals, 4, 0, "1.25 1e+20 -0 0Ne", ",1.25e");
    }
    {
        ASSERT_PRECISION_LIST(
            qRealListToLiteral, reals, 4, 17, "1.25 1.0000000200408773e+20 -0 0Ne", ",1.25e"
        );
    }
    {
        ASSERT_PRECISION_LIST(
            qFloatListToLiteral, floats, 4, Q_FMT_DEFAULT_PRECISION, "1.25 1e+20 -0 0n", ",1.25"
        );
    }
}

void testCharListEmbeddedNull() {
    const QChar chars[] = {'a', '\0', 'b'};
    const char expectedString[] = {'a', ' ', '\0', ' ', 'b', '\0'};
    char output[10];
    memset(output, 0x5A, sizeof(output));
    int result = qCharListToStr(output, 6, chars, 3);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_MEMORY(expectedString, output, 6);
    TEST_ASSERT_EQUAL_HEX8(0x5A, output[6]);
    result = qCharListToLiteral(output, 9, chars, 3);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("\"a\\000b\"", output);
    TEST_ASSERT_EQUAL_HEX8(0x5A, output[9]);
}

static void assertDynamicAtomBounds(
    const void *atom, QTypeCode type, int precision, bool literal, const char *expected
) {
    QSize width = strlen(expected);
    const QSize capacities[] = {0, width, width + 1, width + 2};
    int (*formatter)(char *, QSize, const void *, QTypeCode, int) =
        literal ? qAtomToLiteral : qAtomToStr;
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, formatter(nullptr, width + 1, atom, type, precision));
    for (QSize c = 0; c < sizeof(capacities) / sizeof(capacities[0]); c++) {
        unsigned char guarded[512];
        memset(guarded, 0xA5, sizeof(guarded));
        QSize capacity = capacities[c];
        TEST_ASSERT_TRUE(capacity + 1 < sizeof(guarded));
        int result = formatter((char *)guarded + 1, capacity, atom, type, precision);
        TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
        for (QSize i = capacity + 1; i < sizeof(guarded); i++) {
            TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
        }
        if (capacity <= width) {
            TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
            // The scratch-buffer path and its fallback both check size before copying.
            for (QSize i = 1; i <= capacity; i++) {
                TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
            }
        } else {
            TEST_ASSERT_EQUAL_INT(width, result);
            TEST_ASSERT_EQUAL_STRING(expected, (const char *)guarded + 1);
        }
    }
}

void testDynamicSymbolBufferSizes() {
    // Include both sides of the scratch-buffer boundary, including a literal prefix.
    const QSize lengths[] = {0, 126, 127, 128, 129, 256};
    char symbol[257];
    char expected[258];
    for (QSize i = 0; i < sizeof(lengths) / sizeof(lengths[0]); i++) {
        memset(symbol, 'x', lengths[i]);
        symbol[lengths[i]] = '\0';
        assertDynamicAtomBounds(symbol, Q_TYPE_SYMBOL, 0, false, symbol);
        expected[0] = '`';
        memcpy(expected + 1, symbol, lengths[i] + 1);
        assertDynamicAtomBounds(symbol, Q_TYPE_SYMBOL, 0, true, expected);
    }
}

void testDynamicFloatBufferSizes() {
    const struct {
        QFloat value;
        int precision;
        const char *expected;
    } floats[] = {
        {DBL_MAX, 0, "1.7976931348623157e+308"},
        {-DBL_MAX, 17, "-1.7976931348623157e+308"},
        {DBL_MIN, 0, "2.2250738585072014e-308"},
        {DBL_TRUE_MIN, 17, "4.9406564584124654e-324"},
        {-DBL_TRUE_MIN, 17, "-4.9406564584124654e-324"},
        {0.0001, 17, "0.0001"},
        {1.25, 2, "1.2"},
        {-0.0, 0, "-0"},
    };
    for (QSize i = 0; i < sizeof(floats) / sizeof(floats[0]); i++) {
        for (int literal = 0; literal <= 1; literal++)
            assertDynamicAtomBounds(
                &floats[i].value,
                Q_TYPE_FLOAT,
                floats[i].precision,
                literal,
                literal && floats[i].value == 0 ? "-0f" : floats[i].expected
            );
    }
    const struct {
        QReal value;
        int precision;
        const char *expected;
    } reals[] = {
        {FLT_MAX, 0, "3.4028235e+38"},
        {FLT_MAX, 17, "3.4028234663852886e+38"},
        {FLT_MIN, 0, "1.1754944e-38"},
        {FLT_TRUE_MIN, 17, "0"},
        {-FLT_TRUE_MIN, 0, "-0"},
        {nextafterf(FLT_MIN, 0), 0, "0"},
        {-nextafterf(FLT_MIN, 0), 17, "-0"},
        {-0.0f, 0, "-0"},
        {1.25f, 2, "1.2"},
    };
    for (QSize i = 0; i < sizeof(reals) / sizeof(reals[0]); i++) {
        assertDynamicAtomBounds(
            &reals[i].value, Q_TYPE_REAL, reals[i].precision, false, reals[i].expected
        );
        char expected[32];
        snprintf(expected, sizeof(expected), "%se", reals[i].expected);
        assertDynamicAtomBounds(&reals[i].value, Q_TYPE_REAL, reals[i].precision, true, expected);
    }
}

void testFloatLiteralSuffix() {
    // Golden .Q.s1 output: the suffix follows displayed text, not the raw value.
    const struct {
        QFloat value;
        int precision;
        const char *expected;
    } cases[] = {
        {1, 7, "1f"},
        {-1, 7, "-1f"},
        {0, 0, "0f"},
        {-0.0, 17, "-0f"},
        {1.25, 1, "1f"},
        {1.25, 2, "1.2"},
        {1.00000001, 7, "1f"},
        {1.00000001, 0, "1.0000000099999999"},
        {1000000, 7, "1000000f"},
        {10000000, 7, "1e+07"},
        {9.5, 1, "1e+01"},
        {NAN, 7, "0n"},
        {INFINITY, 7, "0w"},
        {-INFINITY, 7, "-0w"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        TEST_ASSERT_EQUAL_INT(
            strlen(cases[i].expected),
            qFloatToLiteral(buffer, SIZE, cases[i].value, cases[i].precision)
        );
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        assertDynamicAtomBounds(
            &cases[i].value, Q_TYPE_FLOAT, cases[i].precision, true, cases[i].expected
        );
    }
}

void testFloatListLiteralSuffix() {
    const struct {
        QFloat values[3];
        QSize length;
        int precision;
        const char *expected;
        const char *singleton;
    } cases[] = {
        {{1, 2}, 2, 7, "1 2f", ",1f"},
        {{-0.0, 0}, 2, 7, "-0 0f", ",-0f"},
        {{1.25, 2.25}, 2, 1, "1 2f", ",1f"},
        {{1.25, 2.25}, 2, 3, "1.25 2.25", ",1.25"},
        {{1, 1.5}, 2, 7, "1 1.5", ",1f"},
        {{1.5, 1}, 2, 7, "1.5 1", ",1.5"},
        {{1, 1e7}, 2, 7, "1 1e+07", ",1f"},
        {{1e7, 1}, 2, 7, "1e+07 1", ",1e+07"},
        {{1, NAN}, 2, 7, "1 0n", ",1f"},
        {{NAN, 1}, 2, 7, "0n 1", ",0n"},
        {{1, INFINITY}, 2, 7, "1 0w", ",1f"},
        {{-INFINITY, 1}, 2, 7, "-0w 1", ",-0w"},
        {{NAN, INFINITY, -INFINITY}, 3, 7, "0n 0w -0w", ",0n"},
        {{1, 1.5, 2}, 3, 7, "1 1.5 2", ",1f"},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        ASSERT_PRECISION_LIST(
            qFloatListToLiteral,
            cases[i].values,
            cases[i].length,
            cases[i].precision,
            cases[i].expected,
            cases[i].singleton
        );
        TEST_ASSERT_EQUAL_INT(
            strlen(cases[i].expected),
            qListToLiteral(
                buffer, SIZE, cases[i].values, cases[i].length, Q_TYPE_FLOAT, cases[i].precision
            )
        );
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
    }
}

void testEmptySymbolListItems() {
    const QSymbol symbols[] = {"", "abc"};
    {
        ASSERT_LIST(qSymbolListToStr, symbols, 2, " abc", "");
    }
    {
        ASSERT_LIST(qSymbolListToLiteral, symbols, 2, "``abc", ",`");
    }
}

// Expected tokens were checked against Q's string and .Q.s1.
static const QShort specialShorts[] = {Q_SHORT_NULL, Q_SHORT_INF, -Q_SHORT_INF, 0};
static const QInt specialInts[] = {Q_INT_NULL, Q_INT_INF, -Q_INT_INF, 0};
static const QLong specialLongs[] = {Q_LONG_NULL, Q_LONG_INF, -Q_LONG_INF, 0};
static const QReal specialReals[] = {NAN, INFINITY, -INFINITY, 0};
static const QFloat specialFloats[] = {NAN, INFINITY, -INFINITY, 0};

static const struct {
    QTypeCode type;
    const void *values;
    QSize itemSize;
    const char *strings[3];
    const char *literals[3];
    const char *listLiteral;
    const char *nullListLiteral;
    const char *mixedString;
    const char *mixedLiteral;
    const char *reversedMixedLiteral;
} specialCases[] = {
    {Q_TYPE_SHORT,
     specialShorts,
     sizeof(QShort),
     {"", "0W", "-0W"},
     {"0Nh", "0Wh", "-0Wh"},
     "0N 0W -0Wh",
     "0N 0Nh",
     " 0",
     "0N 0h",
     "0 0Nh"},
    {Q_TYPE_INT,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Ni", "0Wi", "-0Wi"},
     "0N 0W -0Wi",
     "0N 0Ni",
     " 0",
     "0N 0i",
     "0 0Ni"},
    {Q_TYPE_LONG,
     specialLongs,
     sizeof(QLong),
     {"", "0W", "-0W"},
     {"0N", "0W", "-0W"},
     "0N 0W -0W",
     "0N 0N",
     " 0",
     "0N 0",
     "0 0N"},
    {Q_TYPE_REAL,
     specialReals,
     sizeof(QReal),
     {"", "0w", "-0w"},
     {"0Ne", "0we", "-0we"},
     "0N 0w -0we",
     "0N 0Ne",
     " 0",
     "0N 0e",
     "0 0Ne"},
    {Q_TYPE_FLOAT,
     specialFloats,
     sizeof(QFloat),
     {"", "0w", "-0w"},
     {"0n", "0w", "-0w"},
     "0n 0w -0w",
     "0n 0n",
     " 0",
     "0n 0",
     "0 0n"},
    {Q_TYPE_TIMESTAMP,
     specialLongs,
     sizeof(QLong),
     {"", "0W", "-0W"},
     {"0Np", "0Wp", "-0Wp"},
     "0N 0W -0Wp",
     "0N 0Np",
     " 2000.01.01D00:00:00.000000000",
     "0N 2000.01.01D00:00:00.000000000",
     "2000.01.01D00:00:00.000000000 0N"},
    {Q_TYPE_MONTH,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Nm", "0Wm", "-0Wm"},
     "0N 0W -0Wm",
     "0N 0Nm",
     " 2000.01",
     "0N 2000.01m",
     "2000.01 0Nm"},
    {Q_TYPE_DATE,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Nd", "0Wd", "-0Wd"},
     "0N 0W -0Wd",
     "0N 0Nd",
     " 2000.01.01",
     "0N 2000.01.01",
     "2000.01.01 0N"},
    {Q_TYPE_DATETIME,
     specialFloats,
     sizeof(QFloat),
     {"", "0w", "-0w"},
     {"0Nz", "0wz", "-0wz"},
     "0N 0w -0wz",
     "0N 0Nz",
     " 2000.01.01T00:00:00.000",
     "0N 2000.01.01T00:00:00.000",
     "2000.01.01T00:00:00.000 0N"},
    {Q_TYPE_TIMESPAN,
     specialLongs,
     sizeof(QLong),
     {"", "0W", "-0W"},
     {"0Nn", "0Wn", "-0Wn"},
     "0N 0W -0Wn",
     "0N 0Nn",
     " 0D00:00:00.000000000",
     "0N 0D00:00:00.000000000",
     "0D00:00:00.000000000 0N"},
    {Q_TYPE_MINUTE,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Nu", "0Wu", "-0Wu"},
     "0N 0W -0Wu",
     "0N 0Nu",
     " 00:00",
     "0N 00:00",
     "00:00 0N"},
    {Q_TYPE_SECOND,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Nv", "0Wv", "-0Wv"},
     "0N 0W -0Wv",
     "0N 0Nv",
     " 00:00:00",
     "0N 00:00:00",
     "00:00:00 0N"},
    {Q_TYPE_TIME,
     specialInts,
     sizeof(QInt),
     {"", "0W", "-0W"},
     {"0Nt", "0Wt", "-0Wt"},
     "0N 0W -0Wt",
     "0N 0Nt",
     " 00:00:00.000",
     "0N 00:00:00.000",
     "00:00:00.000 0N"},
};

void testSpecialAtoms() {
    for (QSize c = 0; c < sizeof(specialCases) / sizeof(specialCases[0]); c++) {
        for (QSize i = 0; i < 3; i++) {
            const void *value =
                (const unsigned char *)specialCases[c].values + i * specialCases[c].itemSize;
            for (int literal = 0; literal <= 1; literal++) {
                const char *expected =
                    literal ? specialCases[c].literals[i] : specialCases[c].strings[i];
                for (QSize capacity = 0; capacity <= strlen(expected) + 1; capacity++) {
                    unsigned char guarded[SIZE + 2];
                    memset(guarded, 0xA5, sizeof(guarded));
                    int result = (literal ? qAtomToLiteral : qAtomToStr)(
                        (char *)guarded + 1, capacity, value, specialCases[c].type, 2
                    );
                    assertOutputBounds(guarded, capacity, result, expected);
                    if (result < 0)
                        TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, guarded, sizeof(guarded));
                }
                TEST_ASSERT_EQUAL_INT(
                    Q_FMT_ERROR_SIZE,
                    (literal ? qAtomToLiteral
                             : qAtomToStr)(nullptr, SIZE, value, specialCases[c].type, 2)
                );
            }
        }
    }
}

void testSpecialLists() {
    for (QSize c = 0; c < sizeof(specialCases) / sizeof(specialCases[0]); c++) {
        // All-special, all-null, mixed, and reversed mixed lists exercise suffix
        // selection and leading/trailing empty fields. One null must terminate too.
        const unsigned indices[][3] = {{0, 1, 2}, {0, 0}, {0, 3}, {3, 0}, {0}};
        const QSize lengths[] = {3, 2, 2, 2, 1};
        for (QSize fixture = 0; fixture < 5; fixture++) {
            union {
                QShort shorts[3];
                QInt ints[3];
                QLong longs[3];
                QReal reals[3];
                QFloat floats[3];
            } values;
            void *data;
            switch (specialCases[c].type) {
                case Q_TYPE_SHORT:
                    data = values.shorts;
                    break;
                case Q_TYPE_REAL:
                    data = values.reals;
                    break;
                case Q_TYPE_FLOAT:
                case Q_TYPE_DATETIME:
                    data = values.floats;
                    break;
                case Q_TYPE_LONG:
                case Q_TYPE_TIMESTAMP:
                case Q_TYPE_TIMESPAN:
                    data = values.longs;
                    break;
                default:
                    data = values.ints;
                    break;
            }
            for (QSize i = 0; i < lengths[fixture]; i++)
                memcpy(
                    (unsigned char *)data + i * specialCases[c].itemSize,
                    (const unsigned char *)specialCases[c].values +
                        indices[fixture][i] * specialCases[c].itemSize,
                    specialCases[c].itemSize
                );
            char stringExpected[SIZE], literalExpected[SIZE];
            if (fixture == 0) {
                snprintf(
                    stringExpected,
                    SIZE,
                    " %s %s",
                    specialCases[c].strings[1],
                    specialCases[c].strings[2]
                );
                strcpy(literalExpected, specialCases[c].listLiteral);
            } else if (fixture == 1) {
                strcpy(stringExpected, " ");
                strcpy(literalExpected, specialCases[c].nullListLiteral);
            } else if (fixture == 2) {
                strcpy(stringExpected, specialCases[c].mixedString);
                strcpy(literalExpected, specialCases[c].mixedLiteral);
            } else if (fixture == 3) {
                snprintf(stringExpected, SIZE, "%s ", specialCases[c].mixedString + 1);
                strcpy(literalExpected, specialCases[c].reversedMixedLiteral);
            } else {
                strcpy(stringExpected, "");
                snprintf(literalExpected, SIZE, ",%s", specialCases[c].literals[0]);
            }
            for (int literal = 0; literal <= 1; literal++) {
                const char *expected = literal ? literalExpected : stringExpected;
                for (QSize capacity = 0; capacity <= strlen(expected) + 1; capacity++) {
                    unsigned char guarded[SIZE + 2];
                    memset(guarded, 0xA5, sizeof(guarded));
                    int result = (literal ? qListToLiteral : qListToStr)(
                        (char *)guarded + 1,
                        capacity,
                        data,
                        lengths[fixture],
                        specialCases[c].type,
                        2
                    );
                    assertOutputBounds(guarded, capacity, result, expected);
                }
            }
        }
    }
}

void testSpecialPrecisionAndNaNs() {
    const QReal reals[] = {nanf("1"), -nanf("2"), INFINITY, -INFINITY};
    const QFloat floats[] = {nan("1"), -nan("2"), INFINITY, -INFINITY};
    for (int isReal = 0; isReal <= 1; isReal++) {
        QTypeCode type = isReal ? Q_TYPE_REAL : Q_TYPE_FLOAT;
        for (QSize i = 0; i < 4; i++) {
            const void *value = isReal ? (const void *)&reals[i] : (const void *)&floats[i];
            for (int literal = 0; literal <= 1; literal++) {
                const int invalid[] = {-1, Q_FMT_MAX_PRECISION + 1};
                for (QSize p = 0; p < 2; p++) {
                    strcpy(buffer, "unchanged");
                    TEST_ASSERT_EQUAL_INT(
                        Q_FMT_ERROR_RANGE,
                        (literal ? qAtomToLiteral
                                 : qAtomToStr)(buffer, SIZE, value, type, invalid[p])
                    );
                    TEST_ASSERT_EQUAL_INT(
                        Q_FMT_ERROR_RANGE,
                        (literal ? qListToLiteral
                                 : qListToStr)(buffer, SIZE, value, 1, type, invalid[p])
                    );
                    TEST_ASSERT_EQUAL_STRING("unchanged", buffer);
                }
                if (i < 2) {
                    const char *expected = literal ? (isReal ? "0Ne" : "0n") : "";
                    TEST_ASSERT_EQUAL_INT(
                        strlen(expected),
                        (literal ? qAtomToLiteral
                                 : qAtomToStr)(buffer, SIZE, value, type, Q_FMT_MAX_PRECISION)
                    );
                    TEST_ASSERT_EQUAL_STRING(expected, buffer);
                }
            }
        }
    }
    QDatetime datetime = -nan("123");
    TEST_ASSERT_EQUAL_INT(0, qDatetimeToStr(buffer, SIZE, datetime));
    TEST_ASSERT_EQUAL_STRING("", buffer);
    TEST_ASSERT_EQUAL_INT(3, qDatetimeToLiteral(buffer, SIZE, datetime));
    TEST_ASSERT_EQUAL_STRING("0Nz", buffer);
}

void testFiniteIntegerBoundaries() {
    const QShort shorts[] = {INT16_MIN + 2, INT16_MAX - 1};
    const QInt ints[] = {INT32_MIN + 2, INT32_MAX - 1};
    const QLong longs[] = {INT64_MIN + 2, INT64_MAX - 1};
    const struct {
        QTypeCode type;
        const void *values;
        QSize size;
        const char *expected[2];
        const char *suffix;
    } cases[] = {
        {Q_TYPE_SHORT, shorts, sizeof(QShort), {"-32766", "32766"}, "h"},
        {Q_TYPE_INT, ints, sizeof(QInt), {"-2147483646", "2147483646"}, "i"},
        {Q_TYPE_LONG, longs, sizeof(QLong), {"-9223372036854775806", "9223372036854775806"}, ""},
    };
    for (QSize c = 0; c < 3; c++) {
        for (QSize i = 0; i < 2; i++) {
            const void *value = (const unsigned char *)cases[c].values + i * cases[c].size;
            for (int literal = 0; literal <= 1; literal++) {
                char expected[32];
                snprintf(
                    expected,
                    sizeof(expected),
                    "%s%s",
                    cases[c].expected[i],
                    literal ? cases[c].suffix : ""
                );
                TEST_ASSERT_EQUAL_INT(
                    strlen(expected),
                    (literal ? qAtomToLiteral : qAtomToStr)(buffer, SIZE, value, cases[c].type, 0)
                );
                TEST_ASSERT_EQUAL_STRING(expected, buffer);
            }
        }
    }
}

// Expected text captured from .Q.s1 in Q, including octal escapes for backspace,
// vertical tab, form feed, DEL, and all high bytes (not C's \b, \v, or \f).
void testCharLiteralEscapes() {
    const struct {
        unsigned char value;
        const char *expected;
    } cases[] = {
        {0, "\"\\000\""},
        {7, "\"\\007\""},
        {8, "\"\\010\""},
        {9, "\"\\t\""},
        {10, "\"\\n\""},
        {11, "\"\\013\""},
        {12, "\"\\014\""},
        {13, "\"\\r\""},
        {31, "\"\\037\""},
        {32, "\" \""},
        {34, "\"\\\"\""},
        {48, "\"0\""},
        {92, "\"\\\\\""},
        {126, "\"~\""},
        {127, "\"\\177\""},
        {128, "\"\\200\""},
        {255, "\"\\377\""},
    };
    for (QSize i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        QChar value = (QChar)cases[i].value;
        for (QSize capacity = 0; capacity <= strlen(cases[i].expected) + 1; capacity++) {
            unsigned char guarded[SIZE + 2];
            memset(guarded, 0xA5, sizeof(guarded));
            int result = qCharToLiteral((char *)guarded + 1, capacity, value);
            assertOutputBounds(guarded, capacity, result, cases[i].expected);
            if (result < 0)
                TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, guarded, sizeof(guarded));
        }
        TEST_ASSERT_EQUAL_INT(
            strlen(cases[i].expected), qAtomToLiteral(buffer, SIZE, &value, Q_TYPE_CHAR, 0)
        );
        TEST_ASSERT_EQUAL_STRING(cases[i].expected, buffer);
        // String formatting continues to emit the original byte, even for zero.
        TEST_ASSERT_EQUAL_INT(1, qCharToStr(buffer, SIZE, value));
        TEST_ASSERT_EQUAL_HEX8(cases[i].value, (unsigned char)buffer[0]);
        TEST_ASSERT_EQUAL_HEX8(0, buffer[1]);
    }
}

void testCharListLiteralAllBytes() {
    const char expected[] =
        "\"\\000\\001\\002\\003\\004\\005\\006\\007\\010\\t\\n\\013\\014\\r\\016\\017"
        "\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\033\\034\\035\\036\\037"
        " !\\\"#$%&'()*+,-./"
        "0123456789:;<=>?"
        "@ABCDEFGHIJKLMNO"
        "PQRSTUVWXYZ[\\\\]^_"
        "`abcdefghijklmno"
        "pqrstuvwxyz{|}~\\177"
        "\\200\\201\\202\\203\\204\\205\\206\\207\\210\\211\\212\\213\\214\\215\\216\\217"
        "\\220\\221\\222\\223\\224\\225\\226\\227\\230\\231\\232\\233\\234\\235\\236\\237"
        "\\240\\241\\242\\243\\244\\245\\246\\247\\250\\251\\252\\253\\254\\255\\256\\257"
        "\\260\\261\\262\\263\\264\\265\\266\\267\\270\\271\\272\\273\\274\\275\\276\\277"
        "\\300\\301\\302\\303\\304\\305\\306\\307\\310\\311\\312\\313\\314\\315\\316\\317"
        "\\320\\321\\322\\323\\324\\325\\326\\327\\330\\331\\332\\333\\334\\335\\336\\337"
        "\\340\\341\\342\\343\\344\\345\\346\\347\\350\\351\\352\\353\\354\\355\\356\\357"
        "\\360\\361\\362\\363\\364\\365\\366\\367\\370\\371\\372\\373\\374\\375\\376\\377\"";
    QChar chars[256];
    for (unsigned i = 0; i < 256; i++)
        chars[i] = (QChar)i;
    // Test every capacity, including failures in a four-byte escape and the closing quote.
    for (QSize capacity = 0; capacity <= sizeof(expected); capacity++) {
        unsigned char guarded[sizeof(expected) + 2];
        memset(guarded, 0xA5, sizeof(guarded));
        int result = qCharListToLiteral((char *)guarded + 1, capacity, chars, 256);
        TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[0]);
        for (QSize i = capacity + 1; i < sizeof(guarded); i++)
            TEST_ASSERT_EQUAL_HEX8(0xA5, guarded[i]);
        if (capacity < sizeof(expected)) {
            TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
        } else {
            TEST_ASSERT_EQUAL_INT(sizeof(expected) - 1, result);
            TEST_ASSERT_EQUAL_STRING(expected, (char *)guarded + 1);
        }
    }
    char output[sizeof(expected)];
    TEST_ASSERT_EQUAL_INT(
        sizeof(expected) - 1, qListToLiteral(output, sizeof(output), chars, 256, Q_TYPE_CHAR, 0)
    );
    TEST_ASSERT_EQUAL_STRING(expected, output);
}

void testCharListLiteralEscapes() {
    // Three octal digits keep the following numeric characters unambiguous.
    const QChar chars[] = {'\0', '0', '7', '\t', '\n', '\r', '"', '\\', (QChar)255};
    ASSERT_LIST(
        qCharListToLiteral, chars, sizeof(chars), "\"\\00007\\t\\n\\r\\\"\\\\\\377\"", ",\"\\000\""
    );
}

// Precision is deliberately invalid for real/float: an empty list has no values
// to format, so precision is ignored while literal output still needs a buffer.
#define ASSERT_EMPTY_LISTS(type, stringFunc, literalFunc, expected, ...)                       \
    do {                                                                                       \
        for (QSize capacity = 0; capacity <= strlen(expected) + 1; capacity++) {               \
            unsigned char guarded[SIZE + 2];                                                   \
            memset(guarded, 0xA5, sizeof(guarded));                                            \
            int result = stringFunc((char *)guarded + 1, capacity, nullptr, 0, ##__VA_ARGS__); \
            TEST_ASSERT_EQUAL_INT(0, result);                                                  \
            TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, guarded, sizeof(guarded));                       \
            result = literalFunc((char *)guarded + 1, capacity, nullptr, 0, ##__VA_ARGS__);    \
            assertOutputBounds(guarded, capacity, result, expected);                           \
            if (result < 0)                                                                    \
                TEST_ASSERT_EACH_EQUAL_HEX8(0xA5, guarded, sizeof(guarded));                   \
            memset(guarded, 0xA5, sizeof(guarded));                                            \
            result = qListToLiteral((char *)guarded + 1, capacity, nullptr, 0, type, INT_MIN); \
            assertOutputBounds(guarded, capacity, result, expected);                           \
        }                                                                                      \
        TEST_ASSERT_EQUAL_INT(0, stringFunc(nullptr, 0, nullptr, 0, ##__VA_ARGS__));           \
        TEST_ASSERT_EQUAL_INT(                                                                 \
            Q_FMT_ERROR_SIZE, literalFunc(nullptr, SIZE, nullptr, 0, ##__VA_ARGS__)            \
        );                                                                                     \
        TEST_ASSERT_EQUAL_INT(                                                                 \
            Q_FMT_ERROR_SIZE, literalFunc(nullptr, 0, nullptr, 0, ##__VA_ARGS__)               \
        );                                                                                     \
        TEST_ASSERT_EQUAL_INT(                                                                 \
            strlen(expected), qListToLiteral(buffer, SIZE, &guids[0], 0, type, INT_MIN)        \
        );                                                                                     \
        TEST_ASSERT_EQUAL_STRING(expected, buffer);                                            \
    } while (0)

void testEmptyLists() {
    ASSERT_EMPTY_LISTS(Q_TYPE_BOOLEAN, qBooleanListToStr, qBooleanListToLiteral, "`boolean$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_GUID, qGuidListToStr, qGuidListToLiteral, "`guid$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_BYTE, qByteListToStr, qByteListToLiteral, "`byte$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_SHORT, qShortListToStr, qShortListToLiteral, "`short$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_INT, qIntListToStr, qIntListToLiteral, "`int$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_LONG, qLongListToStr, qLongListToLiteral, "`long$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_REAL, qRealListToStr, qRealListToLiteral, "`real$()", INT_MIN);
    ASSERT_EMPTY_LISTS(Q_TYPE_FLOAT, qFloatListToStr, qFloatListToLiteral, "`float$()", INT_MIN);
    ASSERT_EMPTY_LISTS(Q_TYPE_CHAR, qCharListToStr, qCharListToLiteral, "\"\"");
    ASSERT_EMPTY_LISTS(Q_TYPE_SYMBOL, qSymbolListToStr, qSymbolListToLiteral, "`symbol$()");
    ASSERT_EMPTY_LISTS(
        Q_TYPE_TIMESTAMP, qTimestampListToStr, qTimestampListToLiteral, "`timestamp$()"
    );
    ASSERT_EMPTY_LISTS(Q_TYPE_MONTH, qMonthListToStr, qMonthListToLiteral, "`month$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_DATE, qDateListToStr, qDateListToLiteral, "`date$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_DATETIME, qDatetimeListToStr, qDatetimeListToLiteral, "`datetime$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_TIMESPAN, qTimespanListToStr, qTimespanListToLiteral, "`timespan$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_MINUTE, qMinuteListToStr, qMinuteListToLiteral, "`minute$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_SECOND, qSecondListToStr, qSecondListToLiteral, "`second$()");
    ASSERT_EMPTY_LISTS(Q_TYPE_TIME, qTimeListToStr, qTimeListToLiteral, "`time$()");
}

#undef ASSERT_EMPTY_LISTS

int main() {
    UNITY_BEGIN();
    RUN_TEST(testEmptyLists);
    RUN_TEST(testSpecialAtoms);
    RUN_TEST(testSpecialLists);
    RUN_TEST(testSpecialPrecisionAndNaNs);
    RUN_TEST(testFiniteIntegerBoundaries);
    RUN_TEST(testEmptySymbolListItems);
    RUN_TEST(testDynamicSymbolBufferSizes);
    RUN_TEST(testDynamicFloatBufferSizes);
    RUN_TEST(testFloatLiteralSuffix);
    RUN_TEST(testFloatListLiteralSuffix);
    RUN_TEST(testCharListEmbeddedNull);
    RUN_TEST(testPrecisionRange);
    RUN_TEST(testMaximumPrecision);
    RUN_TEST(testDisplayPrecisionLists);
    RUN_TEST(testPrecisionRounding);
    RUN_TEST(testGenericNullInputs);
    RUN_TEST(testNullSymbolInputs);
    RUN_TEST(testFormattedCountLimits);

    RUN_TEST(testBooleanToStr);
    RUN_TEST(testBooleanToLiteral);
    RUN_TEST(testGuidToStr);
    RUN_TEST(testGuidToLiteral);
    RUN_TEST(testByteToStr);
    RUN_TEST(testByteToLiteral);
    RUN_TEST(testShortToStr);
    RUN_TEST(testShortToLiteral);
    RUN_TEST(testIntToStr);
    RUN_TEST(testIntToLiteral);
    RUN_TEST(testLongToStr);
    RUN_TEST(testLongToLiteral);
    RUN_TEST(testRealToStr);
    RUN_TEST(testRealToLiteral);
    RUN_TEST(testFloatToStr);
    RUN_TEST(testFloatToLiteral);
    RUN_TEST(testCharToStr);
    RUN_TEST(testCharToLiteral);
    RUN_TEST(testCharLiteralEscapes);
    RUN_TEST(testCharListLiteralAllBytes);
    RUN_TEST(testCharListLiteralEscapes);
    RUN_TEST(testSymbolToStr);
    RUN_TEST(testSymbolToLiteral);
    RUN_TEST(testTimestampToStr);
    RUN_TEST(testTimestampToLiteral);
    RUN_TEST(testMonthToStr);
    RUN_TEST(testMonthToLiteral);
    RUN_TEST(testDateToStr);
    RUN_TEST(testDateToLiteral);
    RUN_TEST(testDatetimeToStr);
    RUN_TEST(testDatetimeToLiteral);
    RUN_TEST(testDatetimeInvalidRange);
    RUN_TEST(testDatetimeCalendarAndDisplay);
    RUN_TEST(testDatetimeMicrosecondBoundaries);
    RUN_TEST(testYearFormattingBufferBounds);
    RUN_TEST(testDateArithmeticBoundaries);
    RUN_TEST(testTimestampRangeAndNegativeFraction);
    RUN_TEST(testTimespanToStr);
    RUN_TEST(testTimespanToLiteral);
    RUN_TEST(testTimespanExtremes);
    RUN_TEST(testMinuteToStr);
    RUN_TEST(testMinuteToLiteral);
    RUN_TEST(testSecondToStr);
    RUN_TEST(testSecondToLiteral);
    RUN_TEST(testTimeToStr);
    RUN_TEST(testTimeToLiteral);
    RUN_TEST(testAtomToStr);
    RUN_TEST(testAtomToLiteral);

    RUN_TEST(testBooleanListToStr);
    RUN_TEST(testBooleanListToLiteral);
    RUN_TEST(testGuidListToStr);
    RUN_TEST(testGuidListToLiteral);
    RUN_TEST(testByteListToStr);
    RUN_TEST(testByteListToLiteral);
    RUN_TEST(testByteListToLiteralBufferBounds);
    RUN_TEST(testShortListToStr);
    RUN_TEST(testShortListToLiteral);
    RUN_TEST(testIntListToStr);
    RUN_TEST(testIntListToLiteral);
    RUN_TEST(testLongListToStr);
    RUN_TEST(testLongListToLiteral);
    RUN_TEST(testRealListToStr);
    RUN_TEST(testRealListToLiteral);
    RUN_TEST(testFloatListToStr);
    RUN_TEST(testFloatListToLiteral);
    RUN_TEST(testCharListToStr);
    RUN_TEST(testCharListToLiteral);
    RUN_TEST(testCharListToLiteralBufferBounds);
    RUN_TEST(testSymbolListToStr);
    RUN_TEST(testSymbolListToLiteral);
    RUN_TEST(testTimestampListToStr);
    RUN_TEST(testTimestampListToLiteral);
    RUN_TEST(testMonthListToStr);
    RUN_TEST(testMonthListToLiteral);
    RUN_TEST(testDateListToStr);
    RUN_TEST(testDateListToLiteral);
    RUN_TEST(testDatetimeListToStr);
    RUN_TEST(testDatetimeListToLiteral);
    RUN_TEST(testTimespanListToStr);
    RUN_TEST(testTimespanListToLiteral);
    RUN_TEST(testTimespanListExtremes);
    RUN_TEST(testMinuteListToStr);
    RUN_TEST(testMinuteListToLiteral);
    RUN_TEST(testSecondListToStr);
    RUN_TEST(testSecondListToLiteral);
    RUN_TEST(testTimeListToStr);
    RUN_TEST(testTimeListToLiteral);
    RUN_TEST(testListToStr);
    RUN_TEST(testListToLiteral);
    RUN_TEST(testMonthBoundaries);
    RUN_TEST(testDurationIntegerExtremes);
    RUN_TEST(testGuidListFromQObject);

    return UNITY_END();
}
