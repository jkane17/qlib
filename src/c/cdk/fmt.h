
/**
 * @file fmt.h
 * @brief Display formatting of Q values.
 *
 * For supported primitive atoms and lists, ToStr follows Q string and ToLiteral
 * follows .Q.s1, with the documented exceptions. List strings deliberately join
 * element strings with spaces; temporal fields expand rather than use Q placeholders.
 * Literal lists retain empty/singleton shape. Dictionaries and tables are unsupported.
 * Neither form is a serialization API: symbols are not escaped, display precision
 * can round away information, and output need not be a parseable Q expression.
 * Numeric output follows the current locale and floating-point rounding mode.
 * See doc/c/formatting.md for examples and compatibility checks.
 *
 * Numeric and temporal special values follow Q's string (ToStr) and .Q.s1
 * (ToLiteral): null strings are empty, infinities use 0W/-0W or 0w/-0w, and
 * literals carry the appropriate type markers. Any floating NaN is treated as
 * null. List strings join element strings with spaces, retaining empty null
 * fields; literal lists use shared type markers. See the documentation for the
 * full token table and remaining differences from Q.
 * Special values are handled before calendar conversion or arithmetic.
 *
 * Buffer sizes are byte capacities, including room for the final null terminator.
 * Successful output counts exclude that terminator and cannot exceed INT_MAX.
 * Successful atoms and all literal lists write a final terminator. String lists
 * write a terminator unless empty, in which case they write nothing. Empty symbols
 * and numeric/temporal nulls formatted with ToStr return zero but write a terminator.
 * Embedded null character data counts toward the result,
 * which can therefore exceed strlen.
 *
 * Insufficient capacity returns Q_FMT_ERROR_SIZE, never successful truncation. There
 * is no sizing-query mode. After an error, output may be partial and need not be
 * null-terminated; do not read it as a string. List accumulation beyond INT_MAX
 * returns Q_FMT_ERROR_RANGE. Individual functions may specify stronger guarantees.
 *
 * Real/float precision follows Q's display precision: 1 through 17 significant
 * digits, or zero for maximum display precision (8 digits for real, 17 for float).
 * Q_FMT_DEFAULT_PRECISION is 7. Trailing zeros are suppressed and scientific
 * notation is used for small/large values. Real subnormals display as signed zero;
 * float subnormals retain their value. Precision is explicit: these functions
 * do not query a Q runtime. Invalid precision returns Q_FMT_ERROR_RANGE before
 * formatting; empty lists ignore precision in both output forms.
 *
 * Float literals append f when the rounded display text otherwise looks integral.
 * Float literal lists append one f only if every displayed item looks integral;
 * decimal, exponent, null, and infinity tokens identify the type without a suffix.
 *
 * Character literals escape quotes, backslashes, and non-printable bytes as Q
 * does: named escapes for tab/newline/carriage return, three-digit octal otherwise.
 * Escaping is byte-based and independent of locale. A character atom literal needs
 * up to 7 bytes including its terminator; a nonempty character-list literal needs
 * at most 4 * length + 3 bytes, plus one for a singleton comma. Check sizing
 * arithmetic for overflow.
 * Character ToStr functions retain raw bytes, including embedded zero bytes.
 *
 * Temporal widths can grow with the year, day count, hour count, or sign. Allow one
 * additional byte beyond the formatted width for the terminator; see the width
 * table in doc/c/formatting.md for maximum atom widths.
 *
 * Required null input pointers return Q_FMT_ERROR_ARGUMENT. Nonempty lists require
 * a non-null list pointer; symbol elements must be non-null, terminated strings.
 * Empty lists accept null input pointers. Empty string lists return zero without
 * writing, even with a null output pointer. Empty literal lists write Q's typed
 * empty representation (or two quotes for chars) and require output capacity.
 * Singleton literal lists start with a comma before any type prefix or quote.
 * A null output buffer otherwise returns Q_FMT_ERROR_SIZE for valid inputs.
 * Callers must supply valid, correctly aligned input storage of the declared type
 * and length, and writable output storage of at least size bytes. Input and output
 * must not overlap. Dangling pointers, insufficient storage, and misalignment are
 * caller errors and are not detected.
 */

#ifndef QLIB_FMT_H
#define QLIB_FMT_H

#include "type.h"

// Q display precision: zero selects maximum display precision, otherwise 1..17.
#define Q_FMT_MAX_PRECISION 17
#define Q_FMT_DEFAULT_PRECISION 7

typedef enum : signed int {
    Q_FMT_ERROR_ARGUMENT = -5, // Required input pointer is null
    Q_FMT_ERROR_RANGE = -4, // Value, precision, or output count out of range; calendar conversion failed
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
 * @brief Format a boolean value into Q-style text for display.
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
 * @param guid A non-null pointer to the guid value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qGuidToStr(char *buffer, QSize size, const QGuid *guid);

/**
 * @brief Format a guid value into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param guid A non-null pointer to the guid value to format.
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
 * @brief Format a byte value into Q-style text for display.
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
 * @brief Format a short value into Q-style text for display.
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
 * @brief Format a int value into Q-style text for display.
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
 * @brief Format a long value into Q-style text for display.
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
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealToStr(char *buffer, QSize size, QReal real, int precision);

/**
 * @brief Format a real value into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param real The real value to format.
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
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
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatToStr(char *buffer, QSize size, QFloat float_, int precision);

/**
 * @brief Format a float value into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param float_ The float value to format.
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
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
 * @brief Format a char value into Q-style text for display.
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
 * @param symbol A non-null, null-terminated symbol string (an empty string is valid).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolToStr(char *buffer, QSize size, QSymbol symbol);

/**
 * @brief Format a symbol value into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param symbol A non-null, null-terminated symbol string (an empty string is valid).
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qSymbolToLiteral(char *buffer, QSize size, QSymbol symbol);

/**
 * @brief Format a timestamp value into string form.
 *
 * @note Uses UTC. Returns Q_FMT_ERROR_RANGE without writing output if the platform
 * cannot convert the value to calendar time.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param timestamp The timestamp value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qTimestampToStr(char *buffer, QSize size, QTimestamp timestamp);

/**
 * @brief Format a timestamp value into Q-style text for display.
 *
 * @note Uses UTC. Returns Q_FMT_ERROR_RANGE without writing output if the platform
 * cannot convert the value to calendar time.
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
 * @brief Format a month value into Q-style text for display.
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
 * @note Uses UTC. Returns Q_FMT_ERROR_RANGE without writing output if the platform
 * cannot convert the value to calendar time.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param date The date value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDateToStr(char *buffer, QSize size, QDate date);

/**
 * @brief Format a date value into Q-style text for display.
 *
 * @note Uses UTC. Returns Q_FMT_ERROR_RANGE without writing output if the platform
 * cannot convert the value to calendar time.
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
 * @note Uses the proleptic Gregorian calendar with years INT32_MIN through INT32_MAX.
 * Matches Q by rounding epoch-relative microseconds (ties away from the epoch),
 * then discarding sub-millisecond time within the resulting day. Extended dates
 * outside the int64 microsecond range split the day first to avoid overflow.
 * Finite out-of-range values return Q_FMT_ERROR_RANGE without writing output.
 * Nulls and infinities use Q special-value text instead of calendar conversion.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param datetime The datetime value to format.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qDatetimeToStr(char *buffer, QSize size, QDatetime datetime);

/**
 * @brief Format a datetime value into Q-style text for display.
 *
 * @note Uses the proleptic Gregorian calendar with years INT32_MIN through INT32_MAX.
 * Matches Q by rounding epoch-relative microseconds (ties away from the epoch),
 * then discarding sub-millisecond time within the resulting day. Extended dates
 * outside the int64 microsecond range split the day first to avoid overflow.
 * Finite out-of-range values return Q_FMT_ERROR_RANGE without writing output.
 * Nulls and infinities use Q special-value text instead of calendar conversion.
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
 * @brief Format a timespan value into Q-style text for display.
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
 * @brief Format a minute value into Q-style text for display.
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
 * @brief Format a second value into Q-style text for display.
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
 * @brief Format a time value into Q-style text for display.
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
 * @param atom Address of the atom value, except symbols: pass the string pointer itself
 * (symbol), not its address (&symbol). Must not be null.
 * @param type Atom type code.
 * @param precision Q display precision (0 for maximum, otherwise 1..17), for real/float
 * values; ignored for other types and empty lists.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qAtomToStr(char *buffer, QSize size, const void *atom, QTypeCode type, int precision);

/**
 * @brief Format any atom value into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param atom Address of the atom value, except symbols: pass the string pointer itself
 * (symbol), not its address (&symbol). Must not be null.
 * @param type Atom type code.
 * @param precision Q display precision (0 for maximum, otherwise 1..17), for real/float
 * values; ignored for other types and empty lists.
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
 * @brief Format a list of boolean values into Q-style text for display.
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
 * @brief Format a list of guid values into Q-style text for display.
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
 * @brief Format a list of byte values into Q-style text for display.
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
 * @brief Format a list of short values into Q-style text for display.
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
 * @brief Format a list of int values into Q-style text for display.
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
 * @brief Format a list of long values into Q-style text for display.
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
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qRealListToStr(char *buffer, QSize size, const QReal *reals, QSize length, int precision);

/**
 * @brief Format a list of real values into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param reals The list of real values to format.
 * @param length Length of the real list.
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
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
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
 * @return The number of characters written to the buffer (not including the terminating null 
 * character), or a negative value if an error occurred (see QFmtError).
 */
int qFloatListToStr(char *buffer, QSize size, const QFloat *floats, QSize length, int precision);

/**
 * @brief Format a list of float values into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted values into.
 * @param size Size of buffer (number of bytes).
 * @param floats The list of float values to format.
 * @param length Length of the float list.
 * @param precision Q display precision: 0 for maximum, or 1 through Q_FMT_MAX_PRECISION.
 * Out-of-range precision returns Q_FMT_ERROR_RANGE.
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
 * @brief Format a list of char values into Q-style text for display.
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
 * @brief Format a list of symbol values into Q-style text for display.
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
 * @brief Format a list of timestamp values into Q-style text for display.
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
 * @brief Format a list of month values into Q-style text for display.
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
 * @brief Format a list of date values into Q-style text for display.
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
 * @brief Format a list of datetime values into Q-style text for display.
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
 * @brief Format a list of timespan values into Q-style text for display.
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
 * @brief Format a list of minute values into Q-style text for display.
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
 * @brief Format a list of second values into Q-style text for display.
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
 * @brief Format a list of time values into Q-style text for display.
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
 * @param precision Q display precision (0 for maximum, otherwise 1..17), for real/float
 * values; ignored for other types and empty lists.
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
 * @brief Format any simple list into Q-style text for display.
 * 
 * @param buffer Buffer to write the formatted value into.
 * @param size Size of buffer (number of bytes).
 * @param list The list to format.
 * @param length Length of the list.
 * @param type List type code.
 * @param precision Q display precision (0 for maximum, otherwise 1..17), for real/float
 * values; ignored for other types and empty lists.
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
