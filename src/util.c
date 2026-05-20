#include "util.h"

int strcmp(const char* a, const char* b) {

    while (*a && *b) {

        if (*a != *b) {
            return 0;
        }

        a++;
        b++;
    }

    return (*a == 0 && *b == 0);
}

int starts_with(const char* str, const char* prefix) {

    while (*prefix) {

        if (*str != *prefix) {
            return 0;
        }

        str++;
        prefix++;
    }

    return 1;
}

int atoi(const char* str) {

    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {

        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

void strcpy(char* dest, const char* src) {

    int i = 0;

    while (src[i] != 0) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = 0;
}
