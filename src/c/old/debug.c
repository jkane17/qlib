
/**
 * @file debug.c
 * @brief Debugging utilities.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "fmt.h"
#include "k.h"
#include "util.h"

#define ATOM_BUFFER_SIZE 64
#define MAX_LIST_LEN 10
#define MAX_BOOL_LEN 20
#define MAX_STRING_LEN 20

#define INDENT 4
#define PRINT_INDENTED(level, fmt, ...) printf("%*s" fmt, (level) * INDENT, "", ##__VA_ARGS__)

#define KEY_WIDTH 7
#define PRINT_KV(level, k, fmt, ...) \
    PRINT_INDENTED(level, "%-*s: " fmt "\n", KEY_WIDTH, k, ##__VA_ARGS__)

static void print(I level, K x);

static const char *type_names[] = {
    "list",
    "boolean",   
    "guid",
    "",    
    "byte",      
    "short",     
    "int",       
    "long",      
    "real",      
    "float",     
    "char",      
    "symbol",    
    "timestamp",
    "month",
    "date",
    "datetime",
    "timespan",
    "minute",
    "second",    
    "time",
    "table",
    "dictionary"
};

static void print_atom(I level, K x) {
    C buffer[ATOM_BUFFER_SIZE];

    I written = k_value_to_str(buffer, ATOM_BUFFER_SIZE, xt, k_value(x));
    buffer[written] = '\0';
    
    PRINT_KV(level, "Value", "%s", buffer);
}

static void print_bool_list(I level, K x) {
    SZ len = xn > MAX_BOOL_LEN ? MAX_BOOL_LEN : xn;
    C buffer[MAX_BOOL_LEN + 1];
    SZ i;

    for (i = 0; i < len; i++) {
        sprintf(buffer + i, "%hhd", xB[i]);
    }

    buffer[i] = '\0';

    if (xn > MAX_BOOL_LEN)
        PRINT_KV(level, "Value", "%.*s..b", MAX_BOOL_LEN, buffer);
    else
        PRINT_KV(level, "Value", "%.*sb", (I)xn, buffer);
}

static void print_string(I level, K x) {
    if (xn > MAX_STRING_LEN)
        PRINT_KV(level, "Value", "\"%.*s..", MAX_STRING_LEN, xC);
    else
        PRINT_KV(level, "Value", "\"%.*s\"", (I)xn, xC);
}

static void print_list(I level, K x) {
    PRINT_KV(level, "Count", "%lld", xn);

    if (xt == KB) {
        print_bool_list(level, x);
        return;
    } else if (xt == KC) {
        print_string(level, x);
        return;
    }

    bool large = xn > MAX_LIST_LEN;
    SZ len = large ? MAX_LIST_LEN : xn;
        
    PRINT_KV(level, "Value", "[");

    if (xt == 0) {
        for (SZ i = 0; i < len; i++) {
           print(level + 1, k_index(x, i));
        }
    } else {
        C buffer[ATOM_BUFFER_SIZE];

        for (SZ i = 0; i < len; i++) {
            I written = k_value_to_str(buffer, ATOM_BUFFER_SIZE, -xt, k_index(x, i));
            buffer[written] = '\0';
            PRINT_INDENTED(level + 1, "%s", buffer);
            if (large || i != len - 1) printf(",");
            printf("\n");
        }
    }

    if (large) PRINT_INDENTED(level + 1, "%s", "..\n");

    PRINT_INDENTED(level, "]\n");
}

static void print_table(I level, K x) {
    (V)x;
    // PRINT_KV("Count", "%lld", xn);
    PRINT_KV(level, "Value", "TODO");
}

static void print_dict(I level, K x) {
    (V)x;
    // PRINT_KV("Count", "%lld", xn);
    PRINT_KV(level, "Value", "TODO");
}

static void print(I level, K x) {
    int type_index = abs(xt);
    if (type_index == 98 || type_index == 99) type_index -= 78;
    
    char type_suffix[3] = { '\0' }; 
    if (0 < xt && xt < 20) strcpy(type_suffix, "[]");

    PRINT_INDENTED(level, "K {\n");
    
    PRINT_KV(level + 1, "Type", "%s%s (%d)", type_names[type_index], type_suffix, xt);
    if (xu) PRINT_KV(level + 1, "Attr", "%c", xu);
    PRINT_KV(level + 1, "Refc", "%d", xr);

    if (xt < 0) print_atom(level + 1, x);
    else if (xt == 98) print_table(level + 1, x);
    else if (xt == 99) print_dict(level + 1, x);
    else print_list(level + 1, x);

    PRINT_INDENTED(level, "}\n");
}

void k_debug_print(K x) { print(0, x); }
