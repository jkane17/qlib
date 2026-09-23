
/**
 * @file k.h
 * @brief Provides the interface for interacting with the KDB+ runtime from C or C++.
 * 
 * @details Major functionality:
 *     - creation of q objects
 *     - IPC communication with q processes
 *     - memory management via reference counting
 *     - construction of lists, dictionaries and tables
 */

#ifndef QLIB_K_H
#define QLIB_K_H

// Representation of basic Q types as primitive C types
typedef char *S, C;
typedef unsigned char G;
typedef short H;
typedef int I;
typedef long long J;
typedef float E;
typedef double F;
typedef void V;
typedef unsigned long long UJ;

/*
    ##### K OBJECT REPRESENTATION #####
    
    All q objects are represented as pointers to the k0 structure.
    The structure stores:
    
    metadata:
        m,a  internal memory management flags (do not modify)
        t    type code (negative = atom, positive = list)
        u    attribute flags (sorted, unique, etc)
        r    reference count
    
    data:
        union containing either:
            - atom value
            - pointer to nested objects
            - list length and data (list data immediately follows the struct header in memory)
*/
typedef struct k0 {
    signed char m, a, t; // internal memory flags and type code
    C u;                 // attribute flags (sorted, unique, grouped, parted)
    I r;                 // reference count
    union {
        G g;             // boolean, byte, char
        H h;             // short
        I i;             // int, month, date, minute, second, time
        J j;             // long, timestamp, timespan
        E e;             // real
        F f;             // float, datetime
        S s;             // symbol
        struct k0 *k;    // dict, table, keyed table
        struct {
            J n;         // number of elements in list
            G G0[];      // start of contiguous list data
        };
    };
} *K;

/*
    ##### GUID TYPE #####

    q GUID values are stored as 16 raw bytes.
    The U struct provides a convenient wrapper for those bytes.
    
    GUID lists are internally stored as byte arrays and accessed via kU().
*/
typedef struct { G g[16]; } U; // 16-byte array
#define kU(x) ((U*)kG(x))      // access GUID list elements
#define xU ((U*)xG)            // internal macro variant

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a Q guid atom.
 * 
 * @param x Value to cast.
 * @return GUID as K object.
 */
extern K ku(U);

/**
 * @brief Create a Q list.
 * 
 * @param type Type number (0 for general, otherwise simple).
 * @param length Length of list.
 * @return List as K object.
 */
extern K ktn(I type, J length);

/**
 * @brief Create a char array from a string of length n.
 * 
 * @param str String to convert to char array.
 * @param length Length of string.
 * @return Char array as a K object.
 */
extern K kpn(S str, J length);

/**
 * @brief Create a keyed table from an existing table.
 *
 * Converts the first `nkeys` columns of `table` into the key columns of a
 * keyed table. The remaining columns become the value columns.
 *
 * @param nkeys Number of leading columns to use as keys.
 * @param table Table to convert.
 * @return Keyed table on success, or NULL if `table` is not a valid table.
 */
extern K knt(J nkeys, K table);

/**
 * @brief Toggle symbol table lock for string interning.
 * 
 * When interning symbols KDB+ can optionally use a lock to make symbol insertion thread-safe.
 * 
 * @param mode 1 to enable the lock, 0 to disable.
 * @return Previous lock mode (1 = locked, 0 = unlocked).
 */
extern I setm(I mode);

/**
 * @brief KDB+ release date.
 *
 * @return Integer as yyyymmdd.
 */
extern I ver();

// Convenient do loop macro
#define DO(n, x) {             \
    J i = 0, _i = (n);         \
    for (; i < _i; ++i) { x; } \
}

#ifdef __cplusplus
}
#endif

/*
    ##### VECTOR ACCESSORS #####

    Usage Example : kJ(x)[i]
*/
#define kG(x) ((x)->G0)
#define kC(x) kG(x)
#define kH(x) ((H*)kG(x))
#define kI(x) ((I*)kG(x))
#define kJ(x) ((J*)kG(x)) 
#define kE(x) ((E*)kG(x))
#define kF(x) ((F*)kG(x))
#define kS(x) ((S*)kG(x))
#define kK(x) ((K*)kG(x))

/*
    ##### ENCODED TYPE NAMES #####

        type     bytes qtype     ctype  accessor
*/
#define KB 1  // 1     boolean   char   kG
#define UU 2  // 16    guid      U      kU
#define KG 4  // 1     byte      char   kG
#define KH 5  // 2     short     short  kH
#define KI 6  // 4     int       int    kI
#define KJ 7  // 8     long      long   kJ
#define KE 8  // 4     real      float  kE
#define KF 9  // 8     float     double kF
#define KC 10 // 1     char      char   kC
#define KS 11 // *     symbol    char*  kS
#define KP 12 // 8     timestamp long   kJ (nanoseconds from 2000.01.01)
#define KM 13 // 4     month     int    kI (months from 2000.01.01)
#define KD 14 // 4     date      int    kI (days from 2000.01.01)
#define KZ 15 // 8     datetime  double kF (DO NOT USE)
#define KN 16 // 8     timespan  long   kJ (nanoseconds)
#define KU 17 // 4     minute    int    kI
#define KV 18 // 4     second    int    kI
#define KT 19 // 4     time      int    kI (millisecond)
#define XT 98 //   x->k is XD
#define XD 99 //   kK(x)[0] is keys. kK(x)[1] is values.

#ifdef __cplusplus
#include <cstdarg>
extern "C" {
extern V m9();
#else
#include <stdarg.h>
/**
 * @brief Release current thread's allocated memory.
 */
extern V m9(V);
#endif

/**
 * @brief Serialise a K object.
 * 
 * @param mode
 *      -1 	Serialising within the same process (V3.0+)
 *      0 	Unenumerate, block serialization of timespan and timestamp (prior to V2.6)
 *      1 	Retain enumerations, allow serialisation of timespan and timestamp
 *      2 	Unenumerate, allow serialisation of timespan and timestamp
 *      3 	Unenumerate, compress, allow serialisation of timespan and timestamp
 *      4 	(reserved)
 *      5 	Allow 1TB msgs, but no single vector may exceed 2 billion items
 *      6 	Allow 1TB msgs, and individual vectors may exceed 2 billion items
 * @param object K object.
 * @return A serialised representation of the K object or NULL if error (use `ee` to retrieve
 * error string).
 */
extern K b9(I mode, K object); 

/**
 * @brief Deserialise a K object.
 * 
 * @param object K object.
 * @return Deserialised K object or NULL if error (use `ee` to retrieve error string).
 */
extern K d9(K object);

/**
 * @brief Dynamically link a C function that takes `n` K objects as arguments and returns a K 
 * object (shared library only).
 * 
 * @param func C function to link.
 * @param n Number of arguments C function takes.
 * @return Linked function as a Q function.
 */
extern K dl(V *func, J n);

/**
 * @brief Apply a function to an argument list, i.e., `.[f;x]` (shared library only).
 * 
 * @param func Function to apply.
 * @param args Argument list.
 * @return Linked function as a Q function.
 */
extern K dot(K func, K args);

/**
 * @brief Memory statistics.
 * 
 * @param mode 
 *      0   memory usage for current thread (3 longs):
 *          0   number of bytes allocated
 *          1   bytes available in heap
 *          2   maximum heap size so far
 *      1   symbol stats (2 longs):
 *          0   number of internalised symbols (or NULL if not main thread)
 *          1   corresponding memory usage (or NULL if not main thread)
 * @return Memory stats list.
 */
extern K m4(I mode);

/**
 * @brief Remove the callback on `handle` and call `kclose` (only call from main thread).
 * 
 * @param handle Q connection handle.
 */
extern V sd0(I handle);

/**
 * @brief Remove the callback on `handle` and call `kclose` if `condition` is 1 (only call from
 * main thread).
 * 
 * @param handle Q connection handle.
 * @param condition 1 to remove callback and close.
 */
extern V sd0x(I handle, I condition);

/**
 * @brief Put the function `K func(I handle){…}` on the Q main event loop given a connection
 * handle (only call from main thread).
 * 
 * @param handle Q connection handle. Negative for non-blocking (async).
 * @param func Function to put on Q main event loop. Should return NULL or a K object (ref count
 * will be decremented).
 * @return Connection handle if successful, NULL otherwise (handle closed).
 */
extern K sd1(I handle, K(*func)(I));

/**
 * @brief SSL settings information.
 * 
 * @param x `(K)0`.
 * @return Dictionary of settings similar to `-26!x`, or NULL if SSL initialisation failed.
 * 
 * @example
 * sslInfo((K)0);
 */
extern K sslInfo(K x);

/**
 * @brief Access elements of the types 77..97 inclusive (anymap and nested homogeneous vectors).
 * 
 * This is akin to the macro usage `kK(vec)[i]` for `vec` of type 0.
 * It increments the ref count of the obect ad `vec[i]`.
 * 
 * @param vec Vector to access.
 * @return Element at `vec[i]` or NULL of same type if `i` out of bounds.
 */
extern K vi(K vec, UJ i);

/**
 * @brief Attempt to collapse a general list of homogeneous elements into a simple list, or
 * conforming dictionaries into a table. 
 * 
 * @param object K object to collapse.
 * @return Collapsed object.
 */
extern K vk(K object);

/**
 * @brief Close a Q connection and free any associated resources.
 * 
 * @param handle Q connection handle.
 */
extern V kclose(I handle);

/**
 * @brief Open a network socket connection to a listening Q process.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @return Connection handle (OS file descriptor).
 * 
 * @example
 * I handle = khp("localhost", 12345);
 */
extern I khp(const S host, I port);

/**
 * @brief Open a network socket connection to a listening Q process.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @param credentials Username and password credentials required for authentication (separated by
 * a colon).
 * @return Connection handle (OS file descriptor).
 * 
 * @example
 * I handle = khpu("localhost", 12345, "user:password");
 */
extern I khpu(const S host, I port, const S credentials);

/**
 * @brief Open a network socket connection to a listening Q process.
 * Retry until timeout.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @param credentials Username and password credentials required for authentication (separated by
 * a colon).
 * @param timeout Timeout in milliseconds.
 * @return Connection handle (OS file descriptor) or error code:
 *      0   Authentication error
 *      -1  Connection error
 *      -2  Timeout error
 * 
 * @example
 * I handle = khpun("localhost", 12345, "user:password", 5000);
 */
extern I khpun(const S host, I port, const S credentials, I timeout);

/**
 * @brief Open a network socket connection to a listening Q process.
 * Retry until timeout and used encryption capability.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @param credentials Username and password credentials required for authentication (separated by
 * a colon).
 * @param timeout Timeout in milliseconds.
 * @param capability Encryption capability:
 *      1    1 TB limit
 *      2    use TLS
 * @return Connection handle (OS file descriptor) or error code:
 *      0   Authentication error
 *      -1  Connection error
 *      -2  Timeout error
 *      -3  OpenSSL initialisation failed
 * 
 * @example
 * I handle = khpunc("localhost", 12345, "user:password", 5000, 2);
 */
extern I khpunc(const S host, I port, const S credentials, I timeout, I capability);

/**
 * @brief Verify the given byte vector is a valid IPC message.
 * 
 * @param msg Message to verify.
 * @return object Non-zero if valid, otherwise 0.
 */
extern I okx(K msg);

/**
 * @brief Decrement K object reference count (object freed if ref count <0).
 * 
 * @param object K object to decrement.
 */
extern V r0(K object);

/**
 * @brief Increment K object reference count.
 * 
 * @param object K object to increment.
 * @return Incremented K object.
 */
extern K r1(K object);

/**
 * @brief Create a Q atom of the given type.
 * 
 * @param type Type (type number or encoded type name).
 * @return Atom as K object.
 */
extern K ka(I type);

/**
 * @brief Create a Q boolean atom.
 * 
 * @param x Integer value to create boolean from. 
 * @return K object containing a boolean value.
 */
extern K kb(I x);

/**
 * @brief Create a Q byte atom.
 * 
 * @param x Value to cast.
 * @return Byte as K object.
 */
extern K kg(I x);

/**
 * @brief Create a Q short atom.
 * 
 * @param x Value to cast.
 * @return Short as K object.
 */
extern K kh(I x);

/**
 * @brief Create a Q int atom.
 * 
 * @param x Value to cast.
 * @return Int as K object.
 */
extern K ki(I x);

/**
 * @brief Create a Q long atom.
 * 
 * @param x Value to cast.
 * @return Long as K object.
 */
extern K kj(J x);

/**
 * @brief Create a Q real atom.
 * 
 * @param x Value to cast.
 * @return Real as K object.
 */
extern K ke(F x);

/**
 * @brief Create a Q float atom.
 * 
 * @param x Value to cast.
 * @return Float as K object.
 */
extern K kf(F x);

/**
 * @brief Create a Q char atom.
 * 
 * @param x Value to cast.
 * @return Char as K object.
 */
extern K kc(I x);

/**
 * @brief Create a Q symbol atom.
 * 
 * @param x Value to cast.
 * @return Symbol as K object.
 */
extern K ks(S x);

/**
 * @brief Create a temporal atom whose underlying representation is J.
 *
 * Used for the following q types:
 *   timestamp (-KP)
 *   timespan  (-KN)
 *
 * @param type Temporal type (negative type code, e.g. -KP or -KN).
 * @param value Underlying integer representation (nanoseconds since 2000.01.01).
 * @return Temporal atom as K object.
 *
 * @example K ts = ktj(-KP, nanosSince2000);
 */
extern K ktj(I type, J value);

/**
 * @brief Create a Q date atom.
 * 
 * @param x Value to cast.
 * @return Date as K object.
 */
extern K kd(I x);

/**
 * @brief Create a Q datetime atom.
 * 
 * @param x Value to cast.
 * @return Datetime as K object.
 */
extern K kz(F x);

/**
 * @brief Create a Q time atom.
 * 
 * @param x Value to cast.
 * @return Time as K object.
 */
extern K kt(I x);

/**
 * @brief Create a Q mixed list.
 * 
 * @param length Length of list.
 * @param ... List elements as K objects.
 * @return Mixed list as K object.
 */
extern K knk(I length, ...);

/**
 * @brief Create a Q mixed list (variable arguments form).
 * 
 * @param length Length of list.
 * @param args List elements as K objects.
 * @return Mixed list as K object.
 */
extern K vaknk(I length, va_list args);

/**
 * @brief Create a char array from a string.
 * 
 * @param str String to convert to char array.
 * @return Char array as a K object.
 */
extern K kp(S str);

/**
 * @brief Intern a string as a q symbol.
 * 
 * @param str Null-terminated string.
 * @return Interned symbol pointer.
 */
extern S ss(S str);

/**
 * @brief Intern n chars of a string as a q symbol.
 * 
 * @param str Null-terminated string.
 * @param n Number of chars of the string to intern.
 * @return Interned symbol pointer.
 */
extern S sn(S str, I n);

/**
 * @brief Convert a Q date to yyyymmdd integer.
 * 
 * @param date Date to convert.
 * @return Converted date.
 */
extern I dj(I date);

/**
 * @brief Encode a year/month/day as a Q date (0 == ymd(2000,1,1)).
 * 
 * @param year Date year.
 * @param month Date month.
 * @param day Date day.
 * @return Encoded date.
 */
extern I ymd(I year, I month, I day);

/*
    WARNING:

    `ja`, `js`, `jk`, and `jv` may reallocate the list. Therefore:
        - the list must not have multiple references
        - the caller must pass a pointer to the K object

    If reallocation occurs, the pointer will be updated.
*/ 

/**
 * @brief Append an atom to a list.
 * 
 * @param list Address of list to append to.
 * @param atom Address of atom to append.
 * @return List as K object.
 */
extern K ja(K *list, V *atom);

/**
 * @brief Append an interned string to a list.
 * 
 * @param list Address of list to append to.
 * @param str String to append.
 * @return List as K object.
 */
extern K js(K *list, S str);

/**
 * @brief Append another K object to a list.
 * 
 * @param list Address of list to append to.
 * @param object K object to append.
 * @return List as K object.
 */
extern K jk(K *list, K object);

/**
 * @brief Append two lists (of same type).
 * 
 * @param list1 Address of list to append to.
 * @param list2 List to append.
 * @return List as K object.
 */
extern K jv(K *list1, K list2);

/**
 * @brief Create a dictionary.
 *
 * @param keys List of keys.
 * @param values List of values.
 * @return Dictionary object.
 */
extern K xD(K keys, K values);

/**
 * @brief Create a table from a dictionary.
 *
 * @param dict Dictionary mapping column names to column lists.
 * @return Table.
 */
extern K xT(K dict);

/**
 * @brief Create a simple table from a keyed table.
 * 
 * @param ktab Keyed table.
 * @return Simple table.
 */
extern K ktd(K ktab);

/**
 * @brief Execute a q query over an IPC connection.
 * 
 * @param handle Q connection handle (>0 synchronous, <0 asynchronous).
 * @param query Query to be executed by the receiving Q process.
 * @param ... Arguments to the `query` function (must be terminated with the sentinel `(K)0`).
 * @return Query return value.
 * 
 * @example
 * K k(handle, "query", (K)0);
 * K k(handle, "query", arg1, ..., argN, (K)0);
 */
extern K k(I handle, const S query, ...);

/**
 * @brief Execute a q query over an IPC connection (variable arguments form).
 * 
 * @param handle Q connection handle (>0 synchronous, <0 asynchronous).
 * @param query Query to be executed by the receiving Q process.
 * @param args Arguments to the `query` function (must be terminated with the sentinel `(K)0`).
 * @return Query return value.
 * 
 * @example
 * K vak(handle, "query", args);
 */
extern K vak(I handle, const S query, va_list args);

/**
 * @brief Capture (and reset) an error string.
 * 
 * @param x Expression whose (possible) error should be captured when evaluated.
 * @return Captured error string.
 * 
 * @example
 * K x = ee(dot(a, b));
 * if(xt == -128) printf("error %s\n", x->s);
 */
extern K ee(K x);

/**
 * @brief Construct a q error object.
 * 
 * @param error Error message.
 * @return 0 to indicate an error.
 * 
 * @example
 * return krr("type");
 */
extern K krr(const S error);

/**
 * @brief Construct a system error object.
 * 
 * @param error Error message (limited to 255 characters).
 * @return 0 to indicate an error.
 * 
 * @example
 * return orr("file not found");
 */
extern K orr(const S error);

/**
 * @brief TODO (possible garbage collection?)
 * 
 * @param j TODO
 * @return TODO
 */
extern J gc(J j);

#ifdef __cplusplus
}
#endif

/*
    ##### NULLS AND INFINITIES #####

    q represents null and infinity values using special constants matching the internal
    representation of each primitive type.
*/
#define nh ((I)0xFFFF8000)
#define wh ((I)0x7FFF)
#define ni ((I)0x80000000)
#define wi ((I)0x7FFFFFFF)
#define nj ((J)0x8000000000000000LL) 
#define wj 0x7FFFFFFFFFFFFFFFLL
#if defined(WIN32) || defined(_WIN32)
    #define nf (log(-1.0))
    #define wf (-log(0.0))
    #define finite _finite
    extern double log(double);
#else  
    #define nf (0/0.0)
    #define wf (1/0.0)
    #define closesocket(x) close(x)
#endif 

/*
    ##### UTILITY MACROS #####

    These macros are used internally by many KDB+ examples and simplify common C patterns when
    writing extensions.
*/
#define O printf
#define R return
#define Z static
#define P(x, y) { if(x) R(y); }
#define U(x) P(!(x),0)
#define SW switch
#define CS(n, x) case n:x; break;
#define CD default

#define ZV Z V
#define ZK Z K
#define ZH Z H
#define ZI Z I
#define ZJ Z J
#define ZE Z E
#define ZF Z F
#define ZC Z C
#define ZS Z S

#define K1(f) K f(K x)
#define K2(f) K f(K x, K y)
// +8 skips the first 8 bytes (m, a, t, u, & r) of K struct to get g
#define TX(T,x) (*(T*)((G*)(x)+8)) 
#define xr x->r
#define xt x->t
#define xu x->u
#define xn x->n
#define xx xK[0]
#define xy xK[1]
#define xg TX(G, x)
#define xh TX(H, x)
#define xi TX(I, x)
#define xj TX(J, x)
#define xe TX(E, x)
#define xf TX(F, x)
#define xs TX(S, x)
#define xk TX(K, x)
#define xG x->G0
#define xH ((H*)xG)
#define xI ((I*)xG)
#define xJ ((J*)xG)
#define xE ((E*)xG)
#define xF ((F*)xG)
#define xS ((S*)xG)
#define xK ((K*)xG)
#define xC xG
#define xB ((G*)xG)

#endif // QLIB_K_H
