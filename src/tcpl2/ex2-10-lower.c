#include <stdio.h>
#include "string_utils.h"

int main() {
    char src[16] = "HeLlo, WorLD";
    char dst[16];

    str_lower(src, sizeof(src), dst, sizeof(dst));

    printf("%s\n", dst);

    return 0;
}