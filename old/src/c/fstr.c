
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "k.h"
#include "util.h"

char *FSPEC_MAP[] = {
    [1] = "%%",
    "%%",
    "%%",
    "%%",
    "%%",
    "%%",
    "%ld",
    "%%",
    "%%"
};


// typedef struct {
//     K args;
//     uint64_t n;
// } ArgIter;

// static K nextArg(ArgIter *argIter) {
//     if (argIter == NULL) { return NULL; }
    
//     K args = argIter->args;
    
//     if (args->t < 0) { return args; }

//     switch (args->t) {
//         case 1: case 4: return kG(args)[argIter->n++];
//         case 2: return kU(args)[argIter->n++];
//     }
// }

// static K argToStr(K arg) {

// }

K fstr_rep(K value) {
    char *buf = NULL;

    switch (value->t) {
        case -1:  // boolean
            buf = malloc(5);
            if (buf) { snprintf(buf, 5, "%db", value->g); }
            break;

        case -2:  // UUID (16 bytes)
            buf = malloc(37); // 32 hex + 4 dashes + '\0'
            if (buf) {
                char *p = buf;
                for (int i = 0; i < 16; i++) {
                    p += sprintf(p, "%02x", kU(value)->g[i]);
                    if (i == 3 || i == 5 || i == 7 || i == 9)
                        *p++ = '-';
                }
                *p = '\0';
            }
            break;

        case -4:  // byte
            buf = malloc(5);
            if (buf) { snprintf(buf, 5, "0x%02x", value->g); }
            break;

        case -5:  // short
            buf = malloc(7);
            if (buf) { snprintf(buf, 7, "%d", value->h); }
            break;

        case -6:  // int
            buf = malloc(11);
            if (buf) { snprintf(buf, 11, "%d", value->i); }
            break;

        case -7:  // long
            buf = malloc(20);
            if (buf) { snprintf(buf, 20, "%lld", value->j); }
            break;

        default:
            buf = strdup("Unknown type");
    }

    return kp(buf);
}

// @brief Replace "{}" with positional values (similar to Python fstring).
// @param str String Format string.
// @param args Any Arguments to insert.
// @return String Formated string.
// K fstr_format(K str, K args) {
//     if (str->t != KC) { return krr("type"); }

//     char *fspec = NULL;
//     uint8_t fspecLen;
//     if (args->t < 0) { // Single argument replacement
//         fspec = FSPEC_MAP[abs(args->t)];
//         fspecLen = strlen(fspec);
//     }
    
//     uint64_t i, j, fstri = 0, fstrSize = str->n + 1, nreplacements = 0;
//     char *fstr = malloc(fstrSize);
//     if (fstr == NULL) { return krr("alloc"); }

//     for (i = 0; i < str->n; i++) {

//         // printf("Iter begin: i = %ld, fstri = %ld, fstrSize = %ld\n", i, fstri, fstrSize);

//         if (kC(str)[i] == '{' && i != str->n - 1 && kC(str)[i + 1] == '}') {
//             if (args->t >= 0) {
//                 fspec = FSPEC_MAP[abs(args->t)];
//                 fspecLen = strlen(fspec);
//             }
            
//             if (fstri + fspecLen > fstrSize) {
//                 fstrSize *= 2;
//                 fstr = realloc(fstr, fstrSize);
//                 if (fstr == NULL) { return krr("alloc"); }
//             }
//             for (j = 0; j < fspecLen; j++) {
//                 fstr[fstri++] = fspec[j];
//             }
//             i++; // Skip the '}'
//             // printf("Post replacement: i = %ld, fstri = %ld, fstrSize = %ld\n", i, fstri, fstrSize);
//             nreplacements++;
//         } else {
//             if (fstri == fstrSize) {
//                 fstrSize *= 2;
//                 fstr = realloc(fstr, fstrSize);
//                 if (fstr == NULL) { return krr("alloc"); }
//             }
//             fstr[fstri++] = kC(str)[i];
//             // printf("Post copy: i = %ld, fstri = %ld, fstrSize = %ld\n", i, fstri, fstrSize);
//         }
//         // printf("\n");
//     }

//     if (nreplacements != 1 && nreplacements != args->n)

//     if (fstri == fstrSize) {
//         fstrSize += 1;
//         fstr = realloc(fstr, fstrSize);
//         if (fstr == NULL) { return krr("alloc"); }
//     }
//     fstr[fstri] = '\0';

//     // printf("Done: fstri = %ld, fstrSize = %ld, fstr = %s\n", fstri, fstrSize, fstr);

//     return kp(fstr);
// }
