#ifndef HELPERS_H
#define HELPERS_H
#include <stdint.h>

#define MESSAGE_KEY 0x12345

struct msgbuf_local {
    long mtype;
    char mtext[256];
};

int64_t str_to_i64(const char *s) {
    int64_t result = 0;
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }
    return result;
}

void my_puts(const char *s) {
    write(1, s, strlen(s));
    write(1, "\n", 1);
}

#endif