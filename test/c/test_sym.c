
/**
 * @file test_sym.c
 * @brief Unit tests for sym.h.
 */

#include "q.h"
#include "unity.h"

void setUp() {
}
void tearDown() {
}

void testInternString() {
    QSymbol sym = qInternString("hello");
    TEST_ASSERT_EQUAL_STRING("hello", sym);

    // Same characters intern to the same pointer
    QChar buffer[] = "hello";
    TEST_ASSERT_EQUAL_PTR(sym, qInternString(buffer));
    TEST_ASSERT_NOT_EQUAL(buffer, sym);

    // Changing the source string afterwards does not change the symbol
    buffer[0] = 'j';
    TEST_ASSERT_EQUAL_STRING("hello", sym);

    // Different characters intern to a different pointer
    TEST_ASSERT_NOT_EQUAL(sym, qInternString("world"));
}

void testInternStringEmpty() {
    QSymbol sym = qInternString("");
    TEST_ASSERT_EQUAL_STRING("", sym);
    TEST_ASSERT_EQUAL_PTR(sym, qInternString(""));
}

void testInternStringMatchesSymbolAtom() {
    QObj *obj = qNewSymbol("hello");
    TEST_ASSERT_EQUAL_PTR(qGetSymbol(obj), qInternString("hello"));
    decRef(obj);
}

void testInternFixedString() {
    // Prefix of a longer string
    QSymbol sym = qInternFixedString("hello world", 5);
    TEST_ASSERT_EQUAL_STRING("hello", sym);
    TEST_ASSERT_EQUAL_PTR(qInternString("hello"), sym);

    // String that is not null-terminated
    QChar buffer[] = {'a', 'b', 'c'};
    TEST_ASSERT_EQUAL_STRING("abc", qInternFixedString(buffer, 3));

    // Whole string
    TEST_ASSERT_EQUAL_PTR(qInternString("xyz"), qInternFixedString("xyz", 3));
}

void testInternFixedStringStopsAtNull() {
    QSymbol sym = qInternFixedString("ab\0cd", 5);
    TEST_ASSERT_EQUAL_STRING("ab", sym);
    TEST_ASSERT_EQUAL_PTR(qInternString("ab"), sym);
}

void testInternFixedStringEmpty() {
    QSymbol empty = qInternString("");
    TEST_ASSERT_EQUAL_PTR(empty, qInternFixedString("xyz", 0));
    TEST_ASSERT_EQUAL_PTR(empty, qInternFixedString(NULL, 0));
}

void testInternFillsSymbolList() {
    QObj *list = qNewList(Q_TYPE_SYMBOL, 2);
    QSymbol *syms = (QSymbol *)list->list;
    syms[0] = qInternString("abc");
    syms[1] = qInternFixedString("defgh", 3);

    TEST_ASSERT_EQUAL_STRING("abc", qGetSymbolAtIndex(list, 0));
    TEST_ASSERT_EQUAL_STRING("def", qGetSymbolAtIndex(list, 1));
    decRef(list);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testInternString);
    RUN_TEST(testInternStringEmpty);
    RUN_TEST(testInternStringMatchesSymbolAtom);
    RUN_TEST(testInternFixedString);
    RUN_TEST(testInternFixedStringStopsAtNull);
    RUN_TEST(testInternFixedStringEmpty);
    RUN_TEST(testInternFillsSymbolList);

    return UNITY_END();
}
