
/**
 * @file test_get.c
 * @brief Unit tests for get.h.
 */

#include <stdlib.h>

#include "q.h"
#include "unity.h"

void setUp() {
}
void tearDown() {
}

static QObj *newTable(QSize rows) {
    QSymbol names[] = {"a", "b"};
    QLong longs[] = {1, 2, 3};
    QChar chars[] = {'x', 'y', 'z'};
    return qNewTable(qNewSymbolList(names, 2), qNewLongList(longs, rows), qNewCharList(chars, rows));
}

void testGetCountNull() {
    TEST_ASSERT_EQUAL_UINT64(0, qGetCount(nullptr));
}

void testGetCountAtom() {
    QObj *atom = qNewLong(42);
    TEST_ASSERT_EQUAL_UINT64(1, qGetCount(atom));
    qDecRef(atom);

    QObj *symbol = qNewSymbol("abc");
    TEST_ASSERT_EQUAL_UINT64(1, qGetCount(symbol));
    qDecRef(symbol);
}

void testGetCountFunction() {
    // Only the type is read for a function
    QObj lambda = {.type = Q_TYPE_LAMBDA};
    TEST_ASSERT_EQUAL_UINT64(1, qGetCount(&lambda));
}

void testGetCountLists() {
    QLong longs[] = {1, 2, 3, 4};
    QObj *empty = qNewLongList(longs, 0);
    TEST_ASSERT_EQUAL_UINT64(0, qGetCount(empty));
    qDecRef(empty);

    QObj *list = qNewLongList(longs, 4);
    TEST_ASSERT_EQUAL_UINT64(4, qGetCount(list));
    qDecRef(list);

    QObj *mixed = qNewMixedList(2, qNewLong(1), qNewChar('a'));
    TEST_ASSERT_EQUAL_UINT64(2, qGetCount(mixed));
    qDecRef(mixed);

    // Enumerated list (only the type and length are read)
    QObj enumerated = {.type = Q_TYPE_ENUM_FIRST, .length = 5};
    TEST_ASSERT_EQUAL_UINT64(5, qGetCount(&enumerated));
}

void testGetCountDict() {
    QSymbol keys[] = {"a", "b", "c"};
    QLong values[] = {1, 2, 3};
    QObj *dict = qNewDict(qNewSymbolList(keys, 3), qNewLongList(values, 3));
    TEST_ASSERT_EQUAL_UINT64(3, qGetCount(dict));
    TEST_ASSERT_EQUAL_UINT64(3, qGetDictCount(dict));
    qDecRef(dict);
}

void testGetCountTable() {
    QObj *table = newTable(3);
    TEST_ASSERT_EQUAL_UINT64(3, qGetCount(table));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table));
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table));
    qDecRef(table);

    QObj *emptyRows = newTable(0);
    TEST_ASSERT_EQUAL_UINT64(0, qGetCount(emptyRows));
    TEST_ASSERT_EQUAL_UINT64(0, qGetTableRowCount(emptyRows));
    qDecRef(emptyRows);
}

void testGetCountTableNoColumns() {
    // kdb+ does not create a table with no columns, so build one by hand: a table whose column
    // dictionary has an empty header and an empty mixed list of columns
    QObj header = {.type = Q_TYPE_SYMBOL, .length = 0};
    QObj columns = {.type = Q_TYPE_MIXED, .length = 0};

    // The dictionary's two items are stored in its flexible list member
    QObj *dict = malloc(sizeof(QObj) + 2 * sizeof(QObj *));
    TEST_ASSERT_NOT_NULL(dict);
    *dict = (QObj){.type = Q_TYPE_DICTIONARY, .length = 2};
    ((QObj **)dict->list)[0] = &header;
    ((QObj **)dict->list)[1] = &columns;
    QObj table = {.type = Q_TYPE_TABLE, .nested = dict};

    TEST_ASSERT_EQUAL_UINT64(0, qGetTableColumnCount(&table));
    TEST_ASSERT_EQUAL_UINT64(0, qGetTableRowCount(&table));
    TEST_ASSERT_EQUAL_UINT64(0, qGetCount(&table));

    free(dict);
}

void testGetCountKeyedTable() {
    QSymbol keySymbols[] = {"key"};
    QSymbol valueSymbols[] = {"value"};
    QLong longs[] = {1, 2, 3};

    QObj *keys = qNewTable(qNewSymbolList(keySymbols, 1), qNewLongList(longs, 3));
    QObj *values = qNewTable(qNewSymbolList(valueSymbols, 1), qNewLongList(longs, 3));
    QObj *keyedTable = qNewKeyedTable(keys, values);

    // The keys of a keyed table are a table, so the count is its row count
    TEST_ASSERT_EQUAL_UINT64(3, qGetCount(keyedTable));
    TEST_ASSERT_EQUAL_UINT64(3, qGetDictCount(keyedTable));
    qDecRef(keyedTable);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testGetCountNull);
    RUN_TEST(testGetCountAtom);
    RUN_TEST(testGetCountFunction);
    RUN_TEST(testGetCountLists);
    RUN_TEST(testGetCountDict);
    RUN_TEST(testGetCountTable);
    RUN_TEST(testGetCountTableNoColumns);
    RUN_TEST(testGetCountKeyedTable);

    return UNITY_END();
}
