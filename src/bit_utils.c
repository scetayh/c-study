#include "bit_utils.h"
#include <errno.h>

uint32_t bit_replace(const uint32_t src, int src_pos, const uint32_t set,
                     int set_pos, int width) {
    if (
        (src_pos < 0 || src_pos > 31) ||
        (set_pos < 0 || set_pos > 31) ||
        src_pos + width > 32
    ) {
        errno = EINVAL;
        return src;
    }

    
    }