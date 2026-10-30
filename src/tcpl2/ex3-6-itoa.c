#include <stdio.h>
#include <stdint.h>
#include "string_utils.h"
#include "bit32_utils.h"

int main() {
    uint32_t i = 1919810;
    char a[32];

    printf("[ret] %ld\n", u32_to_dec_str(i, a, sizeof(a)));
    
    printf("%s\n", a);

    return 0;
}