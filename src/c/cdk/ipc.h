
/**
 * @file ipc.h
 * @brief Inter-process communication.
 */

#ifndef QLIB_IPC_H
#define QLIB_IPC_H

// THIS FILE IS A WORK IN PROGRESS

// /**
//  * @brief Remove the callback on `handle` and call `kclose` (only call from main thread).
//  *
//  * @param handle Q connection handle.
//  */
// extern V sd0(I handle);

// /**
//  * @brief Remove the callback on `handle` and call `kclose` if `condition` is 1 (only call from
//  * main thread).
//  *
//  * @param handle Q connection handle.
//  * @param condition 1 to remove callback and close.
//  */
// extern V sd0x(I handle, I condition);

// /**
//  * @brief Put the function `K func(I handle){…}` on the Q main event loop given a connection
//  * handle (only call from main thread).
//  *
//  * @param handle Q connection handle. Negative for non-blocking (async).
//  * @param func Function to put on Q main event loop. Should return NULL or a K object (ref count
//  * will be decremented).
//  * @return Connection handle if successful, NULL otherwise (handle closed).
//  */
// extern K sd1(I handle, K(*func)(I));

// /**
//  * @brief Close a Q connection and free any associated resources.
//  *
//  * @param handle Q connection handle.
//  */
// extern V kclose(I handle);

// /**
//  * @brief Open a network socket connection to a listening Q process.
//  *
//  * @param host Hostname of the listening Q process.
//  * @param port Port of the listening Q process.
//  * @return Connection handle (OS file descriptor).
//  *
//  * @example
//  * I handle = khp("localhost", 12345);
//  */
// extern I khp(const S host, I port);

// /**
//  * @brief Open a network socket connection to a listening Q process.
//  *
//  * @param host Hostname of the listening Q process.
//  * @param port Port of the listening Q process.
//  * @param credentials Username and password credentials required for authentication (separated by
//  * a colon).
//  * @return Connection handle (OS file descriptor).
//  *
//  * @example
//  * I handle = khpu("localhost", 12345, "user:password");
//  */
// extern I khpu(const S host, I port, const S credentials);

// /**
//  * @brief Open a network socket connection to a listening Q process.
//  * Retry until timeout.
//  *
//  * @param host Hostname of the listening Q process.
//  * @param port Port of the listening Q process.
//  * @param credentials Username and password credentials required for authentication (separated by
//  * a colon).
//  * @param timeout Timeout in milliseconds.
//  * @return Connection handle (OS file descriptor) or error code:
//  *      0   Authentication error
//  *      -1  Connection error
//  *      -2  Timeout error
//  *
//  * @example
//  * I handle = khpun("localhost", 12345, "user:password", 5000);
//  */
// extern I khpun(const S host, I port, const S credentials, I timeout);

// /**
//  * @brief Open a network socket connection to a listening Q process.
//  * Retry until timeout and used encryption capability.
//  *
//  * @param host Hostname of the listening Q process.
//  * @param port Port of the listening Q process.
//  * @param credentials Username and password credentials required for authentication (separated by
//  * a colon).
//  * @param timeout Timeout in milliseconds.
//  * @param capability Encryption capability:
//  *      1    1 TB limit
//  *      2    use TLS
//  * @return Connection handle (OS file descriptor) or error code:
//  *      0   Authentication error
//  *      -1  Connection error
//  *      -2  Timeout error
//  *      -3  OpenSSL initialisation failed
//  *
//  * @example
//  * I handle = khpunc("localhost", 12345, "user:password", 5000, 2);
//  */
// extern I khpunc(const S host, I port, const S credentials, I timeout, I capability);

// /**
//  * @brief Verify the given byte vector is a valid IPC message.
//  *
//  * @param msg Message to verify.
//  * @return object Non-zero if valid, otherwise 0.
//  */
// extern I okx(K msg);

// /**
//  * @brief Execute a q query over an IPC connection.
//  *
//  * @param handle Q connection handle (>0 synchronous, <0 asynchronous).
//  * @param query Query to be executed by the receiving Q process.
//  * @param ... Arguments to the `query` function (must be terminated with the sentinel `(K)0`).
//  * @return Query return value.
//  *
//  * @example
//  * K k(handle, "query", (K)0);
//  * K k(handle, "query", arg1, ..., argN, (K)0);
//  */
// extern K k(I handle, const S query, ...);

// /**
//  * @brief Execute a q query over an IPC connection (variable arguments form).
//  *
//  * @param handle Q connection handle (>0 synchronous, <0 asynchronous).
//  * @param query Query to be executed by the receiving Q process.
//  * @param args Arguments to the `query` function (must be terminated with the sentinel `(K)0`).
//  * @return Query return value.
//  *
//  * @example
//  * K vak(handle, "query", args);
//  */
// extern K vak(I handle, const S query, va_list args);

#endif // QLIB_IPC_H
