#include <stdio.h>

#include "../../include/string_utils.h"

#define MAXLEN 1024

int main() {
    char s[MAXLEN];

    str_read(s, sizeof(s));

    ssize_t pos = str_any(s, sizeof(s), "aeiou", 6);

    if (pos >= 0) {
        printf("first vowel in index %zd\n", pos);
    } else {
        printf("vowel not found\n");
    }

    return 0;
}
