#ifndef HELPERS_H
#define HELPERS_H

#include <sys/types.h>
#include <stddef.h>

#define SHM_KEY 0x24680
#define SHM_SIZE 65536

struct shm_header {
    size_t data_size;
    int read_done;
};

size_t i64_to_str(long n, char *output_buffer) {
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

    return i;
};

#endif
