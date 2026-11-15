/**
 * @file test_list.c
 * @brief Unit tests for list.h.
 */

// For fork, waitpid and close (used by the assertion tests)
#define _POSIX_C_SOURCE 200809L

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "q.h"
#include "unity.h"

// The argument checks in list.h are asserts, so they can only be tested in debug builds. Each check
// is tested by running the call in a child process and checking that it aborts.
#if !defined(NDEBUG) && !defined(_WIN32)
#define TEST_ASSERTIONS 1
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#else
#define TEST_ASSERTIONS 0
#endif

void setUp() {
}
void tearDown() {
}

///// Helpers /////

// Create a list of the given type from raw values, append one raw value with qAppendValue, and check
// that the list holds the original values followed by the new one
static void checkAppendValue(QTypeCode type, size_t size, const void *values, QSize count,
                             void *value) {
    QObj *list = qNewList(type, count);
    TEST_ASSERT_NOT_NULL(list);
    if (count > 0)
        memcpy(list->list, values, count * size);

    QObj *result = qAppendValue(&list, value);

    TEST_ASSERT_EQUAL_PTR(list, result);
    TEST_ASSERT_EQUAL_INT(type, list->type);
    TEST_ASSERT_EQUAL_UINT64(count + 1, list->length);
    if (count > 0)
        TEST_ASSERT_EQUAL_MEMORY(values, list->list, count * size);
    TEST_ASSERT_EQUAL_MEMORY(value, list->list + count * size, size);

    qDecRef(list);
}

///// qAppendValue /////

void testAppendValueLong() {
    QLong longs[] = {1, 12, 123};
    QObj *list = qNewLongList(longs, 3);

    QLong value = 1234;
    QObj *result = qAppendValue(&list, &value);

    TEST_ASSERT_EQUAL_PTR(list, result);
    TEST_ASSERT_TRUE(qIsLongList(list));
    TEST_ASSERT_EQUAL_UINT64(4, list->length);
    TEST_ASSERT_EQUAL_INT64(1, qGetLongAtIndex(list, 0));
    TEST_ASSERT_EQUAL_INT64(12, qGetLongAtIndex(list, 1));
    TEST_ASSERT_EQUAL_INT64(123, qGetLongAtIndex(list, 2));
    TEST_ASSERT_EQUAL_INT64(1234, qGetLongAtIndex(list, 3));

    qDecRef(list);
}

void testAppendValueEveryType() {
    QBoolean booleans[] = {1, 0};
    QBoolean boolean = 1;
    checkAppendValue(Q_TYPE_BOOLEAN, sizeof(QBoolean), booleans, 2, &boolean);

    QGuid guids[] = {{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}}, {{0xff}}};
    QGuid guid = {{16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}};
    checkAppendValue(Q_TYPE_GUID, sizeof(QGuid), guids, 2, &guid);

    QByte bytes[] = {0x01, 0xfe};
    QByte byte = 0x7f;
    checkAppendValue(Q_TYPE_BYTE, sizeof(QByte), bytes, 2, &byte);

    QShort shorts[] = {1, -2};
    QShort short_ = 300;
    checkAppendValue(Q_TYPE_SHORT, sizeof(QShort), shorts, 2, &short_);

    QInt ints[] = {1, -2};
    QInt int_ = 70000;
    checkAppendValue(Q_TYPE_INT, sizeof(QInt), ints, 2, &int_);

    QLong longs[] = {1, -2};
    QLong long_ = 5000000000;
    checkAppendValue(Q_TYPE_LONG, sizeof(QLong), longs, 2, &long_);

    QReal reals[] = {1.5f, -2.5f};
    QReal real = 3.25f;
    checkAppendValue(Q_TYPE_REAL, sizeof(QReal), reals, 2, &real);

    QFloat floats[] = {1.5, -2.5};
    QFloat float_ = 3.14159;
    checkAppendValue(Q_TYPE_FLOAT, sizeof(QFloat), floats, 2, &float_);

    QChar chars[] = {'a', 'b'};
    QChar char_ = 'c';
    checkAppendValue(Q_TYPE_CHAR, sizeof(QChar), chars, 2, &char_);

    QSymbol symbols[] = {qInternString("abc"), qInternString("def")};
    QSymbol symbol = qInternString("ghi");
    checkAppendValue(Q_TYPE_SYMBOL, sizeof(QSymbol), symbols, 2, &symbol);

    QTimestamp timestamps[] = {0, 1000000000};
    QTimestamp timestamp = 2000000000;
    checkAppendValue(Q_TYPE_TIMESTAMP, sizeof(QTimestamp), timestamps, 2, &timestamp);

    QMonth months[] = {0, 12};
    QMonth month = 24;
    checkAppendValue(Q_TYPE_MONTH, sizeof(QMonth), months, 2, &month);

    QDate dates[] = {0, 365};
    QDate date = 730;
    checkAppendValue(Q_TYPE_DATE, sizeof(QDate), dates, 2, &date);

    QDatetime datetimes[] = {0.0, 1.5};
    QDatetime datetime = 2.25;
    checkAppendValue(Q_TYPE_DATETIME, sizeof(QDatetime), datetimes, 2, &datetime);

    QTimespan timespans[] = {0, 1000};
    QTimespan timespan = 2000;
    checkAppendValue(Q_TYPE_TIMESPAN, sizeof(QTimespan), timespans, 2, &timespan);

    QMinute minutes[] = {0, 60};
    QMinute minute = 120;
    checkAppendValue(Q_TYPE_MINUTE, sizeof(QMinute), minutes, 2, &minute);

    QSecond seconds[] = {0, 60};
    QSecond second = 120;
    checkAppendValue(Q_TYPE_SECOND, sizeof(QSecond), seconds, 2, &second);

    QTime times[] = {0, 1000};
    QTime time = 2000;
    checkAppendValue(Q_TYPE_TIME, sizeof(QTime), times, 2, &time);
}

void testAppendValueToEmptyList() {
    QLong long_ = 42;
    checkAppendValue(Q_TYPE_LONG, sizeof(QLong), NULL, 0, &long_);

    QChar char_ = 'x';
    checkAppendValue(Q_TYPE_CHAR, sizeof(QChar), NULL, 0, &char_);

    QGuid guid = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}};
    checkAppendValue(Q_TYPE_GUID, sizeof(QGuid), NULL, 0, &guid);

    QObj *list = qNewLongList(NULL, 0);
    TEST_ASSERT_EQUAL_UINT64(0, list->length);
    qAppendValue(&list, &long_);
    TEST_ASSERT_EQUAL_UINT64(1, list->length);
    TEST_ASSERT_EQUAL_INT64(42, qGetLongAtIndex(list, 0));
    qDecRef(list);
}

void testAppendValueSpecialValues() {
    QObj *longs = qNewLongList(NULL, 0);
    QLong longNull = Q_LONG_NULL, longInf = Q_LONG_INF, longNegInf = -Q_LONG_INF;
    qAppendValue(&longs, &longNull);
    qAppendValue(&longs, &longInf);
    qAppendValue(&longs, &longNegInf);
    TEST_ASSERT_EQUAL_UINT64(3, longs->length);
    TEST_ASSERT_EQUAL_INT64(Q_LONG_NULL, qGetLongAtIndex(longs, 0));
    TEST_ASSERT_EQUAL_INT64(Q_LONG_INF, qGetLongAtIndex(longs, 1));
    TEST_ASSERT_EQUAL_INT64(-Q_LONG_INF, qGetLongAtIndex(longs, 2));
    qDecRef(longs);

    QObj *floats = qNewFloatList(NULL, 0);
    QFloat floatNull = Q_FLOAT_NULL, floatInf = Q_FLOAT_INF, negZero = -0.0;
    qAppendValue(&floats, &floatNull);
    qAppendValue(&floats, &floatInf);
    qAppendValue(&floats, &negZero);
    TEST_ASSERT_TRUE(isnan(qGetFloatAtIndex(floats, 0)));
    TEST_ASSERT_TRUE(isinf(qGetFloatAtIndex(floats, 1)) && qGetFloatAtIndex(floats, 1) > 0);
    TEST_ASSERT_TRUE(qGetFloatAtIndex(floats, 2) == 0.0 && signbit(qGetFloatAtIndex(floats, 2)));
    qDecRef(floats);

    QObj *guids = qNewGuidList(NULL, 0);
    QGuid guidNull = Q_GUID_NULL;
    qAppendValue(&guids, &guidNull);
    TEST_ASSERT_EQUAL_MEMORY(Q_GUID_NULL.bytes, qGetGuidAtIndex(guids, 0)->bytes, 16);
    qDecRef(guids);
}

void testAppendValueInternedSymbol() {
    QSymbol symbols[] = {"abc"};
    QObj *list = qNewSymbolList(symbols, 1);

    QSymbol symbol = qInternString("def");
    qAppendValue(&list, &symbol);

    TEST_ASSERT_EQUAL_UINT64(2, list->length);
    TEST_ASSERT_EQUAL_PTR(symbol, qGetSymbolAtIndex(list, 1));
    TEST_ASSERT_EQUAL_STRING("def", qGetSymbolAtIndex(list, 1));
    qDecRef(list);
}

void testAppendValueCopiesValue() {
    QObj *list = qNewLongList(NULL, 0);
    QLong value = 1;
    qAppendValue(&list, &value);

    // Changing the source afterwards does not change the list
    value = 2;
    TEST_ASSERT_EQUAL_INT64(1, qGetLongAtIndex(list, 0));
    qDecRef(list);
}

void testAppendValueMany() {
    // Enough appends to force the list to be reallocated many times
    const QSize count = 100000;
    QObj *list = qNewLongList(NULL, 0);

    for (QLong i = 0; i < (QLong)count; i++) {
        QObj *result = qAppendValue(&list, &i);
        TEST_ASSERT_EQUAL_PTR(list, result);
    }

    TEST_ASSERT_EQUAL_UINT64(count, list->length);
    for (QSize i = 0; i < count; i++)
        TEST_ASSERT_EQUAL_INT64((QLong)i, qGetLongAtIndex(list, i));
    qDecRef(list);
}

void testAppendValueFromSameListWithReallocation() {
    // Fill a 16 MiB allocation (including the object header) so appending forces growth.
    // At this size the old allocation can be unmapped: passing its element directly to ja
    // used to read freed memory, whereas small allocations could hide the bug in cached memory.
    const QSize count = (((QSize)1 << 24) - sizeof(QObj)) / sizeof(QLong);
    QObj *list = qNewList(Q_TYPE_LONG, count);
    TEST_ASSERT_NOT_NULL(list);
    QLong *values = (QLong *)list->list;
    memset(values, 0, count * sizeof(*values));
    values[0] = 123456789;
    values[count - 1] = -987654321;
    uintptr_t originalAddress = (uintptr_t)list;

    QObj *result = qAppendValue(&list, &values[0]);

    TEST_ASSERT_TRUE((uintptr_t)list != originalAddress);
    TEST_ASSERT_EQUAL_PTR(list, result);
    TEST_ASSERT_EQUAL_UINT64(count + 1, list->length);
    TEST_ASSERT_EQUAL_INT64(123456789, qGetLongAtIndex(list, 0));
    TEST_ASSERT_EQUAL_INT64(-987654321, qGetLongAtIndex(list, count - 1));
    TEST_ASSERT_EQUAL_INT64(123456789, qGetLongAtIndex(list, count));
    qDecRef(list);
}

void testAppendValueManyChars() {
    // Single-byte elements, to check that the length and storage stay in step as the list grows
    const char *text = "the quick brown fox jumps over the lazy dog";
    QObj *list = qNewCharList(NULL, 0);
    for (QSize i = 0; i < 1000; i++) {
        QChar c = text[i % strlen(text)];
        qAppendValue(&list, &c);
    }

    TEST_ASSERT_EQUAL_UINT64(1000, list->length);
    for (QSize i = 0; i < 1000; i++)
        TEST_ASSERT_EQUAL_CHAR(text[i % strlen(text)], qGetCharAtIndex(list, i));
    qDecRef(list);
}

///// qAppendObject /////

void testAppendObject() {
    QObj *mixed = qNewMixedList(2, qNewSymbol("abc"), qNewLong(10));

    QObj *float_ = qNewFloat(3.14);
    QObj *result = qAppendObject(&mixed, float_);

    TEST_ASSERT_EQUAL_PTR(mixed, result);
    TEST_ASSERT_TRUE(qIsMixedList(mixed));
    TEST_ASSERT_EQUAL_UINT64(3, mixed->length);
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbol(qGetMixedAtIndex(mixed, 0)));
    TEST_ASSERT_EQUAL_INT64(10, qGetLong(qGetMixedAtIndex(mixed, 1)));
    TEST_ASSERT_EQUAL_PTR(float_, qGetMixedAtIndex(mixed, 2));
    TEST_ASSERT_EQUAL_DOUBLE(3.14, qGetFloat(qGetMixedAtIndex(mixed, 2)));

    qDecRef(mixed);
}

void testAppendObjectToEmptyList() {
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    qAppendObject(&mixed, qNewLong(1));

    TEST_ASSERT_EQUAL_UINT64(1, mixed->length);
    TEST_ASSERT_EQUAL_INT64(1, qGetLong(qGetMixedAtIndex(mixed, 0)));
    qDecRef(mixed);
}

void testAppendObjectKinds() {
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);

    QLong longs[] = {1, 2, 3};
    QObj *longList = qNewLongList(longs, 3);
    QObj *nested = qNewMixedList(1, qNewChar('z'));
    QObj *empty = qNewList(Q_TYPE_MIXED, 0);
    QObj *string = qNewCharListFromString("xyz");

    qAppendObject(&mixed, longList);
    qAppendObject(&mixed, nested);
    qAppendObject(&mixed, empty);
    qAppendObject(&mixed, string);

    TEST_ASSERT_EQUAL_UINT64(4, mixed->length);
    TEST_ASSERT_EQUAL_PTR(longList, qGetMixedAtIndex(mixed, 0));
    TEST_ASSERT_EQUAL_INT64(3, qGetLongAtIndex(qGetMixedAtIndex(mixed, 0), 2));
    TEST_ASSERT_EQUAL_PTR(nested, qGetMixedAtIndex(mixed, 1));
    TEST_ASSERT_EQUAL_CHAR('z', qGetChar(qGetMixedAtIndex(qGetMixedAtIndex(mixed, 1), 0)));
    TEST_ASSERT_EQUAL_PTR(empty, qGetMixedAtIndex(mixed, 2));
    TEST_ASSERT_EQUAL_UINT64(0, qGetMixedAtIndex(mixed, 2)->length);
    TEST_ASSERT_EQUAL_PTR(string, qGetMixedAtIndex(mixed, 3));

    // Releasing the list releases everything in it
    qDecRef(mixed);
}

void testAppendObjectDoesNotCollapse() {
    // Appending items of the same type keeps the list mixed (see qCollapseMixedList)
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    qAppendObject(&mixed, qNewLong(1));
    qAppendObject(&mixed, qNewLong(2));

    TEST_ASSERT_EQUAL_INT(Q_TYPE_MIXED, mixed->type);
    TEST_ASSERT_EQUAL_UINT64(2, mixed->length);
    qDecRef(mixed);
}

void testAppendObjectTakesOwnership() {
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    QObj *long_ = qNewLong(7);

    // The list takes over the caller's reference, rather than adding one
    qAppendObject(&mixed, long_);
    TEST_ASSERT_EQUAL_INT32(0, long_->refs);

    qDecRef(mixed);
}

void testAppendObjectKeepReference() {
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    QObj *long_ = qNewLong(7);

    qIncRef(long_);
    qAppendObject(&mixed, long_);
    TEST_ASSERT_EQUAL_INT32(1, long_->refs);

    // The object outlives the list, as the caller kept a reference
    qDecRef(mixed);
    TEST_ASSERT_EQUAL_INT32(0, long_->refs);
    TEST_ASSERT_EQUAL_INT64(7, qGetLong(long_));
    qDecRef(long_);
}

void testAppendObjectSameObjectTwice() {
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    QObj *long_ = qNewLong(7);

    // Each item in the list needs its own reference
    qIncRef(long_);
    qAppendObject(&mixed, long_);
    qAppendObject(&mixed, long_);

    TEST_ASSERT_EQUAL_UINT64(2, mixed->length);
    TEST_ASSERT_EQUAL_PTR(long_, qGetMixedAtIndex(mixed, 0));
    TEST_ASSERT_EQUAL_PTR(long_, qGetMixedAtIndex(mixed, 1));
    TEST_ASSERT_EQUAL_INT32(1, long_->refs);

    qDecRef(mixed);
}

void testAppendObjectMany() {
    const QSize count = 10000;
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);

    for (QSize i = 0; i < count; i++) {
        QObj *result = qAppendObject(&mixed, qNewLong((QLong)i));
        TEST_ASSERT_EQUAL_PTR(mixed, result);
    }

    TEST_ASSERT_EQUAL_UINT64(count, mixed->length);
    for (QSize i = 0; i < count; i++)
        TEST_ASSERT_EQUAL_INT64((QLong)i, qGetLong(qGetMixedAtIndex(mixed, i)));
    qDecRef(mixed);
}

///// qAppendSymbol /////

void testAppendSymbol() {
    QSymbol symbols[] = {"abc", "def", "ghi"};
    QObj *list = qNewSymbolList(symbols, 3);

    QObj *result = qAppendSymbol(&list, "jkl");

    TEST_ASSERT_EQUAL_PTR(list, result);
    TEST_ASSERT_TRUE(qIsSymbolList(list));
    TEST_ASSERT_EQUAL_UINT64(4, list->length);
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_STRING("def", qGetSymbolAtIndex(list, 1));
    TEST_ASSERT_EQUAL_STRING("ghi", qGetSymbolAtIndex(list, 2));
    TEST_ASSERT_EQUAL_STRING("jkl", qGetSymbolAtIndex(list, 3));

    qDecRef(list);
}

void testAppendSymbolInterns() {
    QObj *list = qNewSymbolList(NULL, 0);

    QChar buffer[] = "hello";
    qAppendSymbol(&list, buffer);

    // The list holds the interned symbol, not the caller's buffer
    QSymbol sym = qGetSymbolAtIndex(list, 0);
    TEST_ASSERT_EQUAL_PTR(qInternString("hello"), sym);
    TEST_ASSERT_NOT_EQUAL(buffer, sym);

    // Changing the buffer afterwards does not change the list
    buffer[0] = 'j';
    TEST_ASSERT_EQUAL_STRING("hello", qGetSymbolAtIndex(list, 0));

    qDecRef(list);
}

void testAppendSymbolAlreadyInterned() {
    QObj *list = qNewSymbolList(NULL, 0);

    QSymbol sym = qInternString("abc");
    qAppendSymbol(&list, sym);

    TEST_ASSERT_EQUAL_PTR(sym, qGetSymbolAtIndex(list, 0));
    qDecRef(list);
}

void testAppendSymbolNull() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendSymbol(&list, Q_SYMBOL_NULL);

    TEST_ASSERT_EQUAL_UINT64(1, list->length);
    TEST_ASSERT_EQUAL_CHAR('\0', qGetSymbolAtIndex(list, 0)[0]);
    TEST_ASSERT_EQUAL_PTR(qInternString(""), qGetSymbolAtIndex(list, 0));
    qDecRef(list);
}

void testAppendSymbolDuplicates() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendSymbol(&list, "abc");
    qAppendSymbol(&list, "abc");

    TEST_ASSERT_EQUAL_UINT64(2, list->length);
    TEST_ASSERT_EQUAL_PTR(qGetSymbolAtIndex(list, 0), qGetSymbolAtIndex(list, 1));
    qDecRef(list);
}

void testAppendSymbolUnusualCharacters() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendSymbol(&list, "with space");
    qAppendSymbol(&list, "caf\xc3\xa9"); // UTF-8
    qAppendSymbol(&list, "`back`tick");

    TEST_ASSERT_EQUAL_STRING("with space", qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_STRING("caf\xc3\xa9", qGetSymbolAtIndex(list, 1));
    TEST_ASSERT_EQUAL_STRING("`back`tick", qGetSymbolAtIndex(list, 2));
    qDecRef(list);
}

void testAppendSymbolMany() {
    const QSize count = 10000;
    QObj *list = qNewSymbolList(NULL, 0);
    char buffer[32];

    for (QSize i = 0; i < count; i++) {
        snprintf(buffer, sizeof(buffer), "sym%llu", (unsigned long long)i);
        QObj *result = qAppendSymbol(&list, buffer);
        TEST_ASSERT_EQUAL_PTR(list, result);
    }

    TEST_ASSERT_EQUAL_UINT64(count, list->length);
    for (QSize i = 0; i < count; i++) {
        snprintf(buffer, sizeof(buffer), "sym%llu", (unsigned long long)i);
        TEST_ASSERT_EQUAL_PTR(qInternString(buffer), qGetSymbolAtIndex(list, i));
    }
    qDecRef(list);
}

///// qAppendFixedSymbol /////

void testAppendFixedSymbol() {
    QSymbol symbols[] = {"abc"};
    QObj *list = qNewSymbolList(symbols, 1);

    // Prefix of a longer string
    QObj *result = qAppendFixedSymbol(&list, "hello world", 5);

    TEST_ASSERT_EQUAL_PTR(list, result);
    TEST_ASSERT_TRUE(qIsSymbolList(list));
    TEST_ASSERT_EQUAL_UINT64(2, list->length);
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_STRING("hello", qGetSymbolAtIndex(list, 1));
    TEST_ASSERT_EQUAL_PTR(qInternString("hello"), qGetSymbolAtIndex(list, 1));

    qDecRef(list);
}

void testAppendFixedSymbolNotNullTerminated() {
    QObj *list = qNewSymbolList(NULL, 0);

    QChar buffer[] = {'a', 'b', 'c'};
    qAppendFixedSymbol(&list, buffer, 3);

    QSymbol sym = qGetSymbolAtIndex(list, 0);
    TEST_ASSERT_EQUAL_PTR(qInternString("abc"), sym);
    TEST_ASSERT_NOT_EQUAL(buffer, sym);

    // Changing the buffer afterwards does not change the list
    buffer[0] = 'x';
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbolAtIndex(list, 0));

    qDecRef(list);
}

void testAppendFixedSymbolWholeString() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendFixedSymbol(&list, "xyz", 3);
    qAppendSymbol(&list, "xyz");

    TEST_ASSERT_EQUAL_PTR(qInternString("xyz"), qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_PTR(qGetSymbolAtIndex(list, 1), qGetSymbolAtIndex(list, 0));
    qDecRef(list);
}

void testAppendFixedSymbolStopsAtNull() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendFixedSymbol(&list, "ab\0cd", 5);

    TEST_ASSERT_EQUAL_STRING("ab", qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_PTR(qInternString("ab"), qGetSymbolAtIndex(list, 0));
    qDecRef(list);
}

void testAppendFixedSymbolEmpty() {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendFixedSymbol(&list, "xyz", 0);
    qAppendFixedSymbol(&list, NULL, 0);
    qAppendFixedSymbol(&list, "\0abc", 4);

    QSymbol empty = qInternString(Q_SYMBOL_NULL);
    TEST_ASSERT_EQUAL_UINT64(3, list->length);
    TEST_ASSERT_EQUAL_PTR(empty, qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_PTR(empty, qGetSymbolAtIndex(list, 1));
    TEST_ASSERT_EQUAL_PTR(empty, qGetSymbolAtIndex(list, 2));
    qDecRef(list);
}

void testAppendFixedSymbolTokens() {
    // Typical use: appending the fields of a buffer without copying them out first
    const QChar *text = "abc,de,,fghi";
    QObj *list = qNewSymbolList(NULL, 0);

    const QChar *start = text;
    for (const QChar *p = text;; p++) {
        if (*p == ',' || *p == '\0') {
            qAppendFixedSymbol(&list, start, (QSize)(p - start));
            if (*p == '\0')
                break;
            start = p + 1;
        }
    }

    const char *expected[] = {"abc", "de", "", "fghi"};
    TEST_ASSERT_EQUAL_UINT64(4, list->length);
    for (QSize i = 0; i < 4; i++)
        TEST_ASSERT_EQUAL_PTR(qInternString(expected[i]), qGetSymbolAtIndex(list, i));
    qDecRef(list);
}

void testAppendFixedSymbolMany() {
    const QSize count = 10000;
    QObj *list = qNewSymbolList(NULL, 0);
    char buffer[32];

    for (QSize i = 0; i < count; i++) {
        int length = snprintf(buffer, sizeof(buffer), "sym%llu", (unsigned long long)i);
        // Append without the final character
        QObj *result = qAppendFixedSymbol(&list, buffer, (QSize)length - 1);
        TEST_ASSERT_EQUAL_PTR(list, result);
    }

    TEST_ASSERT_EQUAL_UINT64(count, list->length);
    for (QSize i = 0; i < count; i++) {
        int length = snprintf(buffer, sizeof(buffer), "sym%llu", (unsigned long long)i);
        buffer[length - 1] = '\0';
        TEST_ASSERT_EQUAL_PTR(qInternString(buffer), qGetSymbolAtIndex(list, i));
    }
    qDecRef(list);
}

///// qAppendList /////

void testAppendList() {
    QLong leftLongs[] = {1, 12, 123};
    QObj *left = qNewLongList(leftLongs, 3);
    QLong rightLongs[] = {4, 45, 456};
    QObj *right = qNewLongList(rightLongs, 3);

    QObj *result = qAppendList(&left, right);

    TEST_ASSERT_EQUAL_PTR(left, result);
    TEST_ASSERT_TRUE(qIsLongList(left));
    TEST_ASSERT_EQUAL_UINT64(6, left->length);
    QLong expected[] = {1, 12, 123, 4, 45, 456};
    TEST_ASSERT_EQUAL_INT64_ARRAY(expected, left->list, 6);

    // The right list is unchanged
    TEST_ASSERT_EQUAL_UINT64(3, right->length);
    TEST_ASSERT_EQUAL_INT64_ARRAY(rightLongs, right->list, 3);

    qDecRef(right);
    qDecRef(left);
}

void testAppendListBorrowsRight() {
    QLong longs[] = {1, 2};
    QObj *left = qNewLongList(longs, 2);
    QObj *right = qNewLongList(longs, 2);

    // Hold an extra reference so that a release inside qAppendList would show in the count
    qIncRef(right);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_INT32(1, right->refs);

    qDecRef(right);
    TEST_ASSERT_EQUAL_INT32(0, right->refs);
    TEST_ASSERT_EQUAL_INT64(2, qGetLongAtIndex(right, 1));
    qDecRef(right);
    qDecRef(left);
}

void testAppendListEmpty() {
    QLong longs[] = {1, 2};

    // Empty right list
    QObj *left = qNewLongList(longs, 2);
    QObj *empty = qNewLongList(NULL, 0);
    QObj *result = qAppendList(&left, empty);
    TEST_ASSERT_EQUAL_PTR(left, result);
    TEST_ASSERT_EQUAL_UINT64(2, left->length);
    TEST_ASSERT_EQUAL_INT64_ARRAY(longs, left->list, 2);
    qDecRef(empty);
    qDecRef(left);

    // Empty left list
    left = qNewLongList(NULL, 0);
    QObj *right = qNewLongList(longs, 2);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(2, left->length);
    TEST_ASSERT_EQUAL_INT64_ARRAY(longs, left->list, 2);
    qDecRef(right);
    qDecRef(left);

    // Both empty
    left = qNewLongList(NULL, 0);
    empty = qNewLongList(NULL, 0);
    qAppendList(&left, empty);
    TEST_ASSERT_TRUE(qIsLongList(left));
    TEST_ASSERT_EQUAL_UINT64(0, left->length);
    qDecRef(empty);
    qDecRef(left);
}

void testAppendListOtherTypes() {
    // Chars
    QObj *left = qNewCharListFromString("hello ");
    QObj *right = qNewCharListFromString("world");
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(11, left->length);
    TEST_ASSERT_EQUAL_MEMORY("hello world", left->list, 11);
    qDecRef(right);
    qDecRef(left);

    // Guids (16-byte elements)
    QGuid guids[] = {{{1}}, {{2}}, {{3}}};
    left = qNewGuidList(guids, 1);
    right = qNewGuidList(guids + 1, 2);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(3, left->length);
    TEST_ASSERT_EQUAL_MEMORY(guids, left->list, sizeof(guids));
    qDecRef(right);
    qDecRef(left);

    // Floats
    QFloat floats[] = {1.5, 2.5, 3.5};
    left = qNewFloatList(floats, 2);
    right = qNewFloatList(floats + 2, 1);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(3, left->length);
    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(floats, left->list, 3);
    qDecRef(right);
    qDecRef(left);

    // Booleans
    QBoolean booleans[] = {1, 0, 1};
    left = qNewBooleanList(booleans, 1);
    right = qNewBooleanList(booleans + 1, 2);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(3, left->length);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(booleans, left->list, 3);
    qDecRef(right);
    qDecRef(left);
}

void testAppendListSymbols() {
    QSymbol leftSymbols[] = {"abc"};
    QSymbol rightSymbols[] = {"def", "ghi"};
    QObj *left = qNewSymbolList(leftSymbols, 1);
    QObj *right = qNewSymbolList(rightSymbols, 2);

    qAppendList(&left, right);

    TEST_ASSERT_EQUAL_UINT64(3, left->length);
    TEST_ASSERT_EQUAL_PTR(qInternString("abc"), qGetSymbolAtIndex(left, 0));
    TEST_ASSERT_EQUAL_PTR(qInternString("def"), qGetSymbolAtIndex(left, 1));
    TEST_ASSERT_EQUAL_PTR(qInternString("ghi"), qGetSymbolAtIndex(left, 2));

    qDecRef(right);
    qDecRef(left);
}

void testAppendListMixed() {
    QObj *left = qNewMixedList(1, qNewLong(1));
    QObj *item0 = qNewSymbol("abc");
    QObj *item1 = qNewCharListFromString("xyz");
    QObj *right = qNewMixedList(2, item0, item1);

    qAppendList(&left, right);

    TEST_ASSERT_TRUE(qIsMixedList(left));
    TEST_ASSERT_EQUAL_UINT64(3, left->length);
    TEST_ASSERT_EQUAL_INT64(1, qGetLong(qGetMixedAtIndex(left, 0)));
    TEST_ASSERT_EQUAL_PTR(item0, qGetMixedAtIndex(left, 1));
    TEST_ASSERT_EQUAL_PTR(item1, qGetMixedAtIndex(left, 2));

    // The items are shared by both lists, so each holds a reference to them
    TEST_ASSERT_EQUAL_INT32(1, item0->refs);
    TEST_ASSERT_EQUAL_INT32(1, item1->refs);

    // Releasing the right list leaves the items in the left list valid
    qDecRef(right);
    TEST_ASSERT_EQUAL_INT32(0, item0->refs);
    TEST_ASSERT_EQUAL_INT32(0, item1->refs);
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbol(qGetMixedAtIndex(left, 1)));
    TEST_ASSERT_EQUAL_MEMORY("xyz", qGetMixedAtIndex(left, 2)->list, 3);

    qDecRef(left);
}

void testAppendListSharedRight() {
    // A right list that is shared can still be appended, as it is only read
    QLong longs[] = {1, 2};
    QObj *right = qNewLongList(longs, 2);
    qIncRef(right);

    QObj *left = qNewLongList(NULL, 0);
    qAppendList(&left, right);
    TEST_ASSERT_EQUAL_UINT64(2, left->length);
    TEST_ASSERT_EQUAL_INT32(1, right->refs);

    qDecRef(right);
    qDecRef(right);
    qDecRef(left);
}

void testAppendListCopyOfItself() {
    // Appending a list to itself is not allowed, but appending a copy of it is
    QLong longs[] = {1, 2, 3};
    QObj *left = qNewLongList(longs, 3);
    QObj *copy = qNewLongList((const QLong *)left->list, left->length);

    qAppendList(&left, copy);
    QLong expected[] = {1, 2, 3, 1, 2, 3};
    TEST_ASSERT_EQUAL_UINT64(6, left->length);
    TEST_ASSERT_EQUAL_INT64_ARRAY(expected, left->list, 6);

    qDecRef(copy);
    qDecRef(left);
}

void testAppendListMany() {
    // Repeatedly append a list to force many reallocations
    QLong chunk[] = {1, 2, 3, 4, 5, 6, 7, 8};
    QObj *right = qNewLongList(chunk, 8);
    QObj *left = qNewLongList(NULL, 0);

    for (int i = 0; i < 10000; i++) {
        QObj *result = qAppendList(&left, right);
        TEST_ASSERT_EQUAL_PTR(left, result);
    }

    TEST_ASSERT_EQUAL_UINT64(80000, left->length);
    for (QSize i = 0; i < left->length; i++)
        TEST_ASSERT_EQUAL_INT64((QLong)(i % 8) + 1, qGetLongAtIndex(left, i));

    qDecRef(right);
    qDecRef(left);
}

///// Mixing the functions /////

void testAppendFunctionsTogether() {
    QObj *list = qNewSymbolList(NULL, 0);
    QSymbol sym = qInternString("a");
    qAppendValue(&list, &sym);
    qAppendSymbol(&list, "b");
    QSymbol more[] = {"c", "d"};
    QObj *right = qNewSymbolList(more, 2);
    qAppendList(&list, right);
    qDecRef(right);

    TEST_ASSERT_EQUAL_UINT64(4, list->length);
    const char *expected[] = {"a", "b", "c", "d"};
    for (QSize i = 0; i < 4; i++)
        TEST_ASSERT_EQUAL_PTR(qInternString(expected[i]), qGetSymbolAtIndex(list, i));

    // A mixed list built from all kinds of items, including one created by the other functions
    QObj *mixed = qNewList(Q_TYPE_MIXED, 0);
    qAppendObject(&mixed, list);
    QObj *tail = qNewMixedList(1, qNewLong(5));
    qAppendList(&mixed, tail);
    qDecRef(tail);

    TEST_ASSERT_EQUAL_UINT64(2, mixed->length);
    TEST_ASSERT_EQUAL_PTR(list, qGetMixedAtIndex(mixed, 0));
    TEST_ASSERT_EQUAL_INT64(5, qGetLong(qGetMixedAtIndex(mixed, 1)));
    qDecRef(mixed);
}

///// Argument checks /////

#if TEST_ASSERTIONS

// Run fn in a child process and check that it aborts (as a failed assert does)
static void checkAborts(void (*fn)(void), const char *name) {
    fflush(stdout);
    pid_t pid = fork();
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, pid, "fork failed");

    if (pid == 0) {
        close(STDERR_FILENO); // hide the assert message
        fn();
        _exit(0);
    }

    int status = 0;
    TEST_ASSERT_EQUAL_MESSAGE(pid, waitpid(pid, &status, 0), "waitpid failed");
    TEST_ASSERT_TRUE_MESSAGE(WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT, name);
}

#define CHECK_ABORTS(fn) checkAborts(fn, #fn " did not abort")

static QLong longValue = 1;

static void appendValueNullListPointer(void) {
    qAppendValue(NULL, &longValue);
}
static void appendValueNullList(void) {
    QObj *list = NULL;
    qAppendValue(&list, &longValue);
}
static void appendValueNullValue(void) {
    QObj *list = qNewLongList(NULL, 0);
    qAppendValue(&list, NULL);
}
static void appendValueAtom(void) {
    QObj *atom = qNewLong(1);
    qAppendValue(&atom, &longValue);
}
static void appendValueMixedList(void) {
    QObj *list = qNewList(Q_TYPE_MIXED, 0);
    qAppendValue(&list, &longValue);
}
static void appendValueDict(void) {
    QObj *dict = qNewDict(qNewSymbolList(NULL, 0), qNewLongList(NULL, 0));
    qAppendValue(&dict, &longValue);
}
static void appendValueError(void) {
    qNewError("type");
    QObj *error = qCheckError(NULL); // an error object (type Q_TYPE_ERROR)
    qAppendValue(&error, &longValue);
}

void testAppendValueChecks() {
    CHECK_ABORTS(appendValueNullListPointer);
    CHECK_ABORTS(appendValueNullList);
    CHECK_ABORTS(appendValueNullValue);
    CHECK_ABORTS(appendValueAtom);
    CHECK_ABORTS(appendValueMixedList);
    CHECK_ABORTS(appendValueDict);
    CHECK_ABORTS(appendValueError);
}

static void appendObjectNullListPointer(void) {
    qAppendObject(NULL, qNewLong(1));
}
static void appendObjectNullList(void) {
    QObj *list = NULL;
    qAppendObject(&list, qNewLong(1));
}
static void appendObjectNullObject(void) {
    QObj *list = qNewList(Q_TYPE_MIXED, 0);
    qAppendObject(&list, NULL);
}
static void appendObjectSimpleList(void) {
    QObj *list = qNewLongList(NULL, 0);
    qAppendObject(&list, qNewLong(1));
}
static void appendObjectAtom(void) {
    QObj *atom = qNewLong(1);
    qAppendObject(&atom, qNewLong(2));
}
static void appendObjectItself(void) {
    QObj *list = qNewList(Q_TYPE_MIXED, 0);
    qAppendObject(&list, list);
}

void testAppendObjectChecks() {
    CHECK_ABORTS(appendObjectNullListPointer);
    CHECK_ABORTS(appendObjectNullList);
    CHECK_ABORTS(appendObjectNullObject);
    CHECK_ABORTS(appendObjectSimpleList);
    CHECK_ABORTS(appendObjectAtom);
    CHECK_ABORTS(appendObjectItself);
}

static void appendSymbolNullListPointer(void) {
    qAppendSymbol(NULL, "abc");
}
static void appendSymbolNullList(void) {
    QObj *list = NULL;
    qAppendSymbol(&list, "abc");
}
static void appendSymbolNullString(void) {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendSymbol(&list, NULL);
}
static void appendSymbolLongList(void) {
    QObj *list = qNewLongList(NULL, 0);
    qAppendSymbol(&list, "abc");
}
static void appendSymbolMixedList(void) {
    QObj *list = qNewList(Q_TYPE_MIXED, 0);
    qAppendSymbol(&list, "abc");
}
static void appendSymbolAtom(void) {
    QObj *atom = qNewSymbol("abc");
    qAppendSymbol(&atom, "def");
}

void testAppendSymbolChecks() {
    CHECK_ABORTS(appendSymbolNullListPointer);
    CHECK_ABORTS(appendSymbolNullList);
    CHECK_ABORTS(appendSymbolNullString);
    CHECK_ABORTS(appendSymbolLongList);
    CHECK_ABORTS(appendSymbolMixedList);
    CHECK_ABORTS(appendSymbolAtom);
}

static void appendFixedSymbolNullListPointer(void) {
    qAppendFixedSymbol(NULL, "abc", 3);
}
static void appendFixedSymbolNullList(void) {
    QObj *list = NULL;
    qAppendFixedSymbol(&list, "abc", 3);
}
static void appendFixedSymbolNullString(void) {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendFixedSymbol(&list, NULL, 1);
}
static void appendFixedSymbolTooLong(void) {
    QObj *list = qNewSymbolList(NULL, 0);
    qAppendFixedSymbol(&list, "abc", (QSize)INT32_MAX + 1);
}
static void appendFixedSymbolLongList(void) {
    QObj *list = qNewLongList(NULL, 0);
    qAppendFixedSymbol(&list, "abc", 3);
}
static void appendFixedSymbolMixedList(void) {
    QObj *list = qNewList(Q_TYPE_MIXED, 0);
    qAppendFixedSymbol(&list, "abc", 3);
}
static void appendFixedSymbolAtom(void) {
    QObj *atom = qNewSymbol("abc");
    qAppendFixedSymbol(&atom, "def", 3);
}

void testAppendFixedSymbolChecks() {
    CHECK_ABORTS(appendFixedSymbolNullListPointer);
    CHECK_ABORTS(appendFixedSymbolNullList);
    CHECK_ABORTS(appendFixedSymbolNullString);
    CHECK_ABORTS(appendFixedSymbolTooLong);
    CHECK_ABORTS(appendFixedSymbolLongList);
    CHECK_ABORTS(appendFixedSymbolMixedList);
    CHECK_ABORTS(appendFixedSymbolAtom);
}

static void appendListNullLeftPointer(void) {
    qAppendList(NULL, qNewLongList(NULL, 0));
}
static void appendListNullLeft(void) {
    QObj *left = NULL;
    qAppendList(&left, qNewLongList(NULL, 0));
}
static void appendListNullRight(void) {
    QObj *left = qNewLongList(NULL, 0);
    qAppendList(&left, NULL);
}
static void appendListDifferentTypes(void) {
    QObj *left = qNewLongList(NULL, 0);
    qAppendList(&left, qNewIntList(NULL, 0));
}
static void appendListSimpleToMixed(void) {
    QObj *left = qNewList(Q_TYPE_MIXED, 0);
    qAppendList(&left, qNewLongList(NULL, 0));
}
static void appendListMixedToSimple(void) {
    QObj *left = qNewLongList(NULL, 0);
    qAppendList(&left, qNewList(Q_TYPE_MIXED, 0));
}
static void appendListAtoms(void) {
    QObj *left = qNewLong(1);
    qAppendList(&left, qNewLong(2));
}
static void appendListItself(void) {
    QLong longs[] = {1, 2};
    QObj *left = qNewLongList(longs, 2);
    qAppendList(&left, left);
}

void testAppendListChecks() {
    CHECK_ABORTS(appendListNullLeftPointer);
    CHECK_ABORTS(appendListNullLeft);
    CHECK_ABORTS(appendListNullRight);
    CHECK_ABORTS(appendListDifferentTypes);
    CHECK_ABORTS(appendListSimpleToMixed);
    CHECK_ABORTS(appendListMixedToSimple);
    CHECK_ABORTS(appendListAtoms);
    CHECK_ABORTS(appendListItself);
}

#endif // TEST_ASSERTIONS

int main() {
    UNITY_BEGIN();

    RUN_TEST(testAppendValueLong);
    RUN_TEST(testAppendValueEveryType);
    RUN_TEST(testAppendValueToEmptyList);
    RUN_TEST(testAppendValueSpecialValues);
    RUN_TEST(testAppendValueInternedSymbol);
    RUN_TEST(testAppendValueCopiesValue);
    RUN_TEST(testAppendValueMany);
    RUN_TEST(testAppendValueFromSameListWithReallocation);
    RUN_TEST(testAppendValueManyChars);

    RUN_TEST(testAppendObject);
    RUN_TEST(testAppendObjectToEmptyList);
    RUN_TEST(testAppendObjectKinds);
    RUN_TEST(testAppendObjectDoesNotCollapse);
    RUN_TEST(testAppendObjectTakesOwnership);
    RUN_TEST(testAppendObjectKeepReference);
    RUN_TEST(testAppendObjectSameObjectTwice);
    RUN_TEST(testAppendObjectMany);

    RUN_TEST(testAppendSymbol);
    RUN_TEST(testAppendSymbolInterns);
    RUN_TEST(testAppendSymbolAlreadyInterned);
    RUN_TEST(testAppendSymbolNull);
    RUN_TEST(testAppendSymbolDuplicates);
    RUN_TEST(testAppendSymbolUnusualCharacters);
    RUN_TEST(testAppendSymbolMany);

    RUN_TEST(testAppendFixedSymbol);
    RUN_TEST(testAppendFixedSymbolNotNullTerminated);
    RUN_TEST(testAppendFixedSymbolWholeString);
    RUN_TEST(testAppendFixedSymbolStopsAtNull);
    RUN_TEST(testAppendFixedSymbolEmpty);
    RUN_TEST(testAppendFixedSymbolTokens);
    RUN_TEST(testAppendFixedSymbolMany);

    RUN_TEST(testAppendList);
    RUN_TEST(testAppendListBorrowsRight);
    RUN_TEST(testAppendListEmpty);
    RUN_TEST(testAppendListOtherTypes);
    RUN_TEST(testAppendListSymbols);
    RUN_TEST(testAppendListMixed);
    RUN_TEST(testAppendListSharedRight);
    RUN_TEST(testAppendListCopyOfItself);
    RUN_TEST(testAppendListMany);

    RUN_TEST(testAppendFunctionsTogether);

#if TEST_ASSERTIONS
    RUN_TEST(testAppendValueChecks);
    RUN_TEST(testAppendObjectChecks);
    RUN_TEST(testAppendSymbolChecks);
    RUN_TEST(testAppendFixedSymbolChecks);
    RUN_TEST(testAppendListChecks);
#endif

    return UNITY_END();
}
