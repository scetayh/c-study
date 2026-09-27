#include "bit32_utils.h"
#include <errno.h>

static inline uint32_t u32_mask(int pos, int width) {
    return (width == 32 ? UINT32_MAX : (UINT32_C(1) << width) - 1) << pos;
}

static inline uint32_t u32_extract(uint32_t src, int src_pos, int width) {
    return src & u32_mask(src_pos, width);
}

static inline uint32_t u32_clear(uint32_t src, int src_pos, int width) {
    return src & ~u32_mask(src_pos, width);
}

static inline int u32_normalize_shift(int shift) {
    int s = shift % 32;
    return (s < 0) ? s + 32 : s;
}

bool u32_replace(const uint32_t src, int src_pos, const uint32_t set,
                 int set_pos, int width, uint32_t *result) {
    CHECK_U32_BIT_REPLACE_RET(src_pos, set_pos, width);
    CHECK_RESULT_NOT_NULL_RET(result);

    *result = u32_clear(src, src_pos, width) |
              (u32_extract(set >> set_pos, 0, width) << src_pos);
    return true;
}

bool u32_flip(uint32_t src, int src_pos, int width, uint32_t *result) {
    CHECK_U32_BIT_RANGE_RET(src_pos, width);
    CHECK_RESULT_NOT_NULL_RET(result);

    *result =
        u32_extract(~src, src_pos, width) | u32_clear(src, src_pos, width);

    return true;
}

uint32_t u32_rotate_left(uint32_t src, int shift) {
    int nshift = u32_normalize_shift(shift);
    return nshift ? (src << nshift) | (src >> (32 - nshift)) : src;
}

uint32_t u32_rotate_right(uint32_t src, int shift) {
    int nshift = u32_normalize_shift(shift);
    return nshift ? (src >> nshift) | (src << (32 - nshift)) : src;
}

int u32_popcount(uint32_t src) {
    int count = 0;

    for (; src; count++) {
        src &= src - 1; // Brian Kernighan 算法
    }

    return count;
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