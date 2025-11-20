
#ifndef K_H_INCLUDED
#define K_H_INCLUDED

// C types as single characters
typedef char *S, C;
typedef unsigned char G;
typedef short H;
typedef int I;
typedef long long J;
typedef float E;
typedef double F;
typedef void V;
typedef unsigned long long UJ;

#ifdef __cplusplus
    extern"C"{
#endif

// K object data structure
typedef struct k0 {
    signed char m, a, t; // m and a are internal. t is type
    C u;                 // Attribute flags
    I r;                 // Reference count
    union {
        G g;             // boolean, byte
        H h;             // short
        I i;             // int, char, month, date, minute, second, time
        J j;             // long, timestamp, timespan
        E e;             // real
        F f;             // float, datetime
        S s;             // symbol
        struct k0 *k;    // dict, table, keyed table
        struct {
            J n;         // List length
            G G0[];
        };
    };
} *K;

typedef struct { G g[16]; } U;
#define kU(x) ((U*)kG(x))
#define xU ((U*)xG)

/**
 * @brief Create a Q guid type.
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

extern K knt(J, K);

extern I setm(I), ver();

#define DO(n, x) {           \
    J i=0,_i=(n);            \
    for (; i<_i; ++i) { x; } \
}

#ifdef __cplusplus
    }
#endif


// ##### VECTOR ACCESSORS #####

// Usage Example : kJ(x)[i]
#define kG(x) ((x)->G0)
#define kC(x) kG(x)
#define kH(x) ((H*)kG(x))
#define kI(x) ((I*)kG(x))
#define kJ(x) ((J*)kG(x)) 
#define kE(x) ((E*)kG(x))
#define kF(x) ((F*)kG(x))
#define kS(x) ((S*)kG(x))
#define kK(x) ((K*)kG(x))


// ##### ENCODED TYPE NAMES #####

//      type     bytes qtype     ctype  accessor
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


// ##### MEMORY MANAGEMENT AND CONNECTIONS #####

#ifdef __cplusplus
    #include <cstdarg>
    extern"C"{
    extern V m9();
#else
    #include <stdarg.h>
    extern V m9(V);
#endif

/**
 * @brief Close a Q connection and free any associated resources.
 * 
 * @param handle Q connection handle
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
 * @param userPass Username and password credentials required for authentication 
 * (separated by a colon).
 * @return Connection handle (OS file descriptor).
 * 
 * @example
 * I handle = khpu("localhost", 12345, "user:password");
 */
extern I khpu(const S host, I port, const S userPass);

/**
 * @brief Open a network socket connection to a listening Q process.
 * Retry until timeout.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @param userPass Username and password credentials required for authentication 
 * (separated by a colon).
 * @param timeout Timeout in milliseconds.
 * @return Connection handle (OS file descriptor).
 * 
 * @example
 * I handle = khpun("localhost", 12345, "user:password", 5000);
 */
extern I khpun(const S host, I port, const S userPass, I timeout);

/**
 * @brief Open a network socket connection to a listening Q process.
 * Retry until timeout and used encryption capability.
 * 
 * @param host Hostname of the listening Q process.
 * @param port Port of the listening Q process.
 * @param userPass Username and password credentials required for authentication 
 * (separated by a colon).
 * @param timeout Timeout in milliseconds.
 * @param capability Encryption capability.
 * @return Connection handle (OS file descriptor).
 * 
 * @example
 * I handle = khpunc("localhost", 12345, "user:password", 5000, 2);
 */
extern I khpunc(const S host, I port, const S userPass, I timeout, I capability);

/**
 * @brief Decrement K object reference count (object freed if ref count is 0).
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


// ##### CREATE K OBJECT #####

/**
 * @brief Create a Q atom of the given type.
 * 
 * @param x Type (type number or encoded type name).
 * @return Atom as K object.
 */
extern K ka(I x);

/**
 * @brief Create a Q boolean type.
 * 
 * @param x Value to cast.
 * @return Boolean as K object.
 */
extern K kb(I x);

/**
 * @brief Create a Q byte type.
 * 
 * @param x Value to cast.
 * @return Byte as K object.
 */
extern K kg(I x);

/**
 * @brief Create a Q short type.
 * 
 * @param x Value to cast.
 * @return Short as K object.
 */
extern K kh(I x);

/**
 * @brief Create a Q int type.
 * 
 * @param x Value to cast.
 * @return Int as K object.
 */
extern K ki(I x);

/**
 * @brief Create a Q long type.
 * 
 * @param x Value to cast.
 * @return Long as K object.
 */
extern K kj(I x);

/**
 * @brief Create a Q real type.
 * 
 * @param x Value to cast.
 * @return Real as K object.
 */
extern K ke(F x);

/**
 * @brief Create a Q float type.
 * 
 * @param x Value to cast.
 * @return Float as K object.
 */
extern K kf(F x);

/**
 * @brief Create a Q char type.
 * 
 * @param x Value to cast.
 * @return Char as K object.
 */
extern K kc(I x);

/**
 * @brief Create a Q symbol type.
 * 
 * @param x Value to cast.
 * @return Symbol as K object.
 */
extern K ks(S x);

/**
 * @brief Create a Q timestamp type.
 * 
 * @param x Value to cast.
 * @param y TODO
 * @return Timestamp as K object.
 */
extern K ktj(I x, J y);

/**
 * @brief Create a Q date type.
 * 
 * @param x Value to cast.
 * @return Date as K object.
 */
extern K kd(I x);

/**
 * @brief Create a Q datetime type.
 * 
 * @param x Value to cast.
 * @return Datetime as K object.
 */
extern K kz(F x);

/**
 * @brief Create a Q timespan type.
 * 
 * @param x Value to cast.
 * @param y TODO
 * @return Timespan as K object.
 */
extern K ktj(I x, J y);

/**
 * @brief Create a Q time type.
 * 
 * @param x Value to cast.
 * @return Time as K object.
 */
extern K kt(I x);

/**
 * @brief Create a Q mixed list (shortcut).
 * 
 * @param length Length of list.
 * @param ... List elements as K objects.
 * @return Mixed list as K object.
 */
extern K knk(I length,...);


// ##### STRING HANDLING #####

/**
 * @brief Create a char array from a string.
 * 
 * @param str String to convert to char array.
 * @return Char array as a K object.
 */
extern K kp(S str);

/**
 * @brief Inter a string.
 * 
 * @param str String to inter.
 * @return S Interned string as a Q symbol type.
 */
extern S ss(S str);

/**
 * @brief Inter n chars from a string.
 * 
 * @param str String to inter.
 * @param n Number of chars of the string to intern.
 * @return S Interned string as a Q symbol type.
 */
extern S sn(S str, I n);


// ##### DATE HANDLING #####

/**
 * @brief Convert a Q date to yyyymmdd integer.
 * 
 * @param date Q date to convert.
 * @return Converted date.
 */
extern I dj(I date);

/**
 * @brief Encode a year/month/day as a Q date (0 == ymd(2000,1,1)).
 * 
 * @param year Date year.
 * @param month Date month.
 * @param day Date day.
 * @return I Encoded date.
 */
extern I ymd(I year, I month, I day);


// ##### DYNAMIC LIST APPEND #####

/*
    These functions assume there are no other references to the list, as the 
    list may need to be reallocated during the call. In case of reallocation,
    the passed K* pointer will be updated to refer to new K object and will be
    returned from the function.
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
 * @brief Append a string to a list.
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
 * @brief Append another K list to a list.
 * 
 * @param list1 Address of list to append to.
 * @param list2 List to append.
 * @return List as K object.
 */
extern K jv(K* list1, K list2);


// ##### DICT AND TABLE CREATION #####


extern K xD(K, K);

extern K xT(K);

extern K ktd(K);


// ##### QUERY #####

/**
 * @brief Query a Q process.
 * 
 * @param handle Q connection handle.
 * @param query  Query to be executed by the receiving Q process.
 * @param ...    Arguments to the `query` function. A sentinel value, `(K)0`,
 * must be supplied at the end of the argument list to allow detection of list 
 * end.
 * @return Query return value.
 * 
 * @example
 * K k(handle, "query", (K)0);
 * K k(handle, "query", arg1, ..., argN, (K)0);
 */
extern K k(I handle, const S query,...);

/**
 * @brief Query a Q process (variable arguments form).
 * 
 * @param handle Q connection handle.
 * @param query  Query to be executed by the receiving Q process.
 * @param args   Arguments to the `query` function. A sentinel value, `(K)0`,
 * must be supplied at the end of the argument list to allow detection of list 
 * end.
 * @return Query return value.
 * @example
 * K vak(handle, "query", args);
 */
extern K vak(I handle, const S query, va_list args);


// ##### ERROR HANDLING #####

/**
 * @brief Raise a Q error.
 * 
 * @param error Q error flag.
 * @return K object with error.
 * 
 * @example
 * krr("type");
 */
extern K krr(const S error);


// ##### BLAH #####

extern J gc(J j);
extern I okx(K);
extern V sd0(I), sd0x(I d, I f);
extern K ee(K), sd1(I, K(*)(I)), dl(V*f, J), m4(I), orr(const S), dot(K, K), 
    b9(I, K), d9(K), sslInfo(K x), vaknk(I, va_list), vi(K, UJ);

#ifdef __cplusplus
    }
#endif

// nulls (n?) and infinities (w?)
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

// Remove more clutter
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

#endif
