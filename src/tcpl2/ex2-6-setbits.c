#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

uint32_t setbits(const uint32_t x, int p, int n, uint32_t y) {
    if (p < 0 || n <= 0 || p > 32 || n > 32 - p) {
        errno = EINVAL;
        return x;
    }

    uint32_t mask = (n == 32) ? UINT32_MAX : (UINT32_C(1) << n) - 1;
}

int main() {}