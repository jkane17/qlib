
/**
 * @file test_cpp.cpp
 * @brief Checks that the C interface can be used from C++.
 *
 * Compiled as C++ and linked against the C-built libcdk.so, so this also verifies that the
 * functions defined in C (e.g. qNewLongList in new.c) are declared with C linkage.
 */

#include <cmath>
#include <cstring>
#include <string>

#include "q.h"
#include "unity.h"

extern "C" {
void setUp() {
}
void tearDown() {
}
}

// Type definitions and layout are usable in constant expressions
static_assert(sizeof(QObj) == 16);
static_assert(Q_TYPE_LONG == 7 && Q_TYPE_ERROR == -128);
static_assert(Q_ATTR_GROUPED == 5);
static_assert(Q_GUID_NULL.bytes[0] == 0);

// QSymbol is a (non-const) char *, so C++ string literals must be cast when building symbol arrays.
// kdb+ only reads the strings (it interns a copy).
static QSymbol sym(const char *str) {
    return const_cast<QSymbol>(str);
}

void testAtoms() {
    QObj *boolean = qNewBoolean(7);
    TEST_ASSERT_TRUE(qIsBoolean(boolean));
    TEST_ASSERT_EQUAL_UINT8(1, qGetBoolean(boolean));
    decRef(boolean);

    QObj *longAtom = qNewLong(42);
    TEST_ASSERT_TRUE(qIsLong(longAtom));
    TEST_ASSERT_EQUAL_INT64(42, qGetLong(longAtom));
    decRef(longAtom);

    QObj *floatAtom = qNewFloat(Q_FLOAT_NULL);
    TEST_ASSERT_TRUE(qIsFloat(floatAtom));
    TEST_ASSERT_TRUE(std::isnan(qGetFloat(floatAtom)));
    decRef(floatAtom);

    // A string literal can be passed directly to qNewSymbol
    QObj *symbol = qNewSymbol("abc");
    TEST_ASSERT_TRUE(qIsSymbol(symbol));
    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbol(symbol));
    decRef(symbol);

    QObj *guid = qNewGuid(Q_GUID_NULL);
    TEST_ASSERT_TRUE(qIsGuid(guid));
    TEST_ASSERT_EQUAL_MEMORY(Q_GUID_NULL.bytes, qGetGuid(guid)->bytes, 16);
    decRef(guid);
}

void testLists() {
    const QLong longs[] = {1, 2, 3};
    QObj *longList = qNewLongList(longs, 3);
    TEST_ASSERT_TRUE(qIsLongList(longList));
    TEST_ASSERT_TRUE(qIsSimpleList(longList));
    TEST_ASSERT_EQUAL_UINT64(3, qGetCount(longList));
    TEST_ASSERT_EQUAL_INT64(3, qGetLongAtIndex(longList, 2));
    decRef(longList);

    const QSymbol symbols[] = {sym("x"), sym("y")};
    QObj *symbolList = qNewSymbolList(symbols, 2);
    TEST_ASSERT_TRUE(qIsSymbolList(symbolList));
    TEST_ASSERT_EQUAL_STRING("y", qGetSymbolAtIndex(symbolList, 1));
    decRef(symbolList);

    QObj *chars = qNewCharListFromString("hello");
    TEST_ASSERT_TRUE(qIsCharList(chars));
    TEST_ASSERT_EQUAL_UINT64(5, chars->length);
    decRef(chars);

    QObj *empty = qNewIntList(nullptr, 0);
    TEST_ASSERT_TRUE(qIsIntList(empty));
    TEST_ASSERT_EQUAL_UINT64(0, empty->length);
    decRef(empty);
}

void testMixedListAndDict() {
    QObj *mixed = qNewMixedList(2, qNewLong(1), qNewSymbol("a"));
    TEST_ASSERT_TRUE(qIsMixedList(mixed));
    TEST_ASSERT_TRUE(qIsList(mixed));
    TEST_ASSERT_TRUE(qIsListType(mixed, Q_TYPE_MIXED));
    TEST_ASSERT_TRUE(qIsLong(qGetMixedAtIndex(mixed, 0)));
    decRef(mixed);

    const QSymbol keys[] = {sym("a"), sym("b")};
    const QLong values[] = {10, 20};
    QObj *dict = qNewDict(qNewSymbolList(keys, 2), qNewLongList(values, 2));
    TEST_ASSERT_TRUE(qIsDict(dict));
    TEST_ASSERT_EQUAL_UINT64(2, qGetDictCount(dict));
    TEST_ASSERT_EQUAL_INT64(20, qGetLongAtIndex(qGetDictValues(dict), 1));
    decRef(dict);
}

void testTables() {
    const QSymbol names[] = {sym("id"), sym("code")};
    const QLong ids[] = {1, 2, 3};
    const QChar codes[] = {'a', 'b', 'c'};
    QObj *table =
        qNewTable(qNewSymbolList(names, 2), qNewLongList(ids, 3), qNewCharList(codes, 3));
    TEST_ASSERT_TRUE(qIsTable(table));
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table));

    // qKeyTable does not take ownership of table
    QObj *keyedTable = qKeyTable(1, table);
    TEST_ASSERT_TRUE(qIsKeyedTable(keyedTable));
    TEST_ASSERT_EQUAL_UINT64(3, qGetCount(keyedTable));

    // qUnkeyTable takes ownership of keyedTable
    QObj *unkeyed = qUnkeyTable(keyedTable);
    TEST_ASSERT_TRUE(qIsTable(unkeyed));
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(unkeyed));

    decRef(unkeyed);
    decRef(table);
}

void testTableOverload() {
    const QSymbol names[] = {sym("a"), sym("b")};
    const QLong longs[] = {1, 2, 3};

    // Too few columns - Error, header and column released
    QObj *header = qNewSymbolList(names, 2);
    QObj *column = qNewLongList(longs, 3);
    incRef(header);
    incRef(column);
    TEST_ASSERT_NULL(qNewTable(header, column));
    TEST_ASSERT_TRUE(std::string(qGetError(qCheckError(nullptr))) == "length");
    TEST_ASSERT_EQUAL_INT(0, header->refs);
    TEST_ASSERT_EQUAL_INT(0, column->refs);
    decRef(header);
    decRef(column);

    // Null header with no columns - Error
    TEST_ASSERT_NULL(qNewTable(nullptr));
    TEST_ASSERT_TRUE(std::string(qGetError(qCheckError(nullptr))) == "domain");
}

void testErrors() {
    QObj *list = qNewLongList(nullptr, 1);
    TEST_ASSERT_NULL(list);

    QObj *error = qCheckError(list);
    TEST_ASSERT_TRUE(qIsError(error));
    TEST_ASSERT_TRUE(std::string(qGetError(error)) == "domain");

    // qGetError returns an empty string for anything that is not an error
    QObj *atom = qNewLong(1);
    TEST_ASSERT_EQUAL_STRING("", qGetError(atom));
    decRef(atom);
}

void testReferenceCounting() {
    QObj *atom = qNewLong(1);
    TEST_ASSERT_EQUAL_INT(0, atom->refs);
    incRef(atom);
    TEST_ASSERT_EQUAL_INT(1, atom->refs);
    decRef(atom);
    TEST_ASSERT_EQUAL_INT(0, atom->refs);
    decRef(atom);
}

void testTypeCodeSwitch() {
    // QTypeCode works as a C++ enum in switch statements
    QObj *atom = qNewLong(1);
    const char *name = "other";
    switch (static_cast<QTypeCode>(-atom->type)) {
    case Q_TYPE_LONG:
        name = "long";
        break;
    default:
        break;
    }
    TEST_ASSERT_EQUAL_STRING("long", name);
    decRef(atom);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testAtoms);
    RUN_TEST(testLists);
    RUN_TEST(testMixedListAndDict);
    RUN_TEST(testTables);
    RUN_TEST(testTableOverload);
    RUN_TEST(testErrors);
    RUN_TEST(testReferenceCounting);
    RUN_TEST(testTypeCodeSwitch);

    return UNITY_END();
}
