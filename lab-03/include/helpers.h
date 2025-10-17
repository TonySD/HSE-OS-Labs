#ifndef HELPERS_H
#define HELPERS_H

#include <stdint.h>
#include <time.h>
#define MESSAGE_KEY 0x12345

struct msgbuf_local {
    long mtype;
    size_t amount_of_messages;
    size_t amount_of_authors;
    char authors_buffer[2048];
};

size_t my_strlen(const char *s) {
    size_t result = 0;
    while (*s++) result++;
    return result;
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

char my_strncmp(const char *str1, const char *str2, size_t n) {
    while (*str1 && *str2 && n > 0) {
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
        n--;
    }
    return *str1 == *str2;
}

size_t get_mail_filename(char *output_buffer, const char *current_user) {
    size_t i = 0;
    char* prefix = "/var/mail/";
    for (i = 0; i < my_strlen(prefix); i++) {
        *output_buffer++ = prefix[i];
    }
    for (i = 0; i < my_strlen(current_user); i++) {
        *output_buffer++ = current_user[i];
    }
    i += my_strlen(current_user);
    output_buffer[i] = '\0';
    return i;
}

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

time_t parse_date(const char *s) {
    struct tm tm = {0};
    const char *p = strptime(s, "%a, %d %b %Y %H:%M:%S %z", &tm);
    if (!p) return -1;

    return mktime(&tm);
}

#endif