
/**
 * @file fstr.c
 * @brief Format string.
 */

#include <stdio.h>

#include "common.h"
#include "fmt.h"
#include "k.h"
#include "util.h"

#ifndef NDEBUG
#include "debug.h"
#endif

static I fmt_write(S buffer, SZ size, K x, SZ i) {
    I bytes_written;

    if (xt < 0)
        bytes_written = k_value_to_str(buffer, size, xt, k_value(x));
    else if (xt > 0)
        bytes_written = k_value_to_str(buffer, size, -xt, k_index(x, i));
    else {
        K list = k_index(x, i);
        bytes_written = k_value_to_str(buffer, size, list->t, list);
    }

    return bytes_written;
}

K fmt(K format, K args) {
    if (format->t != KC) return krr("type");

    // k_debug_print(format);
    // k_debug_print(args);

    C buffer[BUFFER_SIZE];
    S buffer_pos = buffer;
    SZ size = BUFFER_SIZE;

    SZ ifmt;
    SZ iarg;
    SZ fmt_len = format->n;
    SZ args_len = args->t < 0 ? 1 : args->n;

    for (ifmt = 0, iarg = 0; ifmt < fmt_len; ifmt++) {
        C ch = kC(format)[ifmt];
        
        if (ch == '{' && ifmt + 1 != fmt_len && kC(format)[ifmt + 1] == '}') {
            if (iarg == args_len) return krr("length");

            I bytes_written = fmt_write(buffer_pos, size, args, iarg);
            if (bytes_written == PRINTF_ERROR) return krr("type");

            buffer_pos += bytes_written;
            
            size = saturated_sub(size, bytes_written);
            if (size == 0) return krr("bufferFull");
            
            ifmt++; // Additional increment to skip closing '}'
            iarg++;
        } else {
            buffer[buffer_pos - buffer] = ch;
            buffer_pos++;
        }
    }

    SZ total_bytes_written = buffer_pos - buffer;
    buffer[total_bytes_written >= BUFFER_SIZE ? BUFFER_SIZE : total_bytes_written] = '\0';

    return kpn(buffer, total_bytes_written);
}

K kexport(K _x) {
    (V)_x;

    K names = ktn(KS, 1);
    kS(names)[0] = ss("fmt");
    
    K fns = ktn(0, 1);
    kK(fns)[0] = dl((V*)fmt, 2);
    
    return xD(names, fns);
}
