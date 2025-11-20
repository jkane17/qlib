
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Map a digit value [0..35] to a character
static char digitToChar(int digit) {
    return (digit < 10) ? ('0' + digit) : ('A' + (digit - 10));
}

// Reverse a string in place
static void reverseStr(char *str, int len) {
    for (int i = 0, j = len - 1; i < j; i++, j--) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}

// Convert an unsigned integer to string in given base
static int u64ToStr(uint64_t value, char *str, int base) {
    int i = 0;
    while (value != 0) {
        int digit = value % base;
        str[i++] = digitToChar(digit);
        value /= base;
    }
    str[i] = '\0';
    reverseStr(str, i);
    return i; // number of chars written (excluding '\0')
}

char *int64ToStr(int64_t value, int base) {
    if (base < 2 || base > 36) { return NULL; } // Invalid base

    char *out = NULL;

    // Max string length:
    //  - binary: 64 digits
    //  - sign: +1
    //  - null terminator: +1
    char temp[66];

    if (value == 0) {
        out = malloc(2);
        if (!out) return NULL;
        out[0] = '0';
        out[1] = '\0';
    } else if (value < 0 && base == 10) {
        if (value == INT64_MIN) { // Special case
            uint64_t tempVal = (uint64_t)(-(value + 1)) + 1ULL;
            int len = u64ToStr(tempVal, temp, base);
            // prepend minus
            memmove(temp + 1, temp, len + 1);
            temp[0] = '-';
            out = malloc(len + 2);
            if (!out) return NULL;
            strcpy(out, temp);
        } else {
            value = -value;
            int len = u64ToStr((uint64_t)value, temp, base);
            memmove(temp + 1, temp, len + 1);
            temp[0] = '-';
            out = malloc(len + 2);
            if (!out) return NULL;
            strcpy(out, temp);
        }
    } else {
        int len = u64ToStr((uint64_t)value, temp, base);
        out = malloc(len + 1);
        if (!out) return NULL;
        strcpy(out, temp);
    }

    return out;
}
