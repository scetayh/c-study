#include <stdio.h>
#include <stdint.h>
#include "bit32_utils.h"

int main() {
    uint32_t src = 0b00111100;
    // position:       ^^^^
    //               76543210
    uint32_t result;
    // expected:     00000000
    //                 ^^^^
    // position:     76543210

    u32_flip(src, 2, 4, &result);

    char dst[33];
    u32_to_bin_str(result, dst, sizeof(dst));
    printf("%s\n", dst);

    return 0;
}