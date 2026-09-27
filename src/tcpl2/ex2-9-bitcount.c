#include <stdio.h>
#include <inttypes.h>
#include "bit32_utils.h"

int main() {
    uint32_t src = 0b00110011100110; // '1' * 7 in total
    printf("%d\n", u32_popcount(src));

    return 0;
}