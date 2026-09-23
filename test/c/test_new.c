
/**
 * @file test_new.c
 * @brief Unit tests for new.h & new.c.
 */

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "q.h"
#include "unity.h"

void setUp() {
}
void tearDown() {
}

static QObj *newMixedListVar(QSize length, ...) {
    va_list args;
    va_start(args);
    QObj *obj = qNewMixedListVar(length, args);
    va_end(args);
    return obj;
}

static QObj *newTableVar(QObj *header, ...) {
    va_list args;
    va_start(args);
    QObj *obj = qNewTableVar(header, args);
    va_end(args);
    return obj;
}

/**
 * @brief Check that a Q object is an atom of the given type and verify basic properties.
 *
 * @param obj Q object to check.
 * @param type Type code to check for.
 */
static void checkAtomType(QObj *obj, QTypeCode type) {
    if (obj->type >= 0)
        TEST_FAIL_MESSAGE("Not atom");

    TEST_ASSERT_EQUAL_INT(-type, obj->type);
    TEST_ASSERT_TRUE(qIsAtom(obj));
    TEST_ASSERT_TRUE(qIsAtomType(obj, type));

    TEST_ASSERT_FALSE(qIsList(obj));
    TEST_ASSERT_FALSE(qIsSimpleList(obj));
    TEST_ASSERT_FALSE(qIsListType(obj, type));

    bool isType[] = {
        false,
        qIsBoolean(obj),
        qIsGuid(obj),
        false,
        qIsByte(obj),
        qIsShort(obj),
        qIsInt(obj),
        qIsLong(obj),
        qIsReal(obj),
        qIsFloat(obj),
        qIsChar(obj),
        qIsSymbol(obj),
        qIsTimestamp(obj),
        qIsMonth(obj),
        qIsDate(obj),
        qIsDatetime(obj),
        qIsTimespan(obj),
        qIsMinute(obj),
        qIsSecond(obj),
        qIsTime(obj),
    };

    bool isListType[] = {
        qIsMixedList(obj),     qIsBooleanList(obj), qIsGuidList(obj),   false,
        qIsByteList(obj),      qIsShortList(obj),   qIsIntList(obj),    qIsLongList(obj),
        qIsRealList(obj),      qIsFloatList(obj),   qIsCharList(obj),   qIsSymbolList(obj),
        qIsTimestampList(obj), qIsMonthList(obj),   qIsDateList(obj),   qIsDatetimeList(obj),
        qIsTimespanList(obj),  qIsMinuteList(obj),  qIsSecondList(obj), qIsTimeList(obj),
    };

    for (QType i = 0; i <= Q_TYPE_TIME; i++) {
        if (i == type) {
            TEST_ASSERT_TRUE(isType[i]);
        } else {
            TEST_ASSERT_FALSE(isType[i]);
        }
        TEST_ASSERT_FALSE(isListType[i]);
    }

    TEST_ASSERT_FALSE(qIsDict(obj));
    TEST_ASSERT_FALSE(qIsTable(obj));
    TEST_ASSERT_FALSE(qIsKeyedTable(obj));
}

/**
 * @brief Check that an atom is the correct value.
 *
 * @param obj Q object containing an atom.
 * @param type Type code of the atom.
 * @param value Expected value.
 */
static void checkAtomValue(QObj *obj, QTypeCode type, void *value) {
    if (obj->type >= 0)
        TEST_FAIL_MESSAGE("Not atom");

    TEST_ASSERT_EQUAL_INT(0, obj->refs);

    switch (type) {
        case Q_TYPE_BOOLEAN:
            TEST_ASSERT_EQUAL_INT(*(QByte *)value, obj->qbyte);
            TEST_ASSERT_EQUAL_INT(*(QByte *)value, qGetBoolean(obj));
            break;
        case Q_TYPE_GUID:
            TEST_ASSERT_EQUAL_CHAR_ARRAY((unsigned char *)value, ((QGuid *)obj->list)->bytes, 16);
            TEST_ASSERT_EQUAL_CHAR_ARRAY((unsigned char *)value, qGetGuid(obj)->bytes, 16);
            break;
        case Q_TYPE_BYTE:
            TEST_ASSERT_EQUAL_INT(*(QByte *)value, obj->qbyte);
            TEST_ASSERT_EQUAL_INT(*(QByte *)value, qGetByte(obj));
            break;
        case Q_TYPE_SHORT:
            TEST_ASSERT_EQUAL_INT(*(QShort *)value, obj->qshort);
            TEST_ASSERT_EQUAL_INT(*(QShort *)value, qGetShort(obj));
            break;
        case Q_TYPE_INT:
            TEST_ASSERT_EQUAL_INT(*(QInt *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QInt *)value, qGetInt(obj));
            break;
        case Q_TYPE_LONG:
            TEST_ASSERT_EQUAL_INT64(*(QLong *)value, obj->qlong);
            TEST_ASSERT_EQUAL_INT64(*(QLong *)value, qGetLong(obj));
            break;
        case Q_TYPE_REAL:
            TEST_ASSERT_EQUAL_FLOAT(*(QReal *)value, obj->qreal);
            TEST_ASSERT_EQUAL_FLOAT(*(QReal *)value, qGetReal(obj));
            break;
        case Q_TYPE_FLOAT:
            TEST_ASSERT_EQUAL_DOUBLE(*(QFloat *)value, obj->qfloat);
            TEST_ASSERT_EQUAL_DOUBLE(*(QFloat *)value, qGetFloat(obj));
            break;
        case Q_TYPE_CHAR:
            TEST_ASSERT_EQUAL_CHAR(*(QChar *)value, obj->qbyte);
            TEST_ASSERT_EQUAL_CHAR(*(QChar *)value, qGetChar(obj));
            break;
        case Q_TYPE_SYMBOL:
            TEST_ASSERT_EQUAL_STRING((QSymbol)value, obj->qsymbol);
            TEST_ASSERT_EQUAL_STRING((QSymbol)value, qGetSymbol(obj));
            break;
        case Q_TYPE_TIMESTAMP:
            TEST_ASSERT_EQUAL_INT64(*(QTimestamp *)value, obj->qlong);
            TEST_ASSERT_EQUAL_INT64(*(QTimestamp *)value, qGetTimestamp(obj));
            break;
        case Q_TYPE_MONTH:
            TEST_ASSERT_EQUAL_INT(*(QMonth *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QMonth *)value, qGetMonth(obj));
            break;
        case Q_TYPE_DATE:
            TEST_ASSERT_EQUAL_INT(*(QDate *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QDate *)value, qGetDate(obj));
            break;
        case Q_TYPE_DATETIME:
            TEST_ASSERT_EQUAL_DOUBLE(*(QDatetime *)value, obj->qfloat);
            TEST_ASSERT_EQUAL_DOUBLE(*(QDatetime *)value, qGetDatetime(obj));
            break;
        case Q_TYPE_TIMESPAN:
            TEST_ASSERT_EQUAL_INT64(*(QTimespan *)value, obj->qlong);
            TEST_ASSERT_EQUAL_INT64(*(QTimespan *)value, qGetTimespan(obj));
            break;
        case Q_TYPE_MINUTE:
            TEST_ASSERT_EQUAL_INT(*(QMinute *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QMinute *)value, qGetMinute(obj));
            break;
        case Q_TYPE_SECOND:
            TEST_ASSERT_EQUAL_INT(*(QSecond *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QSecond *)value, qGetSecond(obj));
            break;
        case Q_TYPE_TIME:
            TEST_ASSERT_EQUAL_INT(*(QTime *)value, obj->qint);
            TEST_ASSERT_EQUAL_INT(*(QTime *)value, qGetTime(obj));
            break;
        default:
            TEST_FAIL_MESSAGE("type");
    }
}

/**
 * @brief Check that a Q object is a list of the given type and verify basic properties.
 *
 * @param obj Q object to check.
 * @param type Type code to check for.
 */
static void checkListType(QObj *obj, QTypeCode type) {
    if (obj->type < 0)
        TEST_FAIL_MESSAGE("Not list");

    TEST_ASSERT_EQUAL_INT(type, obj->type);
    TEST_ASSERT_FALSE(qIsAtom(obj));
    TEST_ASSERT_TRUE(qIsList(obj));
    TEST_ASSERT_TRUE(qIsSimpleList(obj));
    TEST_ASSERT_TRUE(qIsListType(obj, type));

    TEST_ASSERT_FALSE(qIsMixedList(obj));
    TEST_ASSERT_FALSE(qIsDict(obj));
    TEST_ASSERT_FALSE(qIsTable(obj));
    TEST_ASSERT_FALSE(qIsKeyedTable(obj));
}

/**
 * @brief Check that a list contains the expected items.
 *
 * @param obj Q object containing a list.
 * @param type Type code of the list items.
 * @param expectedValues Array of expected values.
 * @param length Number of items in the list.
 */
static void checkListItems(QObj *obj, QTypeCode type, const void *expectedValues, QSize length) {
    TEST_ASSERT_EQUAL_INT(0, obj->refs);
    TEST_ASSERT_EQUAL_INT64(length, obj->length);

    switch (type) {
        case Q_TYPE_BOOLEAN: {
            const QBoolean *expected = (const QBoolean *)expectedValues;
            QBoolean *list = (QBoolean *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT(expected[i], list[i]);
                TEST_ASSERT_EQUAL_INT(expected[i], qGetBooleanAtIndex(obj, i));
            }
            break;
        }
        case Q_TYPE_GUID: {
            const QGuid *expected = (const QGuid *)expectedValues;
            const QGuid *list = (const QGuid *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_CHAR_ARRAY(expected[i].bytes, list[i].bytes, 16);
                TEST_ASSERT_EQUAL_CHAR_ARRAY(expected[i].bytes, qGetGuidAtIndex(obj, i)->bytes, 16);
            }
            break;
        }
        case Q_TYPE_BYTE: {
            const QByte *expected = (const QByte *)expectedValues;
            QByte *list = (QByte *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_SHORT: {
            const QShort *expected = (const QShort *)expectedValues;
            QShort *list = (QShort *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_INT: {
            const QInt *expected = (const QInt *)expectedValues;
            QInt *list = (QInt *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_LONG: {
            const QLong *expected = (const QLong *)expectedValues;
            QLong *list = (QLong *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT64(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_REAL: {
            const QReal *expected = (const QReal *)expectedValues;
            QReal *list = (QReal *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_FLOAT(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_FLOAT: {
            const QFloat *expected = (const QFloat *)expectedValues;
            QFloat *list = (QFloat *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_DOUBLE(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_CHAR: {
            const QChar *expected = (const QChar *)expectedValues;
            QChar *list = (QChar *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_CHAR(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_SYMBOL: {
            const QSymbol *expected = (const QSymbol *)expectedValues;
            QSymbol *list = (QSymbol *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_STRING(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_TIMESTAMP: {
            const QTimestamp *expected = (const QTimestamp *)expectedValues;
            QLong *list = (QLong *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT64(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_MONTH:
        case Q_TYPE_DATE:
        case Q_TYPE_MINUTE:
        case Q_TYPE_SECOND:
        case Q_TYPE_TIME: {
            const QInt *expected = (const QInt *)expectedValues;
            QInt *list = (QInt *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT(expected[i], list[i]);
            }
            break;
        }
        case Q_TYPE_DATETIME:
        case Q_TYPE_TIMESPAN: {
            const QLong *expected = (const QLong *)expectedValues;
            QLong *list = (QLong *)obj->list;
            for (QSize i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL_INT64(expected[i], list[i]);
            }
            break;
        }
        default:
            TEST_FAIL_MESSAGE("Unsupported list type");
    }
}

static void
checkBorrowedListItems(QObj *obj, QTypeCode type, const void *expectedValues, QSize length) {
    QInt refs = obj->refs;
    obj->refs = 0;
    checkListItems(obj, type, expectedValues, length);
    obj->refs = refs;
}

static void checkIsMixedList(QObj *obj) {
    TEST_ASSERT_EQUAL_INT(Q_TYPE_MIXED, obj->type);

    TEST_ASSERT_FALSE(qIsAtom(obj));
    TEST_ASSERT_TRUE(qIsList(obj));
    TEST_ASSERT_FALSE(qIsSimpleList(obj));
    TEST_ASSERT_TRUE(qIsListType(obj, Q_TYPE_MIXED));
    TEST_ASSERT_TRUE(qIsMixedList(obj));
    TEST_ASSERT_FALSE(qIsDict(obj));
    TEST_ASSERT_FALSE(qIsTable(obj));
    TEST_ASSERT_FALSE(qIsKeyedTable(obj));
}

static void checkIsDict(QObj *obj) {
    TEST_ASSERT_EQUAL_INT(Q_TYPE_DICTIONARY, obj->type);
    TEST_ASSERT_EQUAL_UINT64(2, obj->length);

    TEST_ASSERT_FALSE(qIsAtom(obj));
    TEST_ASSERT_FALSE(qIsList(obj));
    TEST_ASSERT_FALSE(qIsSimpleList(obj));
    TEST_ASSERT_FALSE(qIsMixedList(obj));
    TEST_ASSERT_TRUE(qIsDict(obj));
    TEST_ASSERT_FALSE(qIsTable(obj));
    TEST_ASSERT_FALSE(qIsKeyedTable(obj));
}

static void checkIsTable(QObj *obj) {
    TEST_ASSERT_EQUAL_INT(Q_TYPE_TABLE, obj->type);

    TEST_ASSERT_FALSE(qIsAtom(obj));
    TEST_ASSERT_FALSE(qIsList(obj));
    TEST_ASSERT_FALSE(qIsSimpleList(obj));
    TEST_ASSERT_FALSE(qIsMixedList(obj));
    TEST_ASSERT_FALSE(qIsDict(obj));
    TEST_ASSERT_TRUE(qIsTable(obj));
    TEST_ASSERT_FALSE(qIsKeyedTable(obj));
}

static void checkTableHeader(QObj *obj, QTypeCode type, const void *expectedNames, QSize length) {
    QObj *header = qGetTableHeader(obj);
    checkListItems(header, type, expectedNames, length);
}

static void checkTableColumn(
    QObj *obj, QTypeCode type, const void *expectedValues, QSize length, QSize colIndex) {
    QObj *column = qGetMixedAtIndex(qGetTableColumns(obj), colIndex);
    checkListItems(column, type, expectedValues, length);
}

static void
checkBorrowedTableHeader(QObj *obj, QTypeCode type, const void *expectedNames, QSize length) {
    QObj *header = qGetTableHeader(obj);
    checkBorrowedListItems(header, type, expectedNames, length);
}

static void checkBorrowedTableColumn(
    QObj *obj, QTypeCode type, const void *expectedValues, QSize length, QSize colIndex) {
    QObj *column = qGetMixedAtIndex(qGetTableColumns(obj), colIndex);
    checkBorrowedListItems(column, type, expectedValues, length);
}

static void checkIsKeyedTable(QObj *obj) {
    TEST_ASSERT_EQUAL_INT(Q_TYPE_DICTIONARY, obj->type);
    TEST_ASSERT_EQUAL_UINT64(2, obj->length);

    TEST_ASSERT_FALSE(qIsAtom(obj));
    TEST_ASSERT_FALSE(qIsList(obj));
    TEST_ASSERT_FALSE(qIsSimpleList(obj));
    TEST_ASSERT_FALSE(qIsMixedList(obj));
    TEST_ASSERT_FALSE(qIsDict(obj));
    TEST_ASSERT_FALSE(qIsTable(obj));
    TEST_ASSERT_TRUE(qIsKeyedTable(obj));
}

void testIsListKinds() {
    // Objects with type codes not produced by the creation functions (only the type is read)
    QObj unused = {.type = 3};
    QObj enumerated = {.type = 20};
    QObj lastEnumerated = {.type = 76};
    QObj anymap = {.type = 77};
    QObj lastNested = {.type = Q_TYPE_NESTED_SYM_ENUM};

    TEST_ASSERT_FALSE(qIsList(nullptr));
    TEST_ASSERT_FALSE(qIsSimpleList(nullptr));
    TEST_ASSERT_FALSE(qIsListType(nullptr, Q_TYPE_LONG));

    // Type 3 is unused by Q so is not a simple list
    TEST_ASSERT_FALSE(qIsSimpleList(&unused));

    // Enumerated and mapped/nested lists are lists, but not simple lists
    QObj *others[] = {&enumerated, &lastEnumerated, &anymap, &lastNested};
    for (QSize i = 0; i < sizeof(others) / sizeof(others[0]); i++) {
        TEST_ASSERT_TRUE(qIsList(others[i]));
        TEST_ASSERT_FALSE(qIsSimpleList(others[i]));
        TEST_ASSERT_TRUE(qIsListType(others[i], (QTypeCode)others[i]->type));
    }

    // Table and dictionary type codes are not list type codes
    QObj table = {.type = Q_TYPE_TABLE};
    QObj dict = {.type = Q_TYPE_DICTIONARY};
    TEST_ASSERT_FALSE(qIsListType(&table, Q_TYPE_TABLE));
    TEST_ASSERT_FALSE(qIsListType(&dict, Q_TYPE_DICTIONARY));
}

void testNewBoolean() {
    QByte expected;

    QObj *boolObj0 = qNewBoolean(0);
    checkAtomType(boolObj0, Q_TYPE_BOOLEAN);
    expected = 0;
    checkAtomValue(boolObj0, Q_TYPE_BOOLEAN, &expected);

    QObj *boolObj1 = qNewBoolean(1);
    checkAtomType(boolObj1, Q_TYPE_BOOLEAN);
    expected = 1;
    checkAtomValue(boolObj1, Q_TYPE_BOOLEAN, &expected);

    QObj *boolObj2 = qNewBoolean(2);
    checkAtomType(boolObj2, Q_TYPE_BOOLEAN);
    checkAtomValue(boolObj2, Q_TYPE_BOOLEAN, &expected);

    QObj *boolObjNeg = qNewBoolean(-1);
    checkAtomType(boolObjNeg, Q_TYPE_BOOLEAN);
    checkAtomValue(boolObjNeg, Q_TYPE_BOOLEAN, &expected);
}

void testNewGuid() {
    unsigned char bytes[16] = {
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
        0x88};

    QGuid rawGuid = {0};
    memcpy(rawGuid.bytes, bytes, 16);
    QObj *guidObj = qNewGuid(rawGuid);

    checkAtomType(guidObj, Q_TYPE_GUID);
    checkAtomValue(guidObj, Q_TYPE_GUID, bytes);
}

void testNewByte() {
    QByte expected;

    QObj *byteObj0 = qNewByte(0);
    checkAtomType(byteObj0, Q_TYPE_BYTE);
    expected = 0;
    checkAtomValue(byteObj0, Q_TYPE_BYTE, &expected);

    QObj *byteObj1 = qNewByte(10);
    checkAtomType(byteObj1, Q_TYPE_BYTE);
    expected = 0x0a;
    checkAtomValue(byteObj1, Q_TYPE_BYTE, &expected);

    // Positive byte overflow wraps
    QObj *byteObj3 = qNewByte((QByte)256);
    checkAtomType(byteObj3, Q_TYPE_BYTE);
    expected = 0x00;
    checkAtomValue(byteObj3, Q_TYPE_BYTE, &expected);

    // Negative byte overflow wraps
    QObj *byteObj2 = qNewByte((QByte)-1);
    checkAtomType(byteObj2, Q_TYPE_BYTE);
    expected = 0xff;
    checkAtomValue(byteObj2, Q_TYPE_BYTE, &expected);
}

void testNewShort() {
    QShort expected;

    expected = 0;
    QObj *shortObj0 = qNewShort(expected);
    checkAtomType(shortObj0, Q_TYPE_SHORT);
    checkAtomValue(shortObj0, Q_TYPE_SHORT, &expected);

    expected = 1;
    QObj *shortObj1 = qNewShort(expected);
    checkAtomType(shortObj1, Q_TYPE_SHORT);
    checkAtomValue(shortObj1, Q_TYPE_SHORT, &expected);

    expected = -1;
    QObj *shortObj2 = qNewShort(expected);
    checkAtomType(shortObj2, Q_TYPE_SHORT);
    checkAtomValue(shortObj2, Q_TYPE_SHORT, &expected);

    // Positive short overflow wraps
    QObj *shortObj3 = qNewShort((QShort)(Q_SHORT_INF + 1));
    checkAtomType(shortObj3, Q_TYPE_SHORT);
    expected = Q_SHORT_NULL;
    checkAtomValue(shortObj3, Q_TYPE_SHORT, &expected);

    // Negative short overflow wraps
    QObj *shortObj4 = qNewShort((QShort)(Q_SHORT_NULL - 1));
    checkAtomType(shortObj4, Q_TYPE_SHORT);
    expected = Q_SHORT_INF;
    checkAtomValue(shortObj4, Q_TYPE_SHORT, &expected);
}

void testNewInt() {
    QInt expected;

    expected = 0;
    QObj *intObj0 = qNewInt(expected);
    checkAtomType(intObj0, Q_TYPE_INT);
    checkAtomValue(intObj0, Q_TYPE_INT, &expected);

    expected = 1;
    QObj *intObj1 = qNewInt(expected);
    checkAtomType(intObj1, Q_TYPE_INT);
    checkAtomValue(intObj1, Q_TYPE_INT, &expected);

    expected = -1;
    QObj *intObj2 = qNewInt(expected);
    checkAtomType(intObj2, Q_TYPE_INT);
    checkAtomValue(intObj2, Q_TYPE_INT, &expected);

    expected = Q_INT_INF;
    QObj *intObj3 = qNewInt(expected);
    checkAtomType(intObj3, Q_TYPE_INT);
    checkAtomValue(intObj3, Q_TYPE_INT, &expected);

    expected = Q_INT_NULL;
    QObj *intObj4 = qNewInt(expected);
    checkAtomType(intObj4, Q_TYPE_INT);
    checkAtomValue(intObj4, Q_TYPE_INT, &expected);
}

void testNewLong() {
    QLong expected;

    expected = 0;
    QObj *longObj0 = qNewLong(expected);
    checkAtomType(longObj0, Q_TYPE_LONG);
    checkAtomValue(longObj0, Q_TYPE_LONG, &expected);

    expected = 1;
    QObj *longObj1 = qNewLong(expected);
    checkAtomType(longObj1, Q_TYPE_LONG);
    checkAtomValue(longObj1, Q_TYPE_LONG, &expected);

    expected = -1;
    QObj *longObj2 = qNewLong(expected);
    checkAtomType(longObj2, Q_TYPE_LONG);
    checkAtomValue(longObj2, Q_TYPE_LONG, &expected);

    expected = Q_LONG_INF;
    QObj *longObj3 = qNewLong(expected);
    checkAtomType(longObj3, Q_TYPE_LONG);
    checkAtomValue(longObj3, Q_TYPE_LONG, &expected);

    expected = Q_LONG_NULL;
    QObj *longObj4 = qNewLong(expected);
    checkAtomType(longObj4, Q_TYPE_LONG);
    checkAtomValue(longObj4, Q_TYPE_LONG, &expected);
}

void testNewReal() {
    QReal expected;

    expected = 0.0f;
    QObj *realObj0 = qNewReal(expected);
    checkAtomType(realObj0, Q_TYPE_REAL);
    checkAtomValue(realObj0, Q_TYPE_REAL, &expected);

    expected = 3.14f;
    QObj *realObj1 = qNewReal(expected);
    checkAtomType(realObj1, Q_TYPE_REAL);
    checkAtomValue(realObj1, Q_TYPE_REAL, &expected);

    expected = -3.14f;
    QObj *realObj2 = qNewReal(expected);
    checkAtomType(realObj2, Q_TYPE_REAL);
    checkAtomValue(realObj2, Q_TYPE_REAL, &expected);

    expected = Q_FLOAT_NULL;
    QObj *realObj3 = qNewReal(expected);
    checkAtomType(realObj3, Q_TYPE_REAL);
    checkAtomValue(realObj3, Q_TYPE_REAL, &expected);

    expected = Q_FLOAT_INF;
    QObj *realObj4 = qNewReal(expected);
    checkAtomType(realObj4, Q_TYPE_REAL);
    checkAtomValue(realObj4, Q_TYPE_REAL, &expected);
}

void testNewFloat() {
    QFloat expected;

    expected = 0.0;
    QObj *floatObj0 = qNewFloat(expected);
    checkAtomType(floatObj0, Q_TYPE_FLOAT);
    checkAtomValue(floatObj0, Q_TYPE_FLOAT, &expected);

    expected = 3.14;
    QObj *floatObj1 = qNewFloat(expected);
    checkAtomType(floatObj1, Q_TYPE_FLOAT);
    checkAtomValue(floatObj1, Q_TYPE_FLOAT, &expected);

    expected = -3.14;
    QObj *floatObj2 = qNewFloat(expected);
    checkAtomType(floatObj2, Q_TYPE_FLOAT);
    checkAtomValue(floatObj2, Q_TYPE_FLOAT, &expected);

    expected = Q_FLOAT_NULL;
    QObj *floatObj3 = qNewFloat(expected);
    checkAtomType(floatObj3, Q_TYPE_FLOAT);
    checkAtomValue(floatObj3, Q_TYPE_FLOAT, &expected);

    expected = Q_FLOAT_INF;
    QObj *floatObj4 = qNewFloat(expected);
    checkAtomType(floatObj4, Q_TYPE_FLOAT);
    checkAtomValue(floatObj4, Q_TYPE_FLOAT, &expected);
}

void testNewChar() {
    QChar expected;

    expected = 'a';
    QObj *charObj0 = qNewChar(expected);
    checkAtomType(charObj0, Q_TYPE_CHAR);
    checkAtomValue(charObj0, Q_TYPE_CHAR, &expected);

    expected = 'Z';
    QObj *charObj1 = qNewChar(expected);
    checkAtomType(charObj1, Q_TYPE_CHAR);
    checkAtomValue(charObj1, Q_TYPE_CHAR, &expected);

    expected = '\n';
    QObj *charObj2 = qNewChar(expected);
    checkAtomType(charObj2, Q_TYPE_CHAR);
    checkAtomValue(charObj2, Q_TYPE_CHAR, &expected);

    // Positive char overflow wraps
    QObj *charObj3 = qNewChar((QChar)(CHAR_MAX + 1));
    checkAtomType(charObj3, Q_TYPE_CHAR);
    expected = CHAR_MIN;
    checkAtomValue(charObj3, Q_TYPE_CHAR, &expected);

    // Negative char overflow wraps
    QObj *charObj4 = qNewChar((QChar)(CHAR_MIN - 1));
    checkAtomType(charObj4, Q_TYPE_CHAR);
    expected = CHAR_MAX;
    checkAtomValue(charObj4, Q_TYPE_CHAR, &expected);
}

void testNewSymbol() {
    QSymbol expected;

    expected = "hello";
    QObj *symbolObj0 = qNewSymbol(expected);
    checkAtomType(symbolObj0, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj0, Q_TYPE_SYMBOL, expected);

    // Should refer to same memory location (interned string)
    QObj *symbolObj01 = qNewSymbol(expected);
    TEST_ASSERT_EQUAL_PTR(qGetSymbol(symbolObj0), qGetSymbol(symbolObj01));

    expected = "";
    QObj *symbolObj1 = qNewSymbol(expected);
    checkAtomType(symbolObj1, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj1, Q_TYPE_SYMBOL, expected);

    expected = "symbol with spaces";
    QObj *symbolObj2 = qNewSymbol(expected);
    checkAtomType(symbolObj2, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj2, Q_TYPE_SYMBOL, expected);

    expected = "symbol-with@specials$characters";
    QObj *symbolObj3 = qNewSymbol(expected);
    checkAtomType(symbolObj3, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj3, Q_TYPE_SYMBOL, expected);
}

void testNewTimestamp() {
    QTimestamp expected;

    expected = 0;
    QObj *timestampObj0 = qNewTimestamp(expected);
    checkAtomType(timestampObj0, Q_TYPE_TIMESTAMP);
    checkAtomValue(timestampObj0, Q_TYPE_TIMESTAMP, &expected);

    expected = 821977817123456789;
    QObj *timestampObj1 = qNewTimestamp(expected);
    checkAtomType(timestampObj1, Q_TYPE_TIMESTAMP);
    checkAtomValue(timestampObj1, Q_TYPE_TIMESTAMP, &expected);

    expected = -124793382876543211;
    QObj *timestampObj2 = qNewTimestamp(expected);
    checkAtomType(timestampObj2, Q_TYPE_TIMESTAMP);
    checkAtomValue(timestampObj2, Q_TYPE_TIMESTAMP, &expected);
}

void testNewMonth() {
    QMonth expected;

    expected = 0;
    QObj *monthObj0 = qNewMonth(expected);
    checkAtomType(monthObj0, Q_TYPE_MONTH);
    checkAtomValue(monthObj0, Q_TYPE_MONTH, &expected);

    expected = 312;
    QObj *monthObj1 = qNewMonth(expected);
    checkAtomType(monthObj1, Q_TYPE_MONTH);
    checkAtomValue(monthObj1, Q_TYPE_MONTH, &expected);

    expected = -48;
    QObj *monthObj2 = qNewMonth(expected);
    checkAtomType(monthObj2, Q_TYPE_MONTH);
    checkAtomValue(monthObj2, Q_TYPE_MONTH, &expected);
}

void testNewDate() {
    QDate expected;

    expected = 0;
    QObj *dateObj0 = qNewDate(expected);
    checkAtomType(dateObj0, Q_TYPE_DATE);
    checkAtomValue(dateObj0, Q_TYPE_DATE, &expected);

    expected = 9513;
    QObj *dateObj1 = qNewDate(expected);
    checkAtomType(dateObj1, Q_TYPE_DATE);
    checkAtomValue(dateObj1, Q_TYPE_DATE, &expected);

    expected = -1445;
    QObj *dateObj2 = qNewDate(expected);
    checkAtomType(dateObj2, Q_TYPE_DATE);
    checkAtomValue(dateObj2, Q_TYPE_DATE, &expected);
}

void testNewDatetime() {
    QDatetime expected;

    expected = 0;
    QObj *datetimeObj0 = qNewDatetime(expected);
    checkAtomType(datetimeObj0, Q_TYPE_DATETIME);
    checkAtomValue(datetimeObj0, Q_TYPE_DATETIME, &expected);

    expected = 9513.6321426273153;
    QObj *datetimeObj1 = qNewDatetime(expected);
    checkAtomType(datetimeObj1, Q_TYPE_DATETIME);
    checkAtomValue(datetimeObj1, Q_TYPE_DATETIME, &expected);

    expected = -1444.3678573726852;
    QObj *datetimeObj2 = qNewDatetime(expected);
    checkAtomType(datetimeObj2, Q_TYPE_DATETIME);
    checkAtomValue(datetimeObj2, Q_TYPE_DATETIME, &expected);
}

void testNewTimespan() {
    QTimespan expected;

    expected = 0;
    QObj *timespanObj0 = qNewTimespan(expected);
    checkAtomType(timespanObj0, Q_TYPE_TIMESPAN);
    checkAtomValue(timespanObj0, Q_TYPE_TIMESPAN, &expected);

    expected = 1473576123456789;
    QObj *timespanObj1 = qNewTimespan(expected);
    checkAtomType(timespanObj1, Q_TYPE_TIMESPAN);
    checkAtomValue(timespanObj1, Q_TYPE_TIMESPAN, &expected);

    expected = -1473576123456789;
    QObj *timespanObj2 = qNewTimespan(expected);
    checkAtomType(timespanObj2, Q_TYPE_TIMESPAN);
    checkAtomValue(timespanObj2, Q_TYPE_TIMESPAN, &expected);
}

void testNewMinute() {
    QMinute expected;

    expected = 0;
    QObj *minuteObj0 = qNewMinute(expected);
    checkAtomType(minuteObj0, Q_TYPE_MINUTE);
    checkAtomValue(minuteObj0, Q_TYPE_MINUTE, &expected);

    expected = 980;
    QObj *minuteObj1 = qNewMinute(expected);
    checkAtomType(minuteObj1, Q_TYPE_MINUTE);
    checkAtomValue(minuteObj1, Q_TYPE_MINUTE, &expected);

    expected = -980;
    QObj *minuteObj2 = qNewMinute(expected);
    checkAtomType(minuteObj2, Q_TYPE_MINUTE);
    checkAtomValue(minuteObj2, Q_TYPE_MINUTE, &expected);
}

void testNewSecond() {
    QSecond expected;

    expected = 0;
    QObj *secondObj0 = qNewSecond(expected);
    checkAtomType(secondObj0, Q_TYPE_SECOND);
    checkAtomValue(secondObj0, Q_TYPE_SECOND, &expected);

    expected = 58817;
    QObj *secondObj1 = qNewSecond(expected);
    checkAtomType(secondObj1, Q_TYPE_SECOND);
    checkAtomValue(secondObj1, Q_TYPE_SECOND, &expected);

    expected = -58817;
    QObj *secondObj2 = qNewSecond(expected);
    checkAtomType(secondObj2, Q_TYPE_SECOND);
    checkAtomValue(secondObj2, Q_TYPE_SECOND, &expected);
}

void testNewTime() {
    QTime expected;

    expected = 0;
    QObj *timeObj0 = qNewTime(expected);
    checkAtomType(timeObj0, Q_TYPE_TIME);
    checkAtomValue(timeObj0, Q_TYPE_TIME, &expected);

    expected = 58817123;
    QObj *timeObj1 = qNewTime(expected);
    checkAtomType(timeObj1, Q_TYPE_TIME);
    checkAtomValue(timeObj1, Q_TYPE_TIME, &expected);

    expected = -58817123;
    QObj *timeObj2 = qNewTime(expected);
    checkAtomType(timeObj2, Q_TYPE_TIME);
    checkAtomValue(timeObj2, Q_TYPE_TIME, &expected);
}

void testNewList() {
    // Test empty boolean list
    QObj *emptyBoolList = qNewList(Q_TYPE_BOOLEAN, 0);
    checkListType(emptyBoolList, Q_TYPE_BOOLEAN);
    TEST_ASSERT_EQUAL_INT64(0, emptyBoolList->length);

    // Single item int list
    QObj *singleIntList = qNewList(Q_TYPE_INT, 1);
    checkListType(singleIntList, Q_TYPE_INT);
    TEST_ASSERT_EQUAL_INT64(1, singleIntList->length);

    // Test multiple item long list
    QObj *multiLongList = qNewList(Q_TYPE_LONG, 10);
    checkListType(multiLongList, Q_TYPE_LONG);
    TEST_ASSERT_EQUAL_INT64(10, multiLongList->length);

    // Test large list
    QObj *largeBoolList = qNewList(Q_TYPE_BOOLEAN, 1000);
    checkListType(largeBoolList, Q_TYPE_BOOLEAN);
    TEST_ASSERT_EQUAL_INT64(1000, largeBoolList->length);

    // Test different types
    QObj *byteList = qNewList(Q_TYPE_BYTE, 5);
    checkListType(byteList, Q_TYPE_BYTE);
    TEST_ASSERT_EQUAL_INT64(5, byteList->length);

    QObj *floatList = qNewList(Q_TYPE_FLOAT, 7);
    checkListType(floatList, Q_TYPE_FLOAT);
    TEST_ASSERT_EQUAL_INT64(7, floatList->length);

    QObj *symbolList = qNewList(Q_TYPE_SYMBOL, 3);
    checkListType(symbolList, Q_TYPE_SYMBOL);
    TEST_ASSERT_EQUAL_INT64(3, symbolList->length);

    // Length too large
    QObj *bigList = qNewList(Q_TYPE_LONG, Q_SIZE_MAX + 1);
    QObj *error = qCheckError(bigList);
    TEST_ASSERT_TRUE(qIsError(error));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error));
}

void testNewBooleanList() {
    // Empty list
    QBoolean values0[] = {};
    QObj *listObj0 = qNewBooleanList(values0, 0);
    checkListType(listObj0, Q_TYPE_BOOLEAN);
    checkListItems(listObj0, Q_TYPE_BOOLEAN, values0, 0);

    // Single item (false)
    QBoolean values1[] = {0};
    QObj *listObj1 = qNewBooleanList(values1, 1);
    checkListType(listObj1, Q_TYPE_BOOLEAN);
    checkListItems(listObj1, Q_TYPE_BOOLEAN, values1, 1);

    // Single item (true)
    QBoolean values2[] = {1};
    QObj *listObj2 = qNewBooleanList(values2, 1);
    checkListType(listObj2, Q_TYPE_BOOLEAN);
    checkListItems(listObj2, Q_TYPE_BOOLEAN, values2, 1);

    // Multiple items
    QBoolean values3[] = {0, 1, 1, 0, 1};
    QObj *listObj3 = qNewBooleanList(values3, 5);
    checkListType(listObj3, Q_TYPE_BOOLEAN);
    checkListItems(listObj3, Q_TYPE_BOOLEAN, values3, 5);

    // Test with values that wrap (non-boolean inputs treated as 0 or 1)
    QBoolean values4[] = {0, -1, 100};
    QObj *listObj4 = qNewBooleanList(values4, 3);
    QBoolean expected[] = {0, 1, 1};
    checkListType(listObj4, Q_TYPE_BOOLEAN);
    checkListItems(listObj4, Q_TYPE_BOOLEAN, expected, 3);
}

void testNewGuidList() {
    // Empty list
    QGuid values0[] = {};
    QObj *listObj0 = qNewGuidList(values0, 0);
    checkListType(listObj0, Q_TYPE_GUID);
    checkListItems(listObj0, Q_TYPE_GUID, values0, 0);

    // Single item
    unsigned char bytes1[16] = {
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
        0x88};
    QGuid values1[1];
    memcpy(values1[0].bytes, bytes1, 16);
    QObj *listObj1 = qNewGuidList(values1, 1);
    checkListType(listObj1, Q_TYPE_GUID);
    checkListItems(listObj1, Q_TYPE_GUID, values1, 1);

    // Multiple items
    unsigned char bytes2[16] = {
        0xAA,
        0xBB,
        0xCC,
        0xDD,
        0xEE,
        0xFF,
        0x00,
        0x11,
        0x22,
        0x33,
        0x44,
        0x55,
        0x66,
        0x77,
        0x88,
        0x99};
    unsigned char bytes3[16] = {
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00};
    QGuid values2[3];
    memcpy(values2[0].bytes, bytes1, 16);
    memcpy(values2[1].bytes, bytes2, 16);
    memcpy(values2[2].bytes, bytes3, 16);
    QObj *listObj2 = qNewGuidList(values2, 3);
    checkListType(listObj2, Q_TYPE_GUID);
    checkListItems(listObj2, Q_TYPE_GUID, values2, 3);
}

void testNewByteList() {
    // Empty list
    QByte values0[] = {};
    QObj *listObj0 = qNewByteList(values0, 0);
    checkListType(listObj0, Q_TYPE_BYTE);
    checkListItems(listObj0, Q_TYPE_BYTE, values0, 0);

    // Single item
    QByte values1[] = {42};
    QObj *listObj1 = qNewByteList(values1, 1);
    checkListType(listObj1, Q_TYPE_BYTE);
    checkListItems(listObj1, Q_TYPE_BYTE, values1, 1);

    // Multiple items
    QByte values2[] = {0, 128, 255, 64, 200};
    QObj *listObj2 = qNewByteList(values2, 5);
    checkListType(listObj2, Q_TYPE_BYTE);
    checkListItems(listObj2, Q_TYPE_BYTE, values2, 5);
}

void testNewShortList() {
    // Empty list
    QShort values0[] = {};
    QObj *listObj0 = qNewShortList(values0, 0);
    checkListType(listObj0, Q_TYPE_SHORT);
    checkListItems(listObj0, Q_TYPE_SHORT, values0, 0);

    // Single item
    QShort values1[] = {100};
    QObj *listObj1 = qNewShortList(values1, 1);
    checkListType(listObj1, Q_TYPE_SHORT);
    checkListItems(listObj1, Q_TYPE_SHORT, values1, 1);

    // Multiple items with edge cases
    QShort values2[] = {0, 32767, -32768, 1000, -1000};
    QObj *listObj2 = qNewShortList(values2, 5);
    checkListType(listObj2, Q_TYPE_SHORT);
    checkListItems(listObj2, Q_TYPE_SHORT, values2, 5);
}

void testNewIntList() {
    // Empty list
    QInt values0[] = {};
    QObj *listObj0 = qNewIntList(values0, 0);
    checkListType(listObj0, Q_TYPE_INT);
    checkListItems(listObj0, Q_TYPE_INT, values0, 0);

    // Single item
    QInt values1[] = {42};
    QObj *listObj1 = qNewIntList(values1, 1);
    checkListType(listObj1, Q_TYPE_INT);
    checkListItems(listObj1, Q_TYPE_INT, values1, 1);

    // Multiple items with edge cases
    QInt values2[] = {0, 2147483647, -2147483648, 1000000, -1000000};
    QObj *listObj2 = qNewIntList(values2, 5);
    checkListType(listObj2, Q_TYPE_INT);
    checkListItems(listObj2, Q_TYPE_INT, values2, 5);
}

void testNewLongList() {
    // Empty list
    QLong values0[] = {};
    QObj *listObj0 = qNewLongList(values0, 0);
    checkListType(listObj0, Q_TYPE_LONG);
    checkListItems(listObj0, Q_TYPE_LONG, values0, 0);

    // Single item
    QLong values1[] = {1234567890123456789LL};
    QObj *listObj1 = qNewLongList(values1, 1);
    checkListType(listObj1, Q_TYPE_LONG);
    checkListItems(listObj1, Q_TYPE_LONG, values1, 1);

    // Multiple items with edge cases
    QLong values2[] = {
        0, 9223372036854775807LL, -9223372036854775807LL, 1000000000000000LL, -1000000000000000LL};
    QObj *listObj2 = qNewLongList(values2, 5);
    checkListType(listObj2, Q_TYPE_LONG);
    checkListItems(listObj2, Q_TYPE_LONG, values2, 5);
}

void testNewRealList() {
    // Empty list
    QReal values0[] = {};
    QObj *listObj0 = qNewRealList(values0, 0);
    checkListType(listObj0, Q_TYPE_REAL);
    checkListItems(listObj0, Q_TYPE_REAL, values0, 0);

    // Single item
    QReal values1[] = {3.14f};
    QObj *listObj1 = qNewRealList(values1, 1);
    checkListType(listObj1, Q_TYPE_REAL);
    checkListItems(listObj1, Q_TYPE_REAL, values1, 1);

    // Multiple items
    QReal values2[] = {0.0f, 3.14f, -2.71f, 1.41f, -1.73f};
    QObj *listObj2 = qNewRealList(values2, 5);
    checkListType(listObj2, Q_TYPE_REAL);
    checkListItems(listObj2, Q_TYPE_REAL, values2, 5);
}

void testNewFloatList() {
    // Empty list
    QFloat values0[] = {};
    QObj *listObj0 = qNewFloatList(values0, 0);
    checkListType(listObj0, Q_TYPE_FLOAT);
    checkListItems(listObj0, Q_TYPE_FLOAT, values0, 0);

    // Single item
    QFloat values1[] = {3.141592653589793};
    QObj *listObj1 = qNewFloatList(values1, 1);
    checkListType(listObj1, Q_TYPE_FLOAT);
    checkListItems(listObj1, Q_TYPE_FLOAT, values1, 1);

    // Multiple items
    QFloat values2[] = {
        0.0, 3.141592653589793, -2.718281828459045, 1.414213562373095, -1.732050807568877};
    QObj *listObj2 = qNewFloatList(values2, 5);
    checkListType(listObj2, Q_TYPE_FLOAT);
    checkListItems(listObj2, Q_TYPE_FLOAT, values2, 5);
}

void testNewCharList() {
    // Empty list
    QChar values0[] = {};
    QObj *listObj0 = qNewCharList(values0, 0);
    checkListType(listObj0, Q_TYPE_CHAR);
    checkListItems(listObj0, Q_TYPE_CHAR, values0, 0);

    // Single item
    QChar values1[] = {'a'};
    QObj *listObj1 = qNewCharList(values1, 1);
    checkListType(listObj1, Q_TYPE_CHAR);
    checkListItems(listObj1, Q_TYPE_CHAR, values1, 1);

    // Multiple items
    QChar values2[] = {'a', 'Z', '\n', '0', '@'};
    QObj *listObj2 = qNewCharList(values2, 5);
    checkListType(listObj2, Q_TYPE_CHAR);
    checkListItems(listObj2, Q_TYPE_CHAR, values2, 5);
}

void testNewCharListFromString() {
    // Empty string
    QChar *string0 = "";
    QObj *listObj0 = qNewCharListFromString(string0);
    checkListType(listObj0, Q_TYPE_CHAR);
    checkListItems(listObj0, Q_TYPE_CHAR, string0, 0);

    // String of length 1
    QChar *string1 = "a";
    QObj *listObj1 = qNewCharListFromString(string1);
    checkListType(listObj1, Q_TYPE_CHAR);
    checkListItems(listObj1, Q_TYPE_CHAR, string1, 1);

    // String of length > 1
    QChar *string2 = "hello\nworld";
    QObj *listObj2 = qNewCharListFromString(string2);
    checkListType(listObj2, Q_TYPE_CHAR);
    checkListItems(listObj2, Q_TYPE_CHAR, string2, 11);
}

void testNewCharListFromFixedString() {
    // Empty string
    QChar *string0 = "";
    QObj *listObj0 = qNewCharListFromFixedString(string0, 0);
    checkListType(listObj0, Q_TYPE_CHAR);
    checkListItems(listObj0, Q_TYPE_CHAR, string0, 0);

    // String of length 1
    QChar *string1 = "a";
    QObj *listObj1 = qNewCharListFromFixedString(string1, 1);
    checkListType(listObj1, Q_TYPE_CHAR);
    checkListItems(listObj1, Q_TYPE_CHAR, string1, 1);

    // String of length > 1
    QChar *string2 = "hello\nworld";
    QObj *listObj2 = qNewCharListFromFixedString(string2, 11);
    checkListType(listObj2, Q_TYPE_CHAR);
    checkListItems(listObj2, Q_TYPE_CHAR, string2, 11);

    // Length too large
    QObj *bigString = qNewCharListFromFixedString("...", Q_SIZE_MAX + 1);
    QObj *error = qCheckError(bigString);
    TEST_ASSERT_TRUE(qIsError(error));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error));
}

void testNewSymbolList() {
    // Empty list
    QSymbol values0[] = {};
    QObj *listObj0 = qNewSymbolList(values0, 0);
    checkListType(listObj0, Q_TYPE_SYMBOL);
    checkListItems(listObj0, Q_TYPE_SYMBOL, values0, 0);

    // Single item
    QSymbol values1[] = {"hello"};
    QObj *listObj1 = qNewSymbolList(values1, 1);
    checkListType(listObj1, Q_TYPE_SYMBOL);
    checkListItems(listObj1, Q_TYPE_SYMBOL, values1, 1);

    // Multiple items
    QSymbol values2[] = {"hello", "world", "", "symbol-123", "test@symbol"};
    QObj *listObj2 = qNewSymbolList(values2, 5);
    checkListType(listObj2, Q_TYPE_SYMBOL);
    checkListItems(listObj2, Q_TYPE_SYMBOL, values2, 5);

    // Same symbol should refer to same memory location (interned string)
    char hello0[] = "hello";
    char hello1[] = "hello";
    char *values3[] = {hello0, "world", hello1};
    QObj *listObj3 = qNewSymbolList(values3, 3);
    TEST_ASSERT_EQUAL_PTR(qGetSymbolAtIndex(listObj3, 0), qGetSymbolAtIndex(listObj3, 2));
    TEST_ASSERT_TRUE(qGetSymbolAtIndex(listObj3, 0) != qGetSymbolAtIndex(listObj3, 1));
}

void testNewTimestampList() {
    // Empty list
    QTimestamp values0[] = {};
    QObj *listObj0 = qNewTimestampList(values0, 0);
    checkListType(listObj0, Q_TYPE_TIMESTAMP);
    checkListItems(listObj0, Q_TYPE_TIMESTAMP, values0, 0);

    // Single item
    QTimestamp values1[] = {821977817123456789LL};
    QObj *listObj1 = qNewTimestampList(values1, 1);
    checkListType(listObj1, Q_TYPE_TIMESTAMP);
    checkListItems(listObj1, Q_TYPE_TIMESTAMP, values1, 1);

    // Multiple items
    QTimestamp values2[] = {
        0,
        821977817123456789LL,
        -124793382876543211LL,
        1000000000000000000LL,
        -1000000000000000000LL};
    QObj *listObj2 = qNewTimestampList(values2, 5);
    checkListType(listObj2, Q_TYPE_TIMESTAMP);
    checkListItems(listObj2, Q_TYPE_TIMESTAMP, values2, 5);
}

void testNewMonthList() {
    // Empty list
    QMonth values0[] = {};
    QObj *listObj0 = qNewMonthList(values0, 0);
    checkListType(listObj0, Q_TYPE_MONTH);
    checkListItems(listObj0, Q_TYPE_MONTH, values0, 0);

    // Single item
    QMonth values1[] = {312};
    QObj *listObj1 = qNewMonthList(values1, 1);
    checkListType(listObj1, Q_TYPE_MONTH);
    checkListItems(listObj1, Q_TYPE_MONTH, values1, 1);

    // Multiple items
    QMonth values2[] = {0, 312, -48, 24, 600};
    QObj *listObj2 = qNewMonthList(values2, 5);
    checkListType(listObj2, Q_TYPE_MONTH);
    checkListItems(listObj2, Q_TYPE_MONTH, values2, 5);
}

void testNewDateList() {
    // Empty list
    QDate values0[] = {};
    QObj *listObj0 = qNewDateList(values0, 0);
    checkListType(listObj0, Q_TYPE_DATE);
    checkListItems(listObj0, Q_TYPE_DATE, values0, 0);

    // Single item
    QDate values1[] = {9513};
    QObj *listObj1 = qNewDateList(values1, 1);
    checkListType(listObj1, Q_TYPE_DATE);
    checkListItems(listObj1, Q_TYPE_DATE, values1, 1);

    // Multiple items
    QDate values2[] = {0, 9513, -1445, 18000, -10000};
    QObj *listObj2 = qNewDateList(values2, 5);
    checkListType(listObj2, Q_TYPE_DATE);
    checkListItems(listObj2, Q_TYPE_DATE, values2, 5);
}

void testNewDatetimeList() {
    // Empty list
    QDatetime values0[] = {};
    QObj *listObj0 = qNewDatetimeList(values0, 0);
    checkListType(listObj0, Q_TYPE_DATETIME);
    checkListItems(listObj0, Q_TYPE_DATETIME, values0, 0);

    // Single item
    QDatetime values1[] = {9513.6321426273153};
    QObj *listObj1 = qNewDatetimeList(values1, 1);
    checkListType(listObj1, Q_TYPE_DATETIME);
    checkListItems(listObj1, Q_TYPE_DATETIME, values1, 1);

    // Multiple items
    QDatetime values2[] = {0.0, 9513.6321426273153, -1444.3678573726852, 18000.5, -10000.25};
    QObj *listObj2 = qNewDatetimeList(values2, 5);
    checkListType(listObj2, Q_TYPE_DATETIME);
    checkListItems(listObj2, Q_TYPE_DATETIME, values2, 5);
}

void testNewTimespanList() {
    // Empty list
    QTimespan values0[] = {};
    QObj *listObj0 = qNewTimespanList(values0, 0);
    checkListType(listObj0, Q_TYPE_TIMESPAN);
    checkListItems(listObj0, Q_TYPE_TIMESPAN, values0, 0);

    // Single item
    QTimespan values1[] = {1473576123456789LL};
    QObj *listObj1 = qNewTimespanList(values1, 1);
    checkListType(listObj1, Q_TYPE_TIMESPAN);
    checkListItems(listObj1, Q_TYPE_TIMESPAN, values1, 1);

    // Multiple items
    QTimespan values2[] = {
        0, 1473576123456789LL, -1473576123456789LL, 86400000000000LL, -86400000000000LL};
    QObj *listObj2 = qNewTimespanList(values2, 5);
    checkListType(listObj2, Q_TYPE_TIMESPAN);
    checkListItems(listObj2, Q_TYPE_TIMESPAN, values2, 5);
}

void testNewMinuteList() {
    // Empty list
    QMinute values0[] = {};
    QObj *listObj0 = qNewMinuteList(values0, 0);
    checkListType(listObj0, Q_TYPE_MINUTE);
    checkListItems(listObj0, Q_TYPE_MINUTE, values0, 0);

    // Single item
    QMinute values1[] = {980};
    QObj *listObj1 = qNewMinuteList(values1, 1);
    checkListType(listObj1, Q_TYPE_MINUTE);
    checkListItems(listObj1, Q_TYPE_MINUTE, values1, 1);

    // Multiple items
    QMinute values2[] = {0, 980, -980, 1440, -1440};
    QObj *listObj2 = qNewMinuteList(values2, 5);
    checkListType(listObj2, Q_TYPE_MINUTE);
    checkListItems(listObj2, Q_TYPE_MINUTE, values2, 5);
}

void testNewSecondList() {
    // Empty list
    QSecond values0[] = {};
    QObj *listObj0 = qNewSecondList(values0, 0);
    checkListType(listObj0, Q_TYPE_SECOND);
    checkListItems(listObj0, Q_TYPE_SECOND, values0, 0);

    // Single item
    QSecond values1[] = {58817};
    QObj *listObj1 = qNewSecondList(values1, 1);
    checkListType(listObj1, Q_TYPE_SECOND);
    checkListItems(listObj1, Q_TYPE_SECOND, values1, 1);

    // Multiple items
    QSecond values2[] = {0, 58817, -58817, 86400, -86400};
    QObj *listObj2 = qNewSecondList(values2, 5);
    checkListType(listObj2, Q_TYPE_SECOND);
    checkListItems(listObj2, Q_TYPE_SECOND, values2, 5);
}

void testNewTimeList() {
    // Empty list
    QTime values0[] = {};
    QObj *listObj0 = qNewTimeList(values0, 0);
    checkListType(listObj0, Q_TYPE_TIME);
    checkListItems(listObj0, Q_TYPE_TIME, values0, 0);

    // Single item
    QTime values1[] = {58817123};
    QObj *listObj1 = qNewTimeList(values1, 1);
    checkListType(listObj1, Q_TYPE_TIME);
    checkListItems(listObj1, Q_TYPE_TIME, values1, 1);

    // Multiple items
    QTime values2[] = {0, 58817123, -58817123, 86400000, -86400000};
    QObj *listObj2 = qNewTimeList(values2, 5);
    checkListType(listObj2, Q_TYPE_TIME);
    checkListItems(listObj2, Q_TYPE_TIME, values2, 5);
}

static void checkError(QObj *obj, const char *expected) {
    TEST_ASSERT_NULL(obj);
    QObj *error = qCheckError(obj);
    TEST_ASSERT_TRUE(qIsError(error));
    TEST_ASSERT_EQUAL_STRING(expected, qGetError(error));
}

static void checkDomainError(QObj *obj) {
    checkError(obj, "domain");
}

/**
 * @brief Check that a function taking ownership of obj released exactly one reference to it.
 *
 * The caller takes an extra reference (incRef) before passing obj, so if the function released its
 * reference, refs is back to 0 and the caller still holds a valid object (released here).
 */
static void checkReleased(QObj *obj) {
    TEST_ASSERT_EQUAL_INT(0, obj->refs);
    decRef(obj);
}

void testNewListNullValues() {
    // Null values with zero length creates an empty list
    QObj *empty[] = {
        qNewBooleanList(nullptr, 0),   qNewGuidList(nullptr, 0),      qNewByteList(nullptr, 0),
        qNewShortList(nullptr, 0),     qNewIntList(nullptr, 0),       qNewLongList(nullptr, 0),
        qNewRealList(nullptr, 0),      qNewFloatList(nullptr, 0),     qNewCharList(nullptr, 0),
        qNewSymbolList(nullptr, 0),    qNewTimestampList(nullptr, 0), qNewMonthList(nullptr, 0),
        qNewDateList(nullptr, 0),      qNewDatetimeList(nullptr, 0),  qNewTimespanList(nullptr, 0),
        qNewMinuteList(nullptr, 0),    qNewSecondList(nullptr, 0),    qNewTimeList(nullptr, 0),
        qNewCharListFromFixedString(nullptr, 0),
    };
    QTypeCode emptyTypes[] = {
        Q_TYPE_BOOLEAN, Q_TYPE_GUID,     Q_TYPE_BYTE,   Q_TYPE_SHORT,    Q_TYPE_INT,
        Q_TYPE_LONG,    Q_TYPE_REAL,     Q_TYPE_FLOAT,  Q_TYPE_CHAR,     Q_TYPE_SYMBOL,
        Q_TYPE_TIMESTAMP, Q_TYPE_MONTH,  Q_TYPE_DATE,   Q_TYPE_DATETIME, Q_TYPE_TIMESPAN,
        Q_TYPE_MINUTE,  Q_TYPE_SECOND,   Q_TYPE_TIME,   Q_TYPE_CHAR,
    };
    for (QSize i = 0; i < sizeof(empty) / sizeof(empty[0]); i++) {
        TEST_ASSERT_NOT_NULL(empty[i]);
        checkListType(empty[i], emptyTypes[i]);
        TEST_ASSERT_EQUAL_UINT64(0, empty[i]->length);
        decRef(empty[i]);
    }

    // Null values with non-zero length is an error (checked immediately after each call, since
    // qCheckError clears the global error indicator)
    checkDomainError(qNewBooleanList(nullptr, 1));
    checkDomainError(qNewGuidList(nullptr, 1));
    checkDomainError(qNewByteList(nullptr, 1));
    checkDomainError(qNewShortList(nullptr, 1));
    checkDomainError(qNewIntList(nullptr, 1));
    checkDomainError(qNewLongList(nullptr, 1));
    checkDomainError(qNewRealList(nullptr, 1));
    checkDomainError(qNewFloatList(nullptr, 1));
    checkDomainError(qNewCharList(nullptr, 1));
    checkDomainError(qNewSymbolList(nullptr, 1));
    checkDomainError(qNewTimestampList(nullptr, 1));
    checkDomainError(qNewMonthList(nullptr, 1));
    checkDomainError(qNewDateList(nullptr, 1));
    checkDomainError(qNewDatetimeList(nullptr, 1));
    checkDomainError(qNewTimespanList(nullptr, 1));
    checkDomainError(qNewMinuteList(nullptr, 1));
    checkDomainError(qNewSecondList(nullptr, 1));
    checkDomainError(qNewTimeList(nullptr, 1));
    checkDomainError(qNewCharListFromFixedString(nullptr, 1));

    // Null string is an error
    checkDomainError(qNewCharListFromString(nullptr));
}

void testNewMixedList() {
    // Empty list
    QObj *mixedList0 = qNewMixedList(0);
    checkIsMixedList(mixedList0);
    TEST_ASSERT_EQUAL_UINT64(0, mixedList0->length);

    // Single item
    QObj *symbol = qNewSymbol("abc");
    QObj *mixedList1 = qNewMixedList(1, symbol);
    checkIsMixedList(mixedList1);
    TEST_ASSERT_EQUAL_UINT64(1, mixedList1->length);
    QObj *symbolObj1 = qGetMixedAtIndex(mixedList1, 0);
    checkAtomType(symbolObj1, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj1, Q_TYPE_SYMBOL, "abc");

    // Multiple items
    QLong longValue = 10;
    QObj *long_ = qNewLong(longValue);
    QChar charValues[] = {'a', 'b', 'c'};
    QObj *chars = qNewCharList(charValues, 3);
    QObj *mixedList2 = qNewMixedList(3, symbol, long_, chars);
    checkIsMixedList(mixedList2);
    TEST_ASSERT_EQUAL_UINT64(3, mixedList2->length);

    QObj *symbolObj2 = qGetMixedAtIndex(mixedList2, 0);
    checkAtomType(symbolObj2, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj2, Q_TYPE_SYMBOL, "abc");

    QObj *longObj2 = qGetMixedAtIndex(mixedList2, 1);
    checkAtomType(longObj2, Q_TYPE_LONG);
    checkAtomValue(longObj2, Q_TYPE_LONG, &longValue);

    QObj *charListObj2 = qGetMixedAtIndex(mixedList2, 2);
    checkListType(charListObj2, Q_TYPE_CHAR);
    checkListItems(charListObj2, Q_TYPE_CHAR, charValues, 3);

    // Nested items
    QObj *mixedList3 = qNewMixedList(4, symbol, long_, chars, mixedList2);
    checkIsMixedList(mixedList3);
    TEST_ASSERT_EQUAL_UINT64(4, mixedList3->length);

    QObj *mixedListObj3 = qGetMixedAtIndex(mixedList3, 3);
    checkIsMixedList(mixedListObj3);
    TEST_ASSERT_EQUAL_UINT64(3, mixedListObj3->length);
}

void testNewMixedListVar() {
    // Empty list
    QObj *mixedList0 = newMixedListVar(0);
    checkIsMixedList(mixedList0);
    TEST_ASSERT_EQUAL_UINT64(0, mixedList0->length);

    // Single item
    QObj *symbol = qNewSymbol("abc");
    QObj *mixedList1 = newMixedListVar(1, symbol);
    checkIsMixedList(mixedList1);
    TEST_ASSERT_EQUAL_UINT64(1, mixedList1->length);
    QObj *symbolObj1 = qGetMixedAtIndex(mixedList1, 0);
    checkAtomType(symbolObj1, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj1, Q_TYPE_SYMBOL, "abc");

    // Multiple items
    QLong longValue = 10;
    QObj *long_ = qNewLong(longValue);
    QChar charValues[] = {'a', 'b', 'c'};
    QObj *chars = qNewCharList(charValues, 3);
    QObj *mixedList2 = newMixedListVar(3, symbol, long_, chars);
    checkIsMixedList(mixedList2);
    TEST_ASSERT_EQUAL_UINT64(3, mixedList2->length);

    QObj *symbolObj2 = qGetMixedAtIndex(mixedList2, 0);
    checkAtomType(symbolObj2, Q_TYPE_SYMBOL);
    checkAtomValue(symbolObj2, Q_TYPE_SYMBOL, "abc");

    QObj *longObj2 = qGetMixedAtIndex(mixedList2, 1);
    checkAtomType(longObj2, Q_TYPE_LONG);
    checkAtomValue(longObj2, Q_TYPE_LONG, &longValue);

    QObj *charListObj2 = qGetMixedAtIndex(mixedList2, 2);
    checkListType(charListObj2, Q_TYPE_CHAR);
    checkListItems(charListObj2, Q_TYPE_CHAR, charValues, 3);

    // Nested items
    QObj *mixedList3 = newMixedListVar(4, symbol, long_, chars, mixedList2);
    checkIsMixedList(mixedList3);
    TEST_ASSERT_EQUAL_UINT64(4, mixedList3->length);

    QObj *mixedListObj3 = qGetMixedAtIndex(mixedList3, 3);
    checkIsMixedList(mixedListObj3);
    TEST_ASSERT_EQUAL_UINT64(3, mixedListObj3->length);
}

void testNewDict() {
    // Empty dict
    QSymbol symbols0[] = {};
    QObj *keys0 = qNewSymbolList(symbols0, 0);
    QLong longs0[] = {};
    QObj *values0 = qNewLongList(longs0, 0);
    QObj *dict0 = qNewDict(keys0, values0);
    checkIsDict(dict0);
    TEST_ASSERT_EQUAL_UINT64(0, qGetDictCount(dict0));
    TEST_ASSERT_EQUAL_UINT64(0, qGetDictKeys(dict0)->length);
    TEST_ASSERT_EQUAL_UINT64(0, qGetDictValues(dict0)->length);

    // Single key-value pair
    QSymbol symbols1[] = {"abc"};
    QObj *keys1 = qNewSymbolList(symbols1, 1);
    QLong longs1[] = {10};
    QObj *values1 = qNewLongList(longs1, 1);
    QObj *dict1 = qNewDict(keys1, values1);
    checkIsDict(dict1);
    TEST_ASSERT_EQUAL_UINT64(1, qGetDictCount(dict1));

    QObj *dictKeys1 = qGetDictKeys(dict1);
    TEST_ASSERT_EQUAL_UINT64(1, dictKeys1->length);
    checkListType(dictKeys1, Q_TYPE_SYMBOL);
    checkListItems(dictKeys1, Q_TYPE_SYMBOL, symbols1, 1);

    QObj *dictValues1 = qGetDictValues(dict1);
    TEST_ASSERT_EQUAL_UINT64(1, dictValues1->length);
    checkListType(dictValues1, Q_TYPE_LONG);
    checkListItems(dictValues1, Q_TYPE_LONG, longs1, 1);

    // Multiple key-value pairs
    QSymbol symbols2[] = {"abc", "def", "ghi"};
    QObj *keys2 = qNewSymbolList(symbols2, 3);
    QLong longs2[] = {10, 20, 30};
    QObj *values2 = qNewLongList(longs2, 3);
    QObj *dict2 = qNewDict(keys2, values2);
    checkIsDict(dict1);
    TEST_ASSERT_EQUAL_UINT64(3, qGetDictCount(dict2));

    QObj *dictKeys2 = qGetDictKeys(dict2);
    TEST_ASSERT_EQUAL_UINT64(3, dictKeys2->length);
    checkListType(dictKeys2, Q_TYPE_SYMBOL);
    checkListItems(dictKeys2, Q_TYPE_SYMBOL, symbols2, 3);

    QObj *dictValues2 = qGetDictValues(dict2);
    TEST_ASSERT_EQUAL_UINT64(3, dictValues2->length);
    checkListType(dictValues2, Q_TYPE_LONG);
    checkListItems(dictValues2, Q_TYPE_LONG, longs2, 3);

    // Keys and values can be any type
    QChar chars3[] = {'a', 'b', 'c'};
    QObj *keys3 = qNewCharList(chars3, 3);
    QInt ints3[] = {10, 20, 30};
    QObj *values3 = qNewIntList(ints3, 3);
    QObj *dict3 = qNewDict(keys3, values3);
    checkIsDict(dict3);
    TEST_ASSERT_EQUAL_UINT64(3, qGetDictCount(dict3));

    QObj *dictKeys3 = qGetDictKeys(dict3);
    TEST_ASSERT_EQUAL_UINT64(3, dictKeys3->length);
    checkListType(dictKeys3, Q_TYPE_CHAR);
    checkListItems(dictKeys3, Q_TYPE_CHAR, chars3, 3);

    QObj *dictValues3 = qGetDictValues(dict3);
    TEST_ASSERT_EQUAL_UINT64(3, dictValues3->length);
    checkListType(dictValues3, Q_TYPE_INT);
    checkListItems(dictValues3, Q_TYPE_INT, ints3, 3);

    // Keys and values length mismatch
    QChar chars4[] = {'a', 'b', 'c'};
    QObj *keys4 = qNewCharList(chars4, 3);
    QInt ints4[] = {10, 20};
    QObj *values4 = qNewIntList(ints4, 2);
    QObj *dict4 = qNewDict(keys4, values4);
    QObj *error4 = qCheckError(dict4);
    TEST_ASSERT_TRUE(qIsError(error4));
    TEST_ASSERT_EQUAL_STRING("length", qGetError(error4));

    QChar chars5[] = {'a', 'b'};
    QObj *keys5 = qNewCharList(chars5, 2);
    QInt ints5[] = {10, 20, 30};
    QObj *values5 = qNewIntList(ints5, 3);
    QObj *dict5 = qNewDict(keys5, values5);
    QObj *error5 = qCheckError(dict5);
    TEST_ASSERT_TRUE(qIsError(error5));
    TEST_ASSERT_EQUAL_STRING("length", qGetError(error5));

    // Keys and/or values are null
    QChar chars6[] = {'a', 'b', 'c'};
    QObj *keys6 = qNewCharList(chars6, 3);
    QObj *dict6 = qNewDict(keys6, nullptr);
    QObj *error6 = qCheckError(dict6);
    TEST_ASSERT_TRUE(qIsError(error6));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error6));

    QInt ints7[] = {10, 20, 30};
    QObj *values7 = qNewIntList(ints7, 3);
    QObj *dict7 = qNewDict(nullptr, values7);
    QObj *error7 = qCheckError(dict7);
    TEST_ASSERT_TRUE(qIsError(error7));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error7));

    QObj *dict8 = qNewDict(nullptr, nullptr);
    QObj *error8 = qCheckError(dict8);
    TEST_ASSERT_TRUE(qIsError(error8));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error8));
}

void testNewTable() {
    // Empty table
    QSymbol symbols0[] = {"colA"};
    QObj *header0 = qNewSymbolList(symbols0, 1);
    QLong longs0[] = {};
    QObj *colA0 = qNewLongList(longs0, 0);
    QObj *table0 = qNewTable(header0, colA0);
    checkIsTable(table0);
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableColumnCount(table0));
    TEST_ASSERT_EQUAL_UINT64(0, qGetTableRowCount(table0));
    checkTableHeader(table0, Q_TYPE_SYMBOL, symbols0, 1);

    // Single column, single row
    QSymbol symbols1[] = {"colA"};
    QObj *header1 = qNewSymbolList(symbols1, 1);
    QLong longs1[] = {1};
    QObj *colA1 = qNewLongList(longs1, 1);
    QObj *table1 = qNewTable(header1, colA1);
    checkIsTable(table1);
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableColumnCount(table1));
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableRowCount(table1));
    checkTableHeader(table1, Q_TYPE_SYMBOL, symbols1, 1);
    checkTableColumn(table1, Q_TYPE_LONG, longs1, 1, 0);

    // Single column, multiple rows
    QSymbol symbols2[] = {"colA"};
    QObj *header2 = qNewSymbolList(symbols2, 1);
    QLong longs2[] = {1, 2};
    QObj *colA2 = qNewLongList(longs2, 2);
    QObj *table2 = qNewTable(header2, colA2);
    checkIsTable(table2);
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableColumnCount(table2));
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableRowCount(table2));
    checkTableHeader(table2, Q_TYPE_SYMBOL, symbols2, 1);
    checkTableColumn(table2, Q_TYPE_LONG, longs2, 2, 0);

    // Multiple columns, single row
    QSymbol symbols3[] = {"colA", "colB"};
    QObj *header3 = qNewSymbolList(symbols3, 2);
    QLong longs3[] = {1};
    QObj *colA3 = qNewLongList(longs3, 1);
    QChar chars3[] = {'a'};
    QObj *colB3 = qNewCharList(chars3, 1);
    QObj *table3 = qNewTable(header3, colA3, colB3);
    checkIsTable(table3);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table3));
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableRowCount(table3));
    checkTableHeader(table3, Q_TYPE_SYMBOL, symbols3, 2);
    checkTableColumn(table3, Q_TYPE_LONG, longs3, 1, 0);
    checkTableColumn(table3, Q_TYPE_CHAR, chars3, 1, 1);

    // Multiple columns, multiple rows
    QSymbol symbols4[] = {"colA", "colB"};
    QObj *header4 = qNewSymbolList(symbols4, 2);
    QLong longs4[] = {1, 2, 3};
    QObj *colA4 = qNewLongList(longs4, 3);
    QChar chars4[] = {'a', 'b', 'c'};
    QObj *colB4 = qNewCharList(chars4, 3);
    QObj *table4 = qNewTable(header4, colA4, colB4);
    checkIsTable(table3);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table4));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table4));
    checkTableHeader(table4, Q_TYPE_SYMBOL, symbols4, 2);
    checkTableColumn(table4, Q_TYPE_LONG, longs4, 3, 0);
    checkTableColumn(table4, Q_TYPE_CHAR, chars4, 3, 1);

    // Empty header - Error
    QSymbol symbols5[] = {};
    QObj *header5 = qNewSymbolList(symbols5, 0);
    QObj *table5 = qNewTable(header5);
    QObj *error5 = qCheckError(table5);
    TEST_ASSERT_TRUE(qIsError(error5));
    TEST_ASSERT_EQUAL_STRING("rank", qGetError(error5));

    // Null header - Error
    QObj *table6 = qNewTable(nullptr);
    QObj *error6 = qCheckError(table6);
    TEST_ASSERT_TRUE(qIsError(error6));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error6));
}

void testNewTableVar() {
    // Empty table
    QSymbol symbols0[] = {};
    QObj *header = qNewSymbolList(symbols0, 0);
    QObj *table0 = newTableVar(header);
    TEST_ASSERT_NULL(table0);

    // Single column, single row
    QSymbol symbols1[] = {"colA"};
    QObj *header1 = qNewSymbolList(symbols1, 1);
    QLong longs1[] = {1};
    QObj *colA1 = qNewLongList(longs1, 1);
    QObj *table1 = newTableVar(header1, colA1);
    checkIsTable(table1);
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableColumnCount(table1));
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableRowCount(table1));
    checkTableHeader(table1, Q_TYPE_SYMBOL, symbols1, 1);
    checkTableColumn(table1, Q_TYPE_LONG, longs1, 1, 0);

    // Single column, multiple rows
    QSymbol symbols2[] = {"colA"};
    QObj *header2 = qNewSymbolList(symbols2, 1);
    QLong longs2[] = {1, 2};
    QObj *colA2 = qNewLongList(longs2, 2);
    QObj *table2 = newTableVar(header2, colA2);
    checkIsTable(table2);
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableColumnCount(table2));
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableRowCount(table2));
    checkTableHeader(table2, Q_TYPE_SYMBOL, symbols2, 1);
    checkTableColumn(table2, Q_TYPE_LONG, longs2, 2, 0);

    // Multiple columns, single row
    QSymbol symbols3[] = {"colA", "colB"};
    QObj *header3 = qNewSymbolList(symbols3, 2);
    QLong longs3[] = {1};
    QObj *colA3 = qNewLongList(longs3, 1);
    QChar chars3[] = {'a'};
    QObj *colB3 = qNewCharList(chars3, 1);
    QObj *table3 = newTableVar(header3, colA3, colB3);
    checkIsTable(table3);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table3));
    TEST_ASSERT_EQUAL_UINT64(1, qGetTableRowCount(table3));
    checkTableHeader(table3, Q_TYPE_SYMBOL, symbols3, 2);
    checkTableColumn(table3, Q_TYPE_LONG, longs3, 1, 0);
    checkTableColumn(table3, Q_TYPE_CHAR, chars3, 1, 1);

    // Multiple columns, multiple rows
    QSymbol symbols4[] = {"colA", "colB"};
    QObj *header4 = qNewSymbolList(symbols4, 2);
    QLong longs4[] = {1, 2, 3};
    QObj *colA4 = qNewLongList(longs4, 3);
    QChar chars4[] = {'a', 'b', 'c'};
    QObj *colB4 = qNewCharList(chars4, 3);
    QObj *table4 = newTableVar(header4, colA4, colB4);
    checkIsTable(table3);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table4));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table4));
    checkTableHeader(table4, Q_TYPE_SYMBOL, symbols4, 2);
    checkTableColumn(table4, Q_TYPE_LONG, longs4, 3, 0);
    checkTableColumn(table4, Q_TYPE_CHAR, chars4, 3, 1);
}

void testNewKeyedTable() {
    QSymbol keySymbols[] = {"key0", "key1"};
    QObj *keyHeader = qNewSymbolList(keySymbols, 2);
    QLong keyLongs[] = {1, 2, 3};
    QObj *key0 = qNewLongList(keyLongs, 3);
    QChar keyChars[] = {'a', 'b', 'c'};
    QObj *key1 = qNewCharList(keyChars, 3);
    QObj *keys = qNewTable(keyHeader, key0, key1);

    QSymbol valueSymbols[] = {"col0", "col1", "col2"};
    QObj *valueHeader = qNewSymbolList(valueSymbols, 3);
    QLong valueLongs[] = {10, 20, 30};
    QObj *col0 = qNewLongList(valueLongs, 3);
    QChar valueChars[] = {'x', 'y', 'z'};
    QObj *col1 = qNewCharList(valueChars, 3);
    QInt valueInts[] = {100, 200, 300};
    QObj *col2 = qNewIntList(valueInts, 3);
    QObj *values = qNewTable(valueHeader, col0, col1, col2);

    QObj *keyedTable = qNewKeyedTable(keys, values);
    checkIsKeyedTable(keyedTable);

    QObj *keyTable = qGetKeyedTableKeys(keyedTable);
    QObj *valueTable = qGetKeyedTableValues(keyedTable);

    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(keyTable));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(keyTable));
    checkTableHeader(keyTable, Q_TYPE_SYMBOL, keySymbols, 2);
    checkTableColumn(keyTable, Q_TYPE_LONG, keyLongs, 3, 0);
    checkTableColumn(keyTable, Q_TYPE_CHAR, keyChars, 3, 1);

    TEST_ASSERT_EQUAL_UINT64(3, qGetTableColumnCount(valueTable));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(valueTable));
    checkTableHeader(valueTable, Q_TYPE_SYMBOL, valueSymbols, 3);
    checkTableColumn(valueTable, Q_TYPE_LONG, valueLongs, 3, 0);
    checkTableColumn(valueTable, Q_TYPE_CHAR, valueChars, 3, 1);
    checkTableColumn(valueTable, Q_TYPE_INT, valueInts, 3, 2);

    // Keys and values length mismatch
    QLong valueLongs1[] = {10, 20};
    QObj *col10 = qNewLongList(valueLongs1, 2);
    QChar valueChars1[] = {'x', 'y'};
    QObj *col11 = qNewCharList(valueChars1, 2);
    QInt valueInts1[] = {100, 200};
    QObj *col12 = qNewIntList(valueInts1, 2);
    incRef(valueHeader); // Reused after being consumed by values (qNewTable takes ownership)
    QObj *values1 = qNewTable(valueHeader, col10, col11, col12);

    incRef(keys); // Reused after being consumed by keyedTable (qNewKeyedTable takes ownership)
    QObj *keyedTable1 = qNewKeyedTable(keys, values1);
    QObj *error0 = qCheckError(keyedTable1);
    TEST_ASSERT_TRUE(qIsError(error0));
    TEST_ASSERT_EQUAL_STRING("length", qGetError(error0));

    // Keys and values must be tables
    incRef(keys);
    incRef(valueHeader);
    QObj *keyedTable2 = qNewKeyedTable(keys, valueHeader);
    QObj *error1 = qCheckError(keyedTable2);
    TEST_ASSERT_TRUE(qIsError(error1));
    TEST_ASSERT_EQUAL_STRING("type", qGetError(error1));
}

void testKeyTable() {
    QSymbol symbols[] = {"key0", "key1", "col0", "col1", "col2"};
    QObj *header = qNewSymbolList(symbols, 5);
    QLong keyLongs[] = {1, 2, 3};
    QObj *key0 = qNewLongList(keyLongs, 3);
    QChar keyChars[] = {'a', 'b', 'c'};
    QObj *key1 = qNewCharList(keyChars, 3);
    QLong valueLongs[] = {10, 20, 30};
    QObj *col0 = qNewLongList(valueLongs, 3);
    QChar valueChars[] = {'x', 'y', 'z'};
    QObj *col1 = qNewCharList(valueChars, 3);
    QInt valueInts[] = {100, 200, 300};
    QObj *col2 = qNewIntList(valueInts, 3);
    QObj *table = qNewTable(header, key0, key1, col0, col1, col2);

    QObj *keyedTable = qKeyTable(2, table);
    checkIsKeyedTable(keyedTable);

    QObj *keys = qGetKeyedTableKeys(keyedTable);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(keys));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(keys));
    checkBorrowedTableHeader(keys, Q_TYPE_SYMBOL, symbols, 2);
    checkBorrowedTableColumn(keys, Q_TYPE_LONG, keyLongs, 3, 0);
    checkBorrowedTableColumn(keys, Q_TYPE_CHAR, keyChars, 3, 1);

    QObj *values = qGetKeyedTableValues(keyedTable);
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableColumnCount(values));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(values));
    checkBorrowedTableHeader(values, Q_TYPE_SYMBOL, symbols + 2, 3);
    checkBorrowedTableColumn(values, Q_TYPE_LONG, valueLongs, 3, 0);
    checkBorrowedTableColumn(values, Q_TYPE_CHAR, valueChars, 3, 1);
    checkBorrowedTableColumn(values, Q_TYPE_INT, valueInts, 3, 2);

    // // Too many keys
    QObj *manyKeys = qKeyTable(Q_SIZE_MAX + 1, table);
    QObj *error0 = qCheckError(manyKeys);
    TEST_ASSERT_TRUE(qIsError(error0));
    TEST_ASSERT_EQUAL_STRING("domain", qGetError(error0));

    // Second argumen must be a table
    QObj *badArg1 = qKeyTable(2, nullptr);
    QObj *error1 = qCheckError(badArg1);
    TEST_ASSERT_TRUE(qIsError(error1));
    TEST_ASSERT_EQUAL_STRING("type", qGetError(error1));

    QObj *badArg2 = qKeyTable(2, col2);
    QObj *error3 = qCheckError(badArg2);
    TEST_ASSERT_TRUE(qIsError(error3));
    TEST_ASSERT_EQUAL_STRING("type", qGetError(error3));
}

void testUnkeyTable() {
    QSymbol keySymbols[] = {"key0", "key1"};
    QObj *keyHeader = qNewSymbolList(keySymbols, 2);
    QLong keyLongs[] = {1, 2, 3};
    QObj *key0 = qNewLongList(keyLongs, 3);
    QChar keyChars[] = {'a', 'b', 'c'};
    QObj *key1 = qNewCharList(keyChars, 3);
    QObj *keys = qNewTable(keyHeader, key0, key1);

    QSymbol valueSymbols[] = {"col0", "col1", "col2"};
    QObj *valueHeader = qNewSymbolList(valueSymbols, 3);
    QLong valueLongs[] = {10, 20, 30};
    QObj *col0 = qNewLongList(valueLongs, 3);
    QChar valueChars[] = {'x', 'y', 'z'};
    QObj *col1 = qNewCharList(valueChars, 3);
    QInt valueInts[] = {100, 200, 300};
    QObj *col2 = qNewIntList(valueInts, 3);
    QObj *values = qNewTable(valueHeader, col0, col1, col2);

    QObj *keyedTable = qNewKeyedTable(keys, values);
    QObj *table = qUnkeyTable(keyedTable);
    checkIsTable(table);

    QSymbol symbols[] = {"key0", "key1", "col0", "col1", "col2"};
    TEST_ASSERT_EQUAL_UINT64(5, qGetTableColumnCount(table));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table));
    checkTableHeader(table, Q_TYPE_SYMBOL, symbols, 5);
    checkTableColumn(table, Q_TYPE_LONG, keyLongs, 3, 0);
    checkTableColumn(table, Q_TYPE_CHAR, keyChars, 3, 1);
    checkTableColumn(table, Q_TYPE_LONG, valueLongs, 3, 2);
    checkTableColumn(table, Q_TYPE_CHAR, valueChars, 3, 3);
    checkTableColumn(table, Q_TYPE_INT, valueInts, 3, 4);
}

void testNewMixedListTooLong() {
    // Length is checked before any arguments are read
    checkDomainError(qNewMixedList((QSize)INT32_MAX + 1));
    checkDomainError(newMixedListVar((QSize)INT32_MAX + 1));
}

void testNewDictErrorsRelease() {
    QLong longs[] = {1, 2, 3};

    // Atom keys - Error, both released
    QObj *atomKeys = qNewLong(1);
    QObj *values0 = qNewLongList(longs, 3);
    incRef(atomKeys);
    incRef(values0);
    checkError(qNewDict(atomKeys, values0), "type");
    checkReleased(atomKeys);
    checkReleased(values0);

    // Atom values - Error, both released
    QSymbol symbols[] = {"a", "b", "c"};
    QObj *keys1 = qNewSymbolList(symbols, 3);
    QObj *atomValues = qNewLong(1);
    incRef(keys1);
    incRef(atomValues);
    checkError(qNewDict(keys1, atomValues), "type");
    checkReleased(keys1);
    checkReleased(atomValues);

    // Length mismatch - Error, both released
    QObj *keys2 = qNewSymbolList(symbols, 3);
    QObj *values2 = qNewLongList(longs, 2);
    incRef(keys2);
    incRef(values2);
    checkError(qNewDict(keys2, values2), "length");
    checkReleased(keys2);
    checkReleased(values2);

    // Null values - Error, keys released
    QObj *keys3 = qNewSymbolList(symbols, 3);
    incRef(keys3);
    checkDomainError(qNewDict(keys3, nullptr));
    checkReleased(keys3);

    // Mixed list keys are accepted
    QObj *mixedKeys = qNewMixedList(3, qNewLong(1), qNewChar('a'), qNewSymbol("x"));
    QObj *dict = qNewDict(mixedKeys, qNewLongList(longs, 3));
    checkIsDict(dict);
    TEST_ASSERT_EQUAL_UINT64(3, qGetDictCount(dict));
    decRef(dict);
}

void testNewTableErrorsRelease() {
    QLong longs[] = {1, 2, 3};
    QChar chars[] = {'a', 'b', 'c'};

    // Header is not a symbol list - Error, header and column released
    QObj *longHeader = qNewLongList(longs, 1);
    QObj *column0 = qNewLongList(longs, 3);
    incRef(longHeader);
    incRef(column0);
    checkError(qNewTable(longHeader, column0), "type");
    checkReleased(longHeader);
    checkReleased(column0);

    // Null header - Error, columns released (the column count is known from the arguments)
    QObj *column1 = qNewLongList(longs, 3);
    QObj *column2 = qNewCharList(chars, 3);
    incRef(column1);
    incRef(column2);
    checkDomainError(qNewTable(nullptr, column1, column2));
    checkReleased(column1);
    checkReleased(column2);

    // Header is a symbol atom - Error, header released
    QObj *atomHeader = qNewSymbol("col");
    incRef(atomHeader);
    checkError(qNewTable(atomHeader), "type");
    checkReleased(atomHeader);

    // Null column - Error, header and non-null columns released
    QSymbol symbols[] = {"colA", "colB", "colC"};
    QObj *header = qNewSymbolList(symbols, 3);
    QObj *colA = qNewLongList(longs, 3);
    QObj *colC = qNewCharList(chars, 3);
    incRef(header);
    incRef(colA);
    incRef(colC);
    checkDomainError(qNewTable(header, colA, nullptr, colC));
    checkReleased(header);
    checkReleased(colA);
    checkReleased(colC);

    // Fewer columns than column names - Error, all released
    QObj *header3 = qNewSymbolList(symbols, 3);
    QObj *colA3 = qNewLongList(longs, 3);
    incRef(header3);
    incRef(colA3);
    checkError(qNewTable(header3, colA3), "length");
    checkReleased(header3);
    checkReleased(colA3);

    // More columns than column names - Error, all released
    QObj *header4 = qNewSymbolList(symbols, 1);
    QObj *colA4 = qNewLongList(longs, 3);
    QObj *colB4 = qNewCharList(chars, 3);
    incRef(header4);
    incRef(colA4);
    incRef(colB4);
    checkError(qNewTable(header4, colA4, colB4), "length");
    checkReleased(header4);
    checkReleased(colA4);
    checkReleased(colB4);

    // Same checks through the va_list variant
    QObj *headerVar = qNewSymbolList(symbols, 2);
    QObj *colVar = qNewLongList(longs, 3);
    incRef(headerVar);
    incRef(colVar);
    checkDomainError(newTableVar(headerVar, nullptr, colVar));
    checkReleased(headerVar);
    checkReleased(colVar);
}

void testNewTableFromArray() {
    QSymbol symbols[] = {"colA", "colB"};
    QLong longs[] = {1, 2, 3};
    QChar chars[] = {'a', 'b', 'c'};

    QObj *columns[] = {qNewLongList(longs, 3), qNewCharList(chars, 3)};
    QObj *table = qNewTableFromArray(qNewSymbolList(symbols, 2), columns, 2);
    checkIsTable(table);
    TEST_ASSERT_EQUAL_UINT64(2, qGetTableColumnCount(table));
    TEST_ASSERT_EQUAL_UINT64(3, qGetTableRowCount(table));
    checkTableHeader(table, Q_TYPE_SYMBOL, symbols, 2);
    checkTableColumn(table, Q_TYPE_LONG, longs, 3, 0);
    checkTableColumn(table, Q_TYPE_CHAR, chars, 3, 1);

    // Null array with a non-zero count - Error, header released
    QObj *header = qNewSymbolList(symbols, 2);
    incRef(header);
    checkDomainError(qNewTableFromArray(header, nullptr, 2));
    checkReleased(header);

    // Count does not match the header - Error, all released
    QObj *header1 = qNewSymbolList(symbols, 2);
    QObj *columns1[] = {qNewLongList(longs, 3)};
    incRef(header1);
    incRef(columns1[0]);
    checkError(qNewTableFromArray(header1, columns1, 1), "length");
    checkReleased(header1);
    checkReleased(columns1[0]);
}

void testNewKeyedTableErrorsRelease() {
    QLong longs[] = {1, 2, 3};
    QSymbol keySymbols[] = {"key"};
    QSymbol valueSymbols[] = {"value"};

    // Values are not a table - Error, both released
    QObj *keys = qNewTable(qNewSymbolList(keySymbols, 1), qNewLongList(longs, 3));
    QObj *notTable = qNewLongList(longs, 3);
    incRef(keys);
    incRef(notTable);
    checkError(qNewKeyedTable(keys, notTable), "type");
    checkReleased(keys);
    checkReleased(notTable);

    // Null keys - Error, values released
    QObj *values = qNewTable(qNewSymbolList(valueSymbols, 1), qNewLongList(longs, 3));
    incRef(values);
    checkDomainError(qNewKeyedTable(nullptr, values));
    checkReleased(values);

    // Row count mismatch - Error, both released
    QObj *keys2 = qNewTable(qNewSymbolList(keySymbols, 1), qNewLongList(longs, 3));
    QObj *values2 = qNewTable(qNewSymbolList(valueSymbols, 1), qNewLongList(longs, 2));
    incRef(keys2);
    incRef(values2);
    checkError(qNewKeyedTable(keys2, values2), "length");
    checkReleased(keys2);
    checkReleased(values2);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(testIsListKinds);
    RUN_TEST(testNewBoolean);
    RUN_TEST(testNewGuid);
    RUN_TEST(testNewByte);
    RUN_TEST(testNewShort);
    RUN_TEST(testNewInt);
    RUN_TEST(testNewLong);
    RUN_TEST(testNewReal);
    RUN_TEST(testNewFloat);
    RUN_TEST(testNewChar);
    RUN_TEST(testNewSymbol);
    RUN_TEST(testNewTimestamp);
    RUN_TEST(testNewMonth);
    RUN_TEST(testNewDate);
    RUN_TEST(testNewDatetime);
    RUN_TEST(testNewTimespan);
    RUN_TEST(testNewMinute);
    RUN_TEST(testNewSecond);
    RUN_TEST(testNewTime);

    RUN_TEST(testNewList);
    RUN_TEST(testNewBooleanList);
    RUN_TEST(testNewGuidList);
    RUN_TEST(testNewByteList);
    RUN_TEST(testNewShortList);
    RUN_TEST(testNewIntList);
    RUN_TEST(testNewLongList);
    RUN_TEST(testNewRealList);
    RUN_TEST(testNewFloatList);
    RUN_TEST(testNewCharList);
    RUN_TEST(testNewCharListFromString);
    RUN_TEST(testNewCharListFromFixedString);
    RUN_TEST(testNewSymbolList);
    RUN_TEST(testNewTimestampList);
    RUN_TEST(testNewMonthList);
    RUN_TEST(testNewDateList);
    RUN_TEST(testNewDatetimeList);
    RUN_TEST(testNewTimespanList);
    RUN_TEST(testNewMinuteList);
    RUN_TEST(testNewSecondList);
    RUN_TEST(testNewTimeList);
    RUN_TEST(testNewListNullValues);

    RUN_TEST(testNewMixedList);
    RUN_TEST(testNewMixedListVar);
    RUN_TEST(testNewDict);
    RUN_TEST(testNewTable);
    RUN_TEST(testNewTableVar);
    RUN_TEST(testNewKeyedTable);
    RUN_TEST(testKeyTable);
    RUN_TEST(testUnkeyTable);
    RUN_TEST(testNewMixedListTooLong);
    RUN_TEST(testNewDictErrorsRelease);
    RUN_TEST(testNewTableErrorsRelease);
    RUN_TEST(testNewTableFromArray);
    RUN_TEST(testNewKeyedTableErrorsRelease);

    return UNITY_END();
}
