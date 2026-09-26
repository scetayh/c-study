#include "bit32_utils.h"
#include <errno.h>

bool u32_bit_replace(const uint32_t src, int src_pos, const uint32_t set,
                     int set_pos, int width, uint32_t *result) {
    if (src_pos < 0 || src_pos > 31 || set_pos < 0 || set_pos > 31 ||
        width < 1 || width > 32 - src_pos || width > 32 - set_pos) {
        errno = EINVAL;
        return false;
    }

    uint32_t mask = width == 32 ? UINT32_MAX : (UINT32_C(1) << width) - 1;
    uint32_t src_mask = ~(mask << src_pos);
    uint32_t set_bits = (set >> set_pos) & mask;
    *result = (src & src_mask) | (set_bits << src_pos);

    return true;
}

ssize_t u32_to_bin_str(uint32_t src, char *dst, size_t dst_buf_size) {
    CHECK_DST_OPTIONAL(dst, dst_buf_size);

    if (dst == NULL) {
        return 32; // 仅计算长度模式
    }

    if (dst_buf_size < 33) {
        errno = ERANGE;
        return -1;
    }

    for (size_t i = 0; i < 32; i++) {
        dst[i] = (src >> (31 - i)) & 1 ? '1' : '0';
    }
    dst[32] = '\0';

    return 32;
}