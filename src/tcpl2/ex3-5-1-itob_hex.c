#include <stdio.h>
#include "bit32_utils.h"

int main() {
    uint32_t num = 114514;
    char s[1024];
    u32_to_hex_str(num, s, sizeof(s));
    printf("%s\n", s);
}