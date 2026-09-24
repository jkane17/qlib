
/**
 * @file test_fmt.c
 * @brief Unit tests for fmt.h and fmt.c.
 */

#include <string.h>

#include "fmt.h"
#include "q.h"
#include "type.h"
#include "unity.h"

#define SIZE 128
char buffer[SIZE];

#define ASSERT_LIST(func, list, length, fullStr, singleStr)                                        \
    int result = func(buffer, SIZE, list, length);                                                 \
    TEST_ASSERT_EQUAL_INT(strlen(fullStr), result);                                                \
    TEST_ASSERT_EQUAL_STRING(fullStr, buffer);                                                     \
                                                                                                   \
    result = func(buffer, SIZE, list, 1);                                                          \
    TEST_ASSERT_EQUAL_INT(strlen(singleStr), result);                                              \
    TEST_ASSERT_EQUAL_STRING(singleStr, buffer);                                                   \
                                                                                                   \
    result = func(buffer, SIZE, list, 0);                                                          \
    TEST_ASSERT_EQUAL_INT(0, result);                                                              \
                                                                                                   \
    result = func(nullptr, SIZE, list, 3);                                                         \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);                                               \
                                                                                                   \
    result = func(buffer, 0, list, 3);                                                             \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result)

#define ASSERT_PRECISON_LIST(func, list, length, precision, fullStr, singleStr)                    \
    int result = func(buffer, SIZE, list, length, precision);                                      \
    TEST_ASSERT_EQUAL_INT(strlen(fullStr), result);                                                \
    TEST_ASSERT_EQUAL_STRING(fullStr, buffer);                                                     \
                                                                                                   \
    result = func(buffer, SIZE, list, 1, precision);                                               \
    TEST_ASSERT_EQUAL_INT(strlen(singleStr), result);                                              \
    TEST_ASSERT_EQUAL_STRING(singleStr, buffer);                                                   \
                                                                                                   \
    result = func(buffer, SIZE, list, 0, precision);                                               \
    TEST_ASSERT_EQUAL_INT(0, result);                                                              \
                                                                                                   \
    result = func(nullptr, SIZE, list, 3, precision);                                              \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);                                               \
                                                                                                   \
    result = func(buffer, 0, list, 3, precision);                                                  \
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result)

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

void testGuidToStr() {
    QGuid guid = {{0x12,
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
                   0x88}};

    int result = qGuidToStr(buffer, SIZE, &guid);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788", buffer);

    result = qGuidToStr(buffer, SIZE, nullptr);
    TEST_ASSERT_EQUAL_INT(0, result);

    result = qGuidToStr(nullptr, SIZE, &guid);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qGuidToStr(buffer, 0, &guid);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testGuidToLiteral() {
    QGuid guid = {{0x12,
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
                   0x88}};

    int result = qGuidToLiteral(buffer, SIZE, &guid);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788", buffer);

    result = qGuidToLiteral(buffer, SIZE, nullptr);
    TEST_ASSERT_EQUAL_INT(0, result);

    result = qGuidToLiteral(nullptr, SIZE, &guid);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qGuidToLiteral(buffer, 0, &guid);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
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

void testLongToStr() {
    QLong long_ = 1;
    int result = qLongToStr(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    long_ = 1234567890123456789;
    result = qLongToStr(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(19, result);
    TEST_ASSERT_EQUAL_STRING("1234567890123456789", buffer);

    // Require size >= 20
    result = qLongToStr(buffer, 19, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    long_ = -1234567890123456789;
    result = qLongToStr(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(20, result);
    TEST_ASSERT_EQUAL_STRING("-1234567890123456789", buffer);

    // Require size >= 21
    result = qLongToStr(buffer, 20, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qLongToStr(nullptr, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qLongToStr(buffer, 0, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testLongToLiteral() {
    QLong long_ = 1;
    int result = qLongToLiteral(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    long_ = 1234567890123456789;
    result = qLongToLiteral(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(19, result);
    TEST_ASSERT_EQUAL_STRING("1234567890123456789", buffer);

    // Require size >= 20
    result = qLongToLiteral(buffer, 19, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    long_ = -1234567890123456789;
    result = qLongToLiteral(buffer, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(20, result);
    TEST_ASSERT_EQUAL_STRING("-1234567890123456789", buffer);

    // Require size >= 21
    result = qLongToLiteral(buffer, 20, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qLongToLiteral(nullptr, SIZE, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qLongToLiteral(buffer, 0, long_);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testRealToStr() {
    QReal real = 3.14;
    int result = qRealToStr(buffer, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer);

    result = qRealToStr(buffer, SIZE, real, 4);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("3.1400", buffer);

    // Real only accurate up to 6 decimal places
    real = 3.1415926535;
    result = qRealToStr(buffer, SIZE, real, 10);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("3.141592", buffer, 8);

    real = -3.14;
    result = qRealToStr(buffer, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("-3.14", buffer);

    // Require size >= 6
    result = qRealToStr(buffer, 5, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qRealToStr(nullptr, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qRealToStr(buffer, 0, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testRealToLiteral() {
    QReal real = 3.14;
    int result = qRealToLiteral(buffer, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("3.14e", buffer);

    result = qRealToLiteral(buffer, SIZE, real, 4);
    TEST_ASSERT_EQUAL_INT(7, result);
    TEST_ASSERT_EQUAL_STRING("3.1400e", buffer);

    // Real only accurate up to 6 decimal places
    real = 3.1415926535;
    result = qRealToLiteral(buffer, SIZE, real, 10);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("3.141592e", buffer, 8);

    real = -3.14;
    result = qRealToLiteral(buffer, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("-3.14e", buffer);

    // Require size >= 7
    result = qRealToLiteral(buffer, 6, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qRealToLiteral(nullptr, SIZE, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qRealToLiteral(buffer, 0, real, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testFloatToStr() {
    QFloat float_ = 3.14;
    int result = qFloatToStr(buffer, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer);

    result = qFloatToStr(buffer, SIZE, float_, 4);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("3.1400", buffer);

    // Float only accurate up to 15 decimal places
    float_ = 3.141592653589793238462;
    result = qFloatToStr(buffer, SIZE, float_, 21);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("3.141592653589793", buffer, 17);

    float_ = -3.14;
    result = qFloatToStr(buffer, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("-3.14", buffer);

    // Require size >= 6
    result = qFloatToStr(buffer, 5, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qFloatToStr(nullptr, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qFloatToStr(buffer, 0, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testFloatToLiteral() {
    QFloat float_ = 3.14;
    int result = qFloatToLiteral(buffer, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("3.14", buffer);

    result = qFloatToLiteral(buffer, SIZE, float_, 4);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("3.1400", buffer);

    // Float only accurate up to 15 decimal places
    float_ = 3.141592653589793238462;
    result = qFloatToLiteral(buffer, SIZE, float_, 21);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("3.141592653589793", buffer, 17);

    float_ = -3.14;
    result = qFloatToLiteral(buffer, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("-3.14", buffer);

    // Require size >= 6
    result = qFloatToLiteral(buffer, 5, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qFloatToLiteral(nullptr, SIZE, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qFloatToLiteral(buffer, 0, float_, 2);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
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
    TEST_ASSERT_EQUAL_INT(3, result);
    TEST_ASSERT_EQUAL_STRING("\"\n\"", buffer);

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

void testTimestampToStr() {
    QTimestamp timestamp = 821977817123456789;
    int result = qTimestampToStr(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    // Require size >= 30
    result = qTimestampToStr(buffer, 29, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timestamp = -124793382876543211;
    result = qTimestampToStr(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17D15:10:17.123456789", buffer);

    result = qTimestampToStr(nullptr, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimestampToStr(buffer, 0, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimestampToLiteral() {
    QTimestamp timestamp = 821977817123456789;
    int result = qTimestampToLiteral(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    // Require size >= 30
    result = qTimestampToLiteral(buffer, 29, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timestamp = -124793382876543211;
    result = qTimestampToLiteral(buffer, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(29, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17D15:10:17.123456789", buffer);

    result = qTimestampToLiteral(nullptr, SIZE, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimestampToLiteral(buffer, 0, timestamp);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
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

void testDateToStr() {
    QDate date = 9513;
    int result = qDateToStr(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    // Require size >= 11
    result = qDateToStr(buffer, 10, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    date = -1445;
    result = qDateToStr(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17", buffer);

    result = qDateToStr(nullptr, SIZE, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qDateToStr(buffer, 0, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testDateToLiteral() {
    QDate date = 9513;
    int result = qDateToLiteral(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    // Require size >= 11
    result = qDateToLiteral(buffer, 10, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    date = -1445;
    result = qDateToLiteral(buffer, SIZE, date);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17", buffer);

    result = qDateToLiteral(nullptr, SIZE, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qDateToLiteral(buffer, 0, date);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testDatetimeToStr() {
    QDatetime datetime = 9513.6321426273153;
    int result = qDatetimeToStr(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    // Require size >= 24
    result = qDatetimeToStr(buffer, 23, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    datetime = -1444.3678573726852;
    result = qDatetimeToStr(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17T15:10:17.123", buffer);

    result = qDatetimeToStr(nullptr, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qDatetimeToStr(buffer, 0, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testDatetimeToLiteral() {
    QDatetime datetime = 9513.6321426273153;
    int result = qDatetimeToLiteral(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    // Require size >= 24
    result = qDatetimeToLiteral(buffer, 23, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    datetime = -1444.3678573726852;
    result = qDatetimeToLiteral(buffer, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(23, result);
    TEST_ASSERT_EQUAL_STRING("1996.01.17T15:10:17.123", buffer);

    result = qDatetimeToLiteral(nullptr, SIZE, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qDatetimeToLiteral(buffer, 0, datetime);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimespanToStr() {
    QTimespan timespan = 1473576123456789;
    int result = qTimespanToStr(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(21, result);
    TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    // Require size >= 22
    result = qTimespanToStr(buffer, 21, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timespan = -1473576123456789;
    result = qTimespanToStr(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(22, result);
    TEST_ASSERT_EQUAL_STRING("-17D01:19:36.123456789", buffer);

    result = qTimespanToStr(nullptr, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimespanToStr(buffer, 0, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimespanToLiteral() {
    QTimespan timespan = 1473576123456789;
    int result = qTimespanToLiteral(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(21, result);
    TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    // Require size >= 22
    result = qTimespanToLiteral(buffer, 21, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    timespan = -1473576123456789;
    result = qTimespanToLiteral(buffer, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(22, result);
    TEST_ASSERT_EQUAL_STRING("-17D01:19:36.123456789", buffer);

    result = qTimespanToLiteral(nullptr, SIZE, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimespanToLiteral(buffer, 0, timespan);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testMinuteToStr() {
    QMinute minute = 980;
    int result = qMinuteToStr(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    // Require size >= 6
    result = qMinuteToStr(buffer, 5, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    minute = -980;
    result = qMinuteToStr(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("-16:20", buffer);

    minute = 6000;
    result = qMinuteToStr(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("100:00", buffer);

    result = qMinuteToStr(nullptr, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qMinuteToStr(buffer, 0, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testMinuteToLiteral() {
    QMinute minute = 980;
    int result = qMinuteToLiteral(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    // Require size >= 6
    result = qMinuteToLiteral(buffer, 5, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    minute = -980;
    result = qMinuteToLiteral(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("-16:20", buffer);

    minute = 6000;
    result = qMinuteToLiteral(buffer, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("100:00", buffer);

    result = qMinuteToLiteral(nullptr, SIZE, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qMinuteToLiteral(buffer, 0, minute);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testSecondToStr() {
    QSecond second = 58817;
    int result = qSecondToStr(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    // Require size >= 9
    result = qSecondToStr(buffer, 8, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    second = -58817;
    result = qSecondToStr(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17", buffer);

    second = 360000;
    result = qSecondToStr(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00", buffer);

    result = qSecondToStr(nullptr, SIZE, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qSecondToStr(buffer, 0, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testSecondToLiteral() {
    QSecond second = 58817;
    int result = qSecondToLiteral(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    // Require size >= 9
    result = qSecondToLiteral(buffer, 8, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    second = -58817;
    result = qSecondToLiteral(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17", buffer);

    second = 360000;
    result = qSecondToLiteral(buffer, SIZE, second);
    TEST_ASSERT_EQUAL_INT(9, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00", buffer);

    result = qSecondToLiteral(nullptr, SIZE, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qSecondToLiteral(buffer, 0, second);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimeToStr() {
    QTime time = 58817123;
    int result = qTimeToStr(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);

    // Require size >= 13
    result = qTimeToStr(buffer, 12, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    time = -58817123;
    result = qTimeToStr(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17.123", buffer);

    time = 360000000;
    result = qTimeToStr(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00.000", buffer);

    result = qTimeToStr(nullptr, SIZE, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimeToStr(buffer, 0, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testTimeToLiteral() {
    QTime time = 58817123;
    int result = qTimeToLiteral(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(12, result);
    TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);

    // Require size >= 13
    result = qTimeToLiteral(buffer, 12, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    time = -58817123;
    result = qTimeToLiteral(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("-16:20:17.123", buffer);

    time = 360000000;
    result = qTimeToLiteral(buffer, SIZE, time);
    TEST_ASSERT_EQUAL_INT(13, result);
    TEST_ASSERT_EQUAL_STRING("100:00:00.000", buffer);

    result = qTimeToLiteral(nullptr, SIZE, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);

    result = qTimeToLiteral(buffer, 0, time);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_SIZE, result);
}

void testAtomToStr() {
    int result;

    QBoolean boolean = 1;
    result = qAtomToStr(buffer, SIZE, &boolean, Q_TYPE_BOOLEAN, 0);
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_STRING("1", buffer);

    QGuid guid = {{0x12,
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
                   0x88}};
    result = qAtomToStr(buffer, SIZE, &guid, Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788", buffer);

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
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("3.142", buffer); // Precision rounding

    QFloat float_ = 3.14159;
    result = qAtomToStr(buffer, SIZE, &float_, Q_TYPE_FLOAT, 3);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("3.142", buffer);

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

    QGuid guid = {{0x12,
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
                   0x88}};
    result = qAtomToLiteral(buffer, SIZE, &guid, Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(36, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788", buffer);

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
    TEST_ASSERT_EQUAL_INT(6, result);
    TEST_ASSERT_EQUAL_STRING("3.142e", buffer);

    QFloat float_ = 3.14159;
    result = qAtomToLiteral(buffer, SIZE, &float_, Q_TYPE_FLOAT, 3);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("3.142", buffer);

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
    ASSERT_LIST(qBooleanListToLiteral, booleans, 3, "101b", "1b");
}

void testGuidListToStr() {
    // Three different guids, so that items read from the wrong position would be detected
    QGuid guids[] = {
        {{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
        {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}},
        {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}},
    };

    ASSERT_LIST(qGuidListToStr,
                guids,
                3,
                "12345678-9abc-def0-1122-334455667788 "
                "00112233-4455-6677-8899-aabbccddeeff "
                "ffeeddcc-bbaa-9988-7766-554433221100",
                "12345678-9abc-def0-1122-334455667788");
}

void testGuidListToLiteral() {
    // Three different guids, so that items read from the wrong position would be detected
    QGuid guids[] = {
        {{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
        {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}},
        {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}},
    };

    ASSERT_LIST(qGuidListToLiteral,
                guids,
                3,
                "12345678-9abc-def0-1122-334455667788 "
                "00112233-4455-6677-8899-aabbccddeeff "
                "ffeeddcc-bbaa-9988-7766-554433221100",
                "12345678-9abc-def0-1122-334455667788");
}

void testByteListToStr() {
    QByte bytes[] = {1, 123, 255};
    ASSERT_LIST(qByteListToStr, bytes, 3, "01 7b ff", "01");
}

void testByteListToLiteral() {
    QByte bytes[] = {1, 123, 255};
    ASSERT_LIST(qByteListToLiteral, bytes, 3, "0x017bff", "0x01");
}

void testShortListToStr() {
    QShort shorts[] = {1, -234, 567};
    ASSERT_LIST(qShortListToStr, shorts, 3, "1 -234 567", "1");
}

void testShortListToLiteral() {
    QShort shorts[] = {1, -234, 567};
    ASSERT_LIST(qShortListToLiteral, shorts, 3, "1 -234 567h", "1h");
}

void testIntListToStr() {
    QInt ints[] = {1, -234, 567};
    ASSERT_LIST(qIntListToStr, ints, 3, "1 -234 567", "1");
}

void testIntListToLiteral() {
    QInt ints[] = {1, -234, 567};
    ASSERT_LIST(qIntListToLiteral, ints, 3, "1 -234 567i", "1i");
}

void testLongListToStr() {
    QLong longs[] = {1, -234, 567};
    ASSERT_LIST(qLongListToStr, longs, 3, "1 -234 567", "1");
}

void testLongListToLiteral() {
    QLong longs[] = {1, -234, 567};
    ASSERT_LIST(qLongListToLiteral, longs, 3, "1 -234 567", "1");
}

void testRealListToStr() {
    QReal reals[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISON_LIST(qRealListToStr, reals, 3, 2, "3.14 -3.14 0.16", "3.14");
}

void testRealListToLiteral() {
    QReal reals[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISON_LIST(qRealListToLiteral, reals, 3, 2, "3.14 -3.14 0.16e", "3.14e");
}

void testFloatListToStr() {
    QFloat floats[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISON_LIST(qFloatListToStr, floats, 3, 2, "3.14 -3.14 0.16", "3.14");
}

void testFloatListToLiteral() {
    QFloat floats[] = {3.14, -3.14, 0.159};
    ASSERT_PRECISON_LIST(qFloatListToLiteral, floats, 3, 2, "3.14 -3.14 0.16", "3.14");
}

void testCharListToStr() {
    QChar chars[] = {'a', 'b', 'c'};
    ASSERT_LIST(qCharListToStr, chars, 3, "a b c", "a");
}

void testCharListToLiteral() {
    QChar chars[] = {'a', 'b', 'c'};
    ASSERT_LIST(qCharListToLiteral, chars, 3, "\"abc\"", "\"a\"");
}

void testSymbolListToStr() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    ASSERT_LIST(qSymbolListToStr, symbols, 3, "abc def ghi", "abc");
}

void testSymbolListToLiteral() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    ASSERT_LIST(qSymbolListToLiteral, symbols, 3, "`abc`def`ghi", "`abc");
}

void testTimestampListToStr() {
    QTimestamp timestamps[] = {821977817123456789, 0, -124793382876543211};
    ASSERT_LIST(
        qTimestampListToStr,
        timestamps,
        3,
        "2026.01.17D15:10:17.123456789 2000.01.01D00:00:00.000000000 1996.01.17D15:10:17.123456789",
        "2026.01.17D15:10:17.123456789");
}

void testTimestampListToLiteral() {
    QTimestamp timestamps[] = {821977817123456789, 0, -124793382876543211};
    ASSERT_LIST(
        qTimestampListToLiteral,
        timestamps,
        3,
        "2026.01.17D15:10:17.123456789 2000.01.01D00:00:00.000000000 1996.01.17D15:10:17.123456789",
        "2026.01.17D15:10:17.123456789");
}

void testMonthListToStr() {
    QMonth months[] = {312, 0, -48};
    ASSERT_LIST(qMonthListToStr, months, 3, "2026.01 2000.01 1996.01", "2026.01");
}

void testMonthListToLiteral() {
    QMonth months[] = {312, 0, -48};
    ASSERT_LIST(qMonthListToLiteral, months, 3, "2026.01 2000.01 1996.01m", "2026.01m");
}

void testDateListToStr() {
    QDate dates[] = {9513, 0, -1445};
    ASSERT_LIST(qDateListToStr, dates, 3, "2026.01.17 2000.01.01 1996.01.17", "2026.01.17");
}

void testDateListToLiteral() {
    QDate dates[] = {9513, 0, -1445};
    ASSERT_LIST(qDateListToLiteral, dates, 3, "2026.01.17 2000.01.01 1996.01.17", "2026.01.17");
}

void testDatetimeListToStr() {
    QDatetime datetimes[] = {9513.6321426273153, 0, -1444.3678573726852};
    ASSERT_LIST(qDatetimeListToStr,
                datetimes,
                3,
                "2026.01.17T15:10:17.123 2000.01.01T00:00:00.000 1996.01.17T15:10:17.123",
                "2026.01.17T15:10:17.123");
}

void testDatetimeListToLiteral() {
    QDatetime datetimes[] = {9513.6321426273153, 0, -1444.3678573726852};
    ASSERT_LIST(qDatetimeListToLiteral,
                datetimes,
                3,
                "2026.01.17T15:10:17.123 2000.01.01T00:00:00.000 1996.01.17T15:10:17.123",
                "2026.01.17T15:10:17.123");
}

void testTimespanListToStr() {
    QTimespan timespans[] = {1473576123456789, 0, -1473576123456789};
    ASSERT_LIST(qTimespanListToStr,
                timespans,
                3,
                "17D01:19:36.123456789 0D00:00:00.000000000 -17D01:19:36.123456789",
                "17D01:19:36.123456789");
}

void testTimespanListToLiteral() {
    QTimespan timespans[] = {1473576123456789, 0, -1473576123456789};
    ASSERT_LIST(qTimespanListToLiteral,
                timespans,
                3,
                "17D01:19:36.123456789 0D00:00:00.000000000 -17D01:19:36.123456789",
                "17D01:19:36.123456789");
}

void testMinuteListToStr() {
    QMinute minutes[] = {980, 0, -980};
    ASSERT_LIST(qMinuteListToStr, minutes, 3, "16:20 00:00 -16:20", "16:20");
}

void testMinuteListToLiteral() {
    QMinute minutes[] = {980, 0, -980};
    ASSERT_LIST(qMinuteListToLiteral, minutes, 3, "16:20 00:00 -16:20", "16:20");
}

void testSecondListToStr() {
    QSecond seconds[] = {58817, 0, -58817};
    ASSERT_LIST(qSecondListToStr, seconds, 3, "16:20:17 00:00:00 -16:20:17", "16:20:17");
}

void testSecondListToLiteral() {
    QSecond seconds[] = {58817, 0, -58817};
    ASSERT_LIST(qSecondListToLiteral, seconds, 3, "16:20:17 00:00:00 -16:20:17", "16:20:17");
}

void testTimeListToStr() {
    QTime times[] = {58817123, 0, -58817123};
    ASSERT_LIST(
        qTimeListToStr, times, 3, "16:20:17.123 00:00:00.000 -16:20:17.123", "16:20:17.123");
}

void testTimeListToLiteral() {
    QTime times[] = {58817123, 0, -58817123};
    ASSERT_LIST(
        qTimeListToLiteral, times, 3, "16:20:17.123 00:00:00.000 -16:20:17.123", "16:20:17.123");
}

void testGuidListFromQObject() {
    // The items of a Q guid list are stored contiguously and can be formatted directly
    QGuid values[] = {
        {{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
        {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}},
    };
    QObj *list = qNewGuidList(values, 2);

    int result = qListToStr(buffer, SIZE, list->list, list->length, Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(73, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788 "
                             "00112233-4455-6677-8899-aabbccddeeff",
                             buffer);

    result = qGuidListToLiteral(buffer, SIZE, (const QGuid *)list->list, list->length);
    TEST_ASSERT_EQUAL_INT(73, result);

    qDecRef(list);
}

void testListToLiteral() {
    int result;

    QBoolean booleans[] = {1, 0, 1};
    result = qListToLiteral(buffer, SIZE, booleans, 3, Q_TYPE_BOOLEAN, 0);
    TEST_ASSERT_EQUAL_INT(4, result);
    TEST_ASSERT_EQUAL_STRING("101b", buffer);

    QByte bytes[] = {1, 123, 255};
    result = qListToLiteral(buffer, SIZE, bytes, 3, Q_TYPE_BYTE, 0);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("0x017bff", buffer);

    QInt ints[] = {1, -2, 3};
    result = qListToLiteral(buffer, SIZE, ints, 3, Q_TYPE_INT, 0);
    TEST_ASSERT_EQUAL_STRING("1 -2 3i", buffer);

    QReal reals[] = {1.5f, 2.25f};
    result = qListToLiteral(buffer, SIZE, reals, 2, Q_TYPE_REAL, 2);
    TEST_ASSERT_EQUAL_STRING("1.50 2.25e", buffer);

    QChar chars[] = {'a', 'b'};
    result = qListToLiteral(buffer, SIZE, chars, 2, Q_TYPE_CHAR, 0);
    TEST_ASSERT_EQUAL_STRING("\"ab\"", buffer);

    QSymbol symbols[] = {"a", "b"};
    result = qListToLiteral(buffer, SIZE, symbols, 2, Q_TYPE_SYMBOL, 0);
    TEST_ASSERT_EQUAL_STRING("`a`b", buffer);

    // Unsupported type
    result = qListToLiteral(buffer, SIZE, ints, 3, Q_TYPE_MIXED, 0);
    TEST_ASSERT_EQUAL_INT(Q_FMT_ERROR_TYPE, result);
}

void testListToStr() {
    int result;

    QBoolean booleans[] = {1, 0, 1};
    result = qListToStr(buffer, SIZE, booleans, 3, Q_TYPE_BOOLEAN, 0);
    TEST_ASSERT_EQUAL_INT(5, result);
    TEST_ASSERT_EQUAL_STRING("1 0 1", buffer);

    // Three different guids, so that items read from the wrong position would be detected
    QGuid guids[] = {
        {{0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}},
        {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}},
        {{0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00}},
    };
    result = qListToStr(buffer, SIZE, guids, 3, Q_TYPE_GUID, 0);
    TEST_ASSERT_EQUAL_INT(110, result);
    TEST_ASSERT_EQUAL_STRING("12345678-9abc-def0-1122-334455667788 "
                             "00112233-4455-6677-8899-aabbccddeeff "
                             "ffeeddcc-bbaa-9988-7766-554433221100",
                             buffer);

    QByte bytes[] = {1, 123, 255};
    result = qListToStr(buffer, SIZE, bytes, 3, Q_TYPE_BYTE, 0);
    TEST_ASSERT_EQUAL_INT(8, result);
    TEST_ASSERT_EQUAL_STRING("01 7b ff", buffer);

    QShort shorts[] = {1, -234, 567};
    result = qListToStr(buffer, SIZE, shorts, 3, Q_TYPE_SHORT, 0);
    TEST_ASSERT_EQUAL_INT(10, result);
    TEST_ASSERT_EQUAL_STRING("1 -234 567", buffer);

    QInt ints[] = {1, -234, 567};
    result = qListToStr(buffer, SIZE, ints, 3, Q_TYPE_INT, 0);
    TEST_ASSERT_EQUAL_INT(10, result);

    QLong longs[] = {1, -234, 567};
    result = qListToStr(buffer, SIZE, longs, 3, Q_TYPE_LONG, 0);
    TEST_ASSERT_EQUAL_INT(10, result);

    // TODO: Continue

    // QReal real = 3.14159;
    // result = qAtomToStr(buffer, SIZE, &real, Q_TYPE_REAL, 3);
    // TEST_ASSERT_EQUAL_INT(5, result);
    // TEST_ASSERT_EQUAL_STRING("3.142", buffer); // Precision rounding

    // QFloat float_ = 3.14159;
    // result = qAtomToStr(buffer, SIZE, &float_, Q_TYPE_FLOAT, 3);
    // TEST_ASSERT_EQUAL_INT(5, result);
    // TEST_ASSERT_EQUAL_STRING("3.142", buffer);

    // QChar char_ = 'a';
    // result = qAtomToStr(buffer, SIZE, &char_, Q_TYPE_CHAR, 0);
    // TEST_ASSERT_EQUAL_INT(1, result);
    // TEST_ASSERT_EQUAL_STRING("a", buffer);

    // QSymbol symbol = "abc";
    // result = qAtomToStr(buffer, SIZE, symbol, Q_TYPE_SYMBOL, 0);
    // TEST_ASSERT_EQUAL_INT(3, result);
    // TEST_ASSERT_EQUAL_STRING("abc", buffer);

    // QTimestamp timestamp = 821977817123456789;
    // result = qAtomToStr(buffer, SIZE, &timestamp, Q_TYPE_TIMESTAMP, 0);
    // TEST_ASSERT_EQUAL_INT(29, result);
    // TEST_ASSERT_EQUAL_STRING("2026.01.17D15:10:17.123456789", buffer);

    // QMonth month = 312;
    // result = qAtomToStr(buffer, SIZE, &month, Q_TYPE_MONTH, 0);
    // TEST_ASSERT_EQUAL_INT(7, result);
    // TEST_ASSERT_EQUAL_STRING("2026.01", buffer);

    // QDate date = 9513;
    // result = qAtomToStr(buffer, SIZE, &date, Q_TYPE_DATE, 0);
    // TEST_ASSERT_EQUAL_INT(10, result);
    // TEST_ASSERT_EQUAL_STRING("2026.01.17", buffer);

    // QDatetime datetime = 9513.6321426273153;
    // result = qAtomToStr(buffer, SIZE, &datetime, Q_TYPE_DATETIME, 0);
    // TEST_ASSERT_EQUAL_INT(23, result);
    // TEST_ASSERT_EQUAL_STRING("2026.01.17T15:10:17.123", buffer);

    // QTimespan timespan = 1473576123456789;
    // result = qAtomToStr(buffer, SIZE, &timespan, Q_TYPE_TIMESPAN, 0);
    // TEST_ASSERT_EQUAL_INT(21, result);
    // TEST_ASSERT_EQUAL_STRING("17D01:19:36.123456789", buffer);

    // QMinute minute = 980;
    // result = qAtomToStr(buffer, SIZE, &minute, Q_TYPE_MINUTE, 0);
    // TEST_ASSERT_EQUAL_INT(5, result);
    // TEST_ASSERT_EQUAL_STRING("16:20", buffer);

    // QSecond second = 58817;
    // result = qAtomToStr(buffer, SIZE, &second, Q_TYPE_SECOND, 0);
    // TEST_ASSERT_EQUAL_INT(8, result);
    // TEST_ASSERT_EQUAL_STRING("16:20:17", buffer);

    // QTime time = 58817123;
    // result = qAtomToStr(buffer, SIZE, &time, Q_TYPE_TIME, 0);
    // TEST_ASSERT_EQUAL_INT(12, result);
    // TEST_ASSERT_EQUAL_STRING("16:20:17.123", buffer);
}

int main() {
    UNITY_BEGIN();

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
    RUN_TEST(testTimespanToStr);
    RUN_TEST(testTimespanToLiteral);
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
    RUN_TEST(testMinuteListToStr);
    RUN_TEST(testMinuteListToLiteral);
    RUN_TEST(testSecondListToStr);
    RUN_TEST(testSecondListToLiteral);
    RUN_TEST(testTimeListToStr);
    RUN_TEST(testTimeListToLiteral);
    RUN_TEST(testListToStr);
    RUN_TEST(testListToLiteral);
    RUN_TEST(testGuidListFromQObject);

    return UNITY_END();
}
