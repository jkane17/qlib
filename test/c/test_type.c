
/**
 * @file test_type.c
 * @brief Unit tests for type.h.
 */

#include <math.h>
#include <stdint.h>

#include "q.h"
#include "unity.h"

void setUp() {
}
void tearDown() {
}

void testTypeSizes() {
    TEST_ASSERT_EQUAL_size_t(1, sizeof(QBoolean));
    TEST_ASSERT_EQUAL_size_t(16, sizeof(QGuid));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(QByte));
    TEST_ASSERT_EQUAL_size_t(2, sizeof(QShort));
    TEST_ASSERT_EQUAL_size_t(4, sizeof(QInt));
    TEST_ASSERT_EQUAL_size_t(8, sizeof(QLong));
    TEST_ASSERT_EQUAL_size_t(4, sizeof(QReal));
    TEST_ASSERT_EQUAL_size_t(8, sizeof(QFloat));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(QChar));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(QTypeCode));
    TEST_ASSERT_EQUAL_size_t(1, sizeof(QAttrCode));
    TEST_ASSERT_EQUAL_size_t(8, sizeof(QSize));
}

void testSizeMax() {
    TEST_ASSERT_EQUAL_UINT64(INT64_MAX, Q_SIZE_MAX);
}

void testIntegerSpecialValues() {
    TEST_ASSERT_EQUAL_INT16(-32768, Q_SHORT_NULL);
    TEST_ASSERT_EQUAL_INT16(32767, Q_SHORT_INF);

    TEST_ASSERT_EQUAL_INT32(INT32_MIN, Q_INT_NULL);
    TEST_ASSERT_EQUAL_INT32(2147483647, Q_INT_INF);

    TEST_ASSERT_EQUAL_INT64(INT64_MIN, Q_LONG_NULL);
    TEST_ASSERT_EQUAL_INT64(INT64_MAX, Q_LONG_INF);

    // Negative infinity is written as the negation of infinity, which is one greater than null
    TEST_ASSERT_EQUAL_INT16(-32767, -Q_SHORT_INF);
    TEST_ASSERT_EQUAL_INT16(Q_SHORT_NULL + 1, -Q_SHORT_INF);
    TEST_ASSERT_EQUAL_INT32(Q_INT_NULL + 1, -Q_INT_INF);
    TEST_ASSERT_EQUAL_INT64(Q_LONG_NULL + 1, -Q_LONG_INF);
}

void testFloatingPointSpecialValues() {
    TEST_ASSERT_TRUE(isnan(Q_REAL_NULL));
    TEST_ASSERT_TRUE(isinf(Q_REAL_INF) && Q_REAL_INF > 0);
    TEST_ASSERT_TRUE(isinf(-Q_REAL_INF) && -Q_REAL_INF < 0);

    TEST_ASSERT_TRUE(isnan(Q_FLOAT_NULL));
    TEST_ASSERT_TRUE(isinf(Q_FLOAT_INF) && Q_FLOAT_INF > 0);
    TEST_ASSERT_TRUE(isinf(-Q_FLOAT_INF) && -Q_FLOAT_INF < 0);
}

void testOtherSpecialValues() {
    TEST_ASSERT_EQUAL_CHAR(' ', Q_CHAR_NULL);
    TEST_ASSERT_EQUAL_STRING("", Q_SYMBOL_NULL);

    for (int i = 0; i < 16; i++)
        TEST_ASSERT_EQUAL_UINT8(0, Q_GUID_NULL.bytes[i]);

    // The null guid round-trips through a guid atom
    QObj *guid = qNewGuid(Q_GUID_NULL);
    TEST_ASSERT_EQUAL_MEMORY(Q_GUID_NULL.bytes, qGetGuid(guid)->bytes, 16);
    decRef(guid);
}

void testSpecialValuesRoundTrip() {
    // Special values survive being stored in, and read back from, Q atoms
    QObj *shortNull = qNewShort(Q_SHORT_NULL);
    TEST_ASSERT_EQUAL_INT16(Q_SHORT_NULL, qGetShort(shortNull));
    decRef(shortNull);

    QObj *intNinf = qNewInt(-Q_INT_INF);
    TEST_ASSERT_EQUAL_INT32(-Q_INT_INF, qGetInt(intNinf));
    decRef(intNinf);

    QObj *timestampNull = qNewTimestamp(Q_LONG_NULL);
    TEST_ASSERT_EQUAL_INT64(Q_LONG_NULL, qGetTimestamp(timestampNull));
    decRef(timestampNull);

    QObj *realNull = qNewReal(Q_REAL_NULL);
    TEST_ASSERT_TRUE(isnan(qGetReal(realNull)));
    decRef(realNull);

    QObj *floatNinf = qNewFloat(-Q_FLOAT_INF);
    TEST_ASSERT_TRUE(isinf(qGetFloat(floatNinf)) && qGetFloat(floatNinf) < 0);
    decRef(floatNinf);
}

void testTypeCodes() {
    TEST_ASSERT_EQUAL_INT(-128, Q_TYPE_ERROR);
    TEST_ASSERT_EQUAL_INT(20, Q_TYPE_ENUM_FIRST);
    TEST_ASSERT_EQUAL_INT(76, Q_TYPE_ENUM_LAST);
    TEST_ASSERT_EQUAL_INT(77, Q_TYPE_ANYMAP);
    TEST_ASSERT_EQUAL_INT(Q_TYPE_ANYMAP + Q_TYPE_BOOLEAN, Q_TYPE_NESTED_FIRST);
    TEST_ASSERT_EQUAL_INT(Q_TYPE_ANYMAP + Q_TYPE_TIME, Q_TYPE_NESTED_LAST);
    TEST_ASSERT_EQUAL_INT(97, Q_TYPE_NESTED_SYM_ENUM);
    TEST_ASSERT_EQUAL_INT(100, Q_TYPE_LAMBDA);
    TEST_ASSERT_EQUAL_INT(112, Q_TYPE_DYNAMIC_LOAD);
}

void testAttributeCodes() {
    TEST_ASSERT_EQUAL_INT(0, Q_ATTR_NONE);
    TEST_ASSERT_EQUAL_INT(1, Q_ATTR_SORTED);
    TEST_ASSERT_EQUAL_INT(2, Q_ATTR_UNIQUE);
    TEST_ASSERT_EQUAL_INT(3, Q_ATTR_PARTED);
    TEST_ASSERT_EQUAL_INT(5, Q_ATTR_GROUPED); // Differs from the IPC serialization (4)
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testTypeSizes);
    RUN_TEST(testSizeMax);
    RUN_TEST(testIntegerSpecialValues);
    RUN_TEST(testFloatingPointSpecialValues);
    RUN_TEST(testOtherSpecialValues);
    RUN_TEST(testSpecialValuesRoundTrip);
    RUN_TEST(testTypeCodes);
    RUN_TEST(testAttributeCodes);

    return UNITY_END();
}
