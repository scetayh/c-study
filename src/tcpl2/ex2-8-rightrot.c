#include <stdio.h>
#include <inttypes.h>
#include "bit32_utils.h"

int main() {
    uint32_t src = 0b00000010110111011110111110111111;
    // position:                      ^^
    //               33222222222211111111119876543210
    //               1098765432109876543210

    uint32_t result = u32_rotate_right(src, 14);
    
    char dst[33];
    u32_to_bin_str(result, dst, sizeof(dst));
    printf("%s\n", dst);

    return 0;
}