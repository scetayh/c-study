#include <stdio.h>
#include "string_utils.h"

#define MAXLEN 1000
#define TAB_WID 8

int main() {
    int ch;
    char src[MAXLEN];
    char dst[MAXLEN];

    for (int i = 0; (ch = getchar()) != EOF; i++) {
        src[i] = ch;
    }

    str_detab(src, sizeof(src), dst, sizeof(dst), TAB_WID);

    printf("%s\n", dst);

    return 0;
}