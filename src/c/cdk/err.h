
/**
 * @file err.h
 * @brief Q error object creation and handling.
 */

#ifndef QLIB_ERR_H
#define QLIB_ERR_H

#include "obj.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if the underlying value of a Q object is an error.
 *
 * @param obj A pointer to a Q object to check.
 * @return true if the underlying value is an error, false otherwise.
 */
static inline bool qIsError(const QObj *obj) {
    return obj ? obj->type == Q_TYPE_ERROR : false;
}

/**
 * @brief Extract the underlying error message from a QObj containing an error.
 *
 * @param obj A pointer to a Q object to extract from.
 * @return The underlying error message, or an empty string if obj is not an error.
 */
static inline const QChar *qGetError(const QObj *obj) {
    return qIsError(obj) ? obj->qsymbol : "";
}

/**
 * @brief Set the global error indicator message and return a null pointer (indicates an error).
 *
 * @param error Error message.
 * @return A null pointer (instructs Q to signal an error using the global error indicator).
 */
static inline QObj *qNewError(const QChar *error) {
    extern QObj *krr(const QChar *);
    return krr(error);
}

/**
 * @brief Set the system error indicator message and return a null pointer (indicates an error).
 *
 * @param error Error message.
 * @return A null pointer (instructs Q to signal an error using the system error indicator).
 */
static inline QObj *qNewOSError(const QChar *error) {
    extern QObj *orr(const QChar *);
    return orr(error);
}

/**
 * @brief Check if the given object is an error (null pointer). If so, extract the error and return
 * it. Otherwise, return the object as is.
 *
 * @note This function clears the global/system error indicator.
 *
 * @param obj A pointer to a Q object to be checked.
 * @return A pointer to a Q object containing the error if the input was an error, otherwise the
 * input object.
 */
static inline QObj *qCheckError(QObj *obj) {
    extern QObj *ee(QObj *);
    return ee(obj);
}

#ifdef __cplusplus
}
#endif

#endif // QLIB_ERR_H
