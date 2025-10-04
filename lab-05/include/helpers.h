#ifndef HELPERS_H
#define HELPERS_H

#include <stddef.h>
#define SOCKET_PORT 13337

int is_digits(const char *s) {
    if (!s || !*s) return 0;
    while (*s) {
        if (*s < '0' || *s > '9') return 0;
        s++;
    }
    return 1;
}

static inline size_t i64_to_str(long n, char *output_buffer) {
    if (n == 0) {
        output_buffer[0] = '0';
        return 1;
    }

    char buffer[128];
    size_t counter = 128, i = 0;
    char negative = 0;

    if (n < 0) {
        negative = 1;
        n = -n;
    }

    while (n > 0) {
        buffer[--counter] = (char)('0' + (n % 10));
        n /= 10;
    }

    if (negative) {
        buffer[counter - 1] = '-';
    }

    while (counter < 128) {
        output_buffer[i++] = buffer[counter++];
    }
    output_buffer[i] = 0;

    return i;
}

long str_to_i64(const char *s) {
    long result = 0;
    char negative = 0;
    if (*s == '-') {
        s++;
        negative = 1;
    } else if (*s == '+') {
        s++;
    }
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result;
}

#endif


