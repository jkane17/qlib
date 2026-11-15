/**
 * @file test_util.c
 * @brief Unit tests for util.h.
 */

#include "util.h"
#include "unity.h"

void setUp() {
}
void tearDown() {
}

void testTypeSizePrimitiveTypes() {
    TEST_ASSERT_EQUAL_UINT64(sizeof(QBoolean), qTypeSize(Q_TYPE_BOOLEAN));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QGuid), qTypeSize(Q_TYPE_GUID));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QByte), qTypeSize(Q_TYPE_BYTE));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QShort), qTypeSize(Q_TYPE_SHORT));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QInt), qTypeSize(Q_TYPE_INT));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QLong), qTypeSize(Q_TYPE_LONG));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QReal), qTypeSize(Q_TYPE_REAL));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QFloat), qTypeSize(Q_TYPE_FLOAT));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QChar), qTypeSize(Q_TYPE_CHAR));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QSymbol), qTypeSize(Q_TYPE_SYMBOL));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QTimestamp), qTypeSize(Q_TYPE_TIMESTAMP));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QMonth), qTypeSize(Q_TYPE_MONTH));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QDate), qTypeSize(Q_TYPE_DATE));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QDatetime), qTypeSize(Q_TYPE_DATETIME));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QTimespan), qTypeSize(Q_TYPE_TIMESPAN));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QMinute), qTypeSize(Q_TYPE_MINUTE));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QSecond), qTypeSize(Q_TYPE_SECOND));
    TEST_ASSERT_EQUAL_UINT64(sizeof(QTime), qTypeSize(Q_TYPE_TIME));
}

void testTypeSizeUnsupportedCodes() {
    // Check every code in Q's signed-byte object type field, including negative atom
    // codes, the unused primitive slot 3, mixed lists, enums, tables and dictionaries.
    for (int type = -128; type <= 127; type++) {
        if (type >= Q_TYPE_BOOLEAN && type <= Q_TYPE_TIME && type != 3)
            continue;
        TEST_ASSERT_EQUAL_UINT64(0, qTypeSize((QTypeCode)type));
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testTypeSizePrimitiveTypes);
    RUN_TEST(testTypeSizeUnsupportedCodes);
    return UNITY_END();
}
