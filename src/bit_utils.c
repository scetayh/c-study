#include "bit_utils.h"
#include <errno.h>

uint32_t setbits(const uint32_t src, int pos, int width, uint32_t set) {
    if (!((pos >= 0 && pos <= 31) || (width >= 1 && width <= 31) ||
          (pos >= 1 && pos <= 32))) {
        errno = EINVAL;
        return src;
    }

    return ;
}