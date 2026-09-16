#include <stdio.h>
#include "string_utils.h"

#define MAXLEN 1000
#define TABLEN 8

int main() {
    char src[MAXLEN];
    char dst[MAXLEN];

    str_read(src, sizeof(src));
    str_entab(src, sizeof(src), dst, sizeof(dst), TABLEN);

    printf("%s\n", dst);

    return 0;
}