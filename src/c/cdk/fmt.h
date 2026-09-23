
/**
 * @file fmt.h
 * @brief String formatting.
 */

#ifndef QLIB_FMT_H
#define QLIB_FMT_H

#include "type.h"

typedef enum : signed int {
    Q_FMT_ERROR_TYPE = -3,   // Invalid/unsupported type
    Q_FMT_ERROR_SIZE = -2,   // Buffer not large enough
    Q_FMT_ERROR_PRINTF = -1, // printf (family) function had an error
} QFmtError;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Format a boolean value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param boolean The boolean value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qBooleanToStr(char *buffer, QSize size, QBoolean boolean);

/**
 * @brief Format a boolean value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param boolean The boolean value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qBooleanToLiteral(char *buffer, QSize size, QBoolean boolean);

/**
 * @brief Format a guid value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param guid A pointer to the guid value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qGuidToStr(char *buffer, QSize size, const QGuid *guid);

/**
 * @brief Format a guid value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param guid A pointer to the guid value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qGuidToLiteral(char *buffer, QSize size, const QGuid *guid);

/**
 * @brief Format a byte value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param byte The byte value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qByteToStr(char *buffer, QSize size, QByte byte);

/**
 * @brief Format a byte value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param byte The byte value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qByteToLiteral(char *buffer, QSize size, QByte byte);

/**
 * @brief Format a short value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param short_ The short value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qShortToStr(char *buffer, QSize size, QShort short_);

/**
 * @brief Format a short value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param short_ The short value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qShortToLiteral(char *buffer, QSize size, QShort short_);

/**
 * @brief Format a int value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param int_ The int value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qIntToStr(char *buffer, QSize size, QInt int_);

/**
 * @brief Format a int value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param int_ The int value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qIntToLiteral(char *buffer, QSize size, QInt int_);

/**
 * @brief Format a long value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param long_ The long value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qLongToStr(char *buffer, QSize size, QLong long_);

/**
 * @brief Format a long value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param long_ The long value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qLongToLiteral(char *buffer, QSize size, QLong long_);

/**
 * @brief Format a real value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param real The real value to format.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealToStr(char *buffer, QSize size, QReal real, int precision);

/**
 * @brief Format a real value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param real The real value to format.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealToLiteral(char *buffer, QSize size, QReal real, int precision);

/**
 * @brief Format a float value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param float_ The float value to format.
 * @param precision Number of decimal places shown (only first 15 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision);

/**
 * @brief Format a float value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param float_ The float value to format.
 * @param precision Number of decimal places shown (only first 15 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatToLiteral(char *buffer, QSize size, QFloat float_, int precision);

/**
 * @brief Format a char value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param char_ The char value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qCharToStr(char *buffer, QSize size, QChar char_);

/**
 * @brief Format a char value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param char_ The char value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qCharToLiteral(char *buffer, QSize size, QChar char_);

/**
 * @brief Format a symbol value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param symbol The symbol value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolToStr(char *buffer, QSize size, QSymbol symbol);

/**
 * @brief Format a symbol value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param symbol The symbol value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolToLiteral(char *buffer, QSize size, QSymbol symbol);

/**
 * @brief Format a timestamp value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param timestamp The timestamp value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimestampToStr(char *buffer, QSize size, QTimestamp timestamp);

/**
 * @brief Format a timestamp value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param timestamp The timestamp value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimestampToLiteral(char *buffer, QSize size, QTimestamp timestamp);

/**
 * @brief Format a month value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param month The month value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMonthToStr(char *buffer, QSize size, QMonth month);

/**
 * @brief Format a month value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param month The month value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMonthToLiteral(char *buffer, QSize size, QMonth month);

/**
 * @brief Format a date value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param date The date value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDateToStr(char *buffer, QSize size, QDate date);

/**
 * @brief Format a date value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param date The date value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDateToLiteral(char *buffer, QSize size, QDate date);

/**
 * @brief Format a datetime value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param datetime The datetime value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDatetimeToStr(char *buffer, QSize size, QDatetime datetime);

/**
 * @brief Format a datetime value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param datetime The datetime value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDatetimeToLiteral(char *buffer, QSize size, QDatetime datetime);

/**
 * @brief Format a timespan value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param timespan The timespan value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimespanToStr(char *buffer, QSize size, QTimespan timespan);

/**
 * @brief Format a timespan value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param timespan The timespan value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimespanToLiteral(char *buffer, QSize size, QTimespan timespan);

/**
 * @brief Format a minute value into string form.
 * 
 * @note Q itself will display any minute with an underlying int value above/below +/-5999 with '**' 
 * in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param minute The minute value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMinuteToStr(char *buffer, QSize size, QMinute minute);

/**
 * @brief Format a minute value into literal form.
 * 
 * @note Q itself will display any minute with an underlying int value above/below +/-5999 with '**' 
 * in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param minute The minute value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMinuteToLiteral(char *buffer, QSize size, QMinute minute);

/**
 * @brief Format a second value into string form.
 * 
 * @note Q itself will display any second with an underlying int value above/below +/-359,999 with 
 * '**' in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param second The second value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSecondToStr(char *buffer, QSize size, QSecond second);

/**
 * @brief Format a second value into literal form.
 * 
 * @note Q itself will display any second with an underlying int value above/below +/-359,999 with 
 * '**' in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param second The second value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSecondToLiteral(char *buffer, QSize size, QSecond second);

/**
 * @brief Format a time value into string form.
 * 
 * @note Q itself will display any time with an underlying int value above/below +/-359,999,999 with 
 * '**' in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param time The time value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimeToStr(char *buffer, QSize size, QTime time);

/**
 * @brief Format a time value into literal form.
 * 
 * @note Q itself will display any time with an underlying int value above/below +/-359,999,999 with 
 * '**' in the hour position (i.e., it does not show the hour above 99). However, this function will 
 * correctly display the hour.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param time The time value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimeToLiteral(char *buffer, QSize size, QTime time);

/**
 * @brief Format any atom value into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param atom The atom value to format.
 * @param type Atom type code.
 * @param precision Number of decimal places shown for real and float values.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qAtomToStr(char *buffer, QSize size, const void *atom, QTypeCode type, int precision);

/**
 * @brief Format any atom value into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param atom The atom value to format.
 * @param type Atom type code.
 * @param precision Number of decimal places shown for real and float values.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qAtomToLiteral(char *buffer, QSize size, const void *atom, QTypeCode type, int precision);


/**
 * @brief Format a list of boolean values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param booleans The list of boolean values to format.
 * @param length Length of the boolean list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qBooleanListToStr(char *buffer, QSize size, const QBoolean *booleans, QSize length);

/**
 * @brief Format a list of boolean values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param booleans The list of boolean values to format.
 * @param length Length of the boolean list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qBooleanListToLiteral(char *buffer, QSize size, const QBoolean *booleans, QSize length);

/**
 * @brief Format a list of guid values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param guids The list of guid values to format.
 * @param length Length of the guid list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qGuidListToStr(char *buffer, QSize size, const QGuid *guids, QSize length);

/**
 * @brief Format a list of guid values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param guids The list of guid values to format.
 * @param length Length of the guid list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qGuidListToLiteral(char *buffer, QSize size, const QGuid *guids, QSize length);

/**
 * @brief Format a list of byte values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param bytes The list of byte values to format.
 * @param length Length of the byte list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qByteListToStr(char *buffer, QSize size, const QByte *bytes, QSize length);

/**
 * @brief Format a list of byte values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param bytes The list of byte values to format.
 * @param length Length of the byte list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qByteListToLiteral(char *buffer, QSize size, const QByte *bytes, QSize length);

/**
 * @brief Format a list of short values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param shorts The list of short values to format.
 * @param length Length of the short list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qShortListToStr(char *buffer, QSize size, const QShort *shorts, QSize length);

/**
 * @brief Format a list of short values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param shorts The list of short values to format.
 * @param length Length of the short list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qShortListToLiteral(char *buffer, QSize size, const QShort *shorts, QSize length);

/**
 * @brief Format a list of int values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param ints The list of int values to format.
 * @param length Length of the int list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qIntListToStr(char *buffer, QSize size, const QInt *ints, QSize length);

/**
 * @brief Format a list of int values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param ints The list of int values to format.
 * @param length Length of the int list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qIntListToLiteral(char *buffer, QSize size, const QInt *ints, QSize length);

/**
 * @brief Format a list of long values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param longs The list of long values to format.
 * @param length Length of the long list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qLongListToStr(char *buffer, QSize size, const QLong *longs, QSize length);

/**
 * @brief Format a list of long values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param longs The list of long values to format.
 * @param length Length of the long list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qLongListToLiteral(char *buffer, QSize size, const QLong *longs, QSize length);

/**
 * @brief Format a list of real values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param reals The list of real values to format.
 * @param length Length of the real list.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealListToStr(char *buffer, QSize size, const QReal *reals, QSize length, int precision);

/**
 * @brief Format a list of real values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param reals The list of real values to format.
 * @param length Length of the real list.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealListToLiteral(char *buffer, QSize size, const QReal *reals, QSize length, int precision);

/**
 * @brief Format a list of float values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param floats The list of float values to format.
 * @param length Length of the float list.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatListToStr(char *buffer, QSize size, const QFloat *floats, QSize length, int precision);

/**
 * @brief Format a list of float values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param floats The list of float values to format.
 * @param length Length of the float list.
 * @param precision Number of decimal places shown (only first 6 are guaranteed to be accurate).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatListToLiteral(
    char *buffer, 
    QSize size, 
    const QFloat *floats, 
    QSize length, 
    int precision
);

/**
 * @brief Format a list of char values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param chars The list of char values to format.
 * @param length Length of the char list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qCharListToStr(char *buffer, QSize size, const QChar *chars, QSize length);

/**
 * @brief Format a list of char values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param chars The list of char values to format.
 * @param length Length of the char list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qCharListToLiteral(char *buffer, QSize size, const QChar *chars, QSize length);

/**
 * @brief Format a list of symbol values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param symbols The list of symbol values to format.
 * @param length Length of the symbol list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolListToStr(char *buffer, QSize size, const QSymbol *symbols, QSize length);

/**
 * @brief Format a list of symbol values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param symbols The list of symbol values to format.
 * @param length Length of the symbol list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolListToLiteral(char *buffer, QSize size, const QSymbol *symbols, QSize length);

/**
 * @brief Format a list of timestamp values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param timestamps The list of timestamp values to format.
 * @param length Length of the timestamp list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimestampListToStr(char *buffer, QSize size, const QTimestamp *timestamps, QSize length);

/**
 * @brief Format a list of timestamp values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param timestamps The list of timestamp values to format.
 * @param length Length of the timestamp list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimestampListToLiteral(char *buffer, QSize size, const QTimestamp *timestamps, QSize length);

/**
 * @brief Format a list of month values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param months The list of month values to format.
 * @param length Length of the month list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMonthListToStr(char *buffer, QSize size, const QMonth *months, QSize length);

/**
 * @brief Format a list of month values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param months The list of month values to format.
 * @param length Length of the month list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMonthListToLiteral(char *buffer, QSize size, const QMonth *months, QSize length);

/**
 * @brief Format a list of date values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param dates The list of date values to format.
 * @param length Length of the date list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDateListToStr(char *buffer, QSize size, const QDate *dates, QSize length);

/**
 * @brief Format a list of date values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param dates The list of date values to format.
 * @param length Length of the date list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDateListToLiteral(char *buffer, QSize size, const QDate *dates, QSize length);

/**
 * @brief Format a list of datetime values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param datetimes The list of datetime values to format.
 * @param length Length of the datetime list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDatetimeListToStr(char *buffer, QSize size, const QDatetime *datetimes, QSize length);

/**
 * @brief Format a list of datetime values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param datetimes The list of datetime values to format.
 * @param length Length of the datetime list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDatetimeListToLiteral(char *buffer, QSize size, const QDatetime *datetimes, QSize length);

/**
 * @brief Format a list of timespan values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param timespans The list of timespan values to format.
 * @param length Length of the timespan list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimespanListToStr(char *buffer, QSize size, const QTimespan *timespans, QSize length);

/**
 * @brief Format a list of timespan values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param timespans The list of timespan values to format.
 * @param length Length of the timespan list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimespanListToLiteral(char *buffer, QSize size, const QTimespan *timespans, QSize length);

/**
 * @brief Format a list of minute values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param minutes The list of minute values to format.
 * @param length Length of the minute list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMinuteListToStr(char *buffer, QSize size, const QMinute *minutes, QSize length);

/**
 * @brief Format a list of minute values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param minutes The list of minute values to format.
 * @param length Length of the minute list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qMinuteListToLiteral(char *buffer, QSize size, const QMinute *minutes, QSize length);

/**
 * @brief Format a list of second values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param seconds The list of second values to format.
 * @param length Length of the second list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSecondListToStr(char *buffer, QSize size, const QSecond *seconds, QSize length);

/**
 * @brief Format a list of second values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param seconds The list of second values to format.
 * @param length Length of the second list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSecondListToLiteral(char *buffer, QSize size, const QSecond *seconds, QSize length);

/**
 * @brief Format a list of time values into string form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param times The list of time values to format.
 * @param length Length of the time list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimeListToStr(char *buffer, QSize size, const QTime *time, QSize length);

/**
 * @brief Format a list of time values into literal form.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param times The list of time values to format.
 * @param length Length of the time list.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimeListToLiteral(char *buffer, QSize size, const QTime *time, QSize length);

/**
 * @brief Format any simple list into string form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param list The list to format.
 * @param length Length of the list.
 * @param type List type code.
 * @param precision Number of decimal places shown for real and float values.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qListToStr(
    char *buffer,
    QSize size,
    const void *list,
    QSize length,
    QTypeCode type,
    int precision
);

/**
 * @brief Format any simple list into literal form.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param list The list to format.
 * @param length Length of the list.
 * @param type List type code.
 * @param precision Number of decimal places shown for real and float values.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qListToLiteral(
    char *buffer,
    QSize size,
    const void *list,
    QSize length,
    QTypeCode type,
    int precision
);

#ifdef __cplusplus
}
#endif

#endif // QLIB_FMT_H
