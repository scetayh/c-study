#include <stdio.h>
#include <inttypes.h>
#include "bit32_utils.h"

int main() {
    uint32_t src = 0b00011100;
    //                  ^^^
    // position:     76543210
    uint32_t set = 0b00101111;
    //                ^^^
    // position:     76543210
    uint32_t result;
    // expected:     00001000
    // position:        ^^^
    //               76543210

    u32_bit_replace(src, 2, set, 4, 3, &result);

    char src_str[33];
    char set_str[33];
    char dst_str[33];

    u32_to_bin_str(src, src_str, sizeof(src_str));
    u32_to_bin_str(set, set_str, sizeof(set_str));
    u32_to_bin_str(result, dst_str, sizeof(dst_str));

    printf("%s\n%s\n%s\n", src_str, set_str, dst_str);

    return 0;
}