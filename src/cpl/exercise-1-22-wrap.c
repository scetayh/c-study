#include <stdio.h>
#include "string_utils.h"

#define MAXLEN 32768
#define TABLEN 4
#define COLLIM 80

int main() {
    char src[MAXLEN];
    char dst[MAXLEN];

    str_read(src, sizeof(src));
    str_wrap(src, sizeof(src), dst, sizeof(dst), TABLEN, COLLIM);

    printf("%s", dst);

    return 0;
}