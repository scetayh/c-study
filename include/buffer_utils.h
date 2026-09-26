/**
 * @file   buffer_utils.h
 * @brief  缓冲区检查宏库
 * @author scetayh
 * @date   2026-09-26
 */

#pragma once
#ifndef BUFFER_UTILS_H
#define BUFFER_UTILS_H

#include <errno.h>     // errno, EINVAL
#include <stddef.h>    // size_t, NULL
#include <sys/types.h> // ssize_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 确保目标缓冲区必须可写。
 *
 * 适用于必须写入目标的函数，不支持“仅计算长度”模式。
 */
#define CHECK_DST_REQUIRED(dst, dst_buf_size)                                  \
    do {                                                                       \
        if ((dst) == NULL || (dst_buf_size) == 0) {                            \
            errno = EINVAL;                                                    \
            return -1;                                                         \
        }                                                                      \
    } while (0)

/**
 * @brief 允许 dst == NULL && dst_buf_size == 0，否则目标缓冲区必须可写。
 *
 * 适用于支持“仅计算长度”模式的函数。
 */
#define CHECK_DST_OPTIONAL(dst, dst_buf_size)                                  \
    do {                                                                       \
        if ((dst) == NULL) {                                                   \
            if ((dst_buf_size) != 0) {                                         \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        } else {                                                               \
            if ((dst_buf_size) == 0) {                                         \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

/**
 * @brief 拒绝 src 与 dst 的任何重叠。
 *
 * 适用于膨胀操作。
 */
#define CHECK_NO_OVERLAP(src, src_len, dst, result_len)                        \
    do {                                                                       \
        if ((src_len) > 0) {                                                   \
            if ((dst) >= (src) && (dst) < (src) + (src_len)) {                 \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
            if ((dst) < (src) && (dst) + (result_len) > (src)) {               \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

/**
 * @brief 允许 src 与 dst 的完全重叠，但拒绝部分重叠（包括右侧重叠和左侧重叠）。
 *
 * 适用于长度不变或缩小操作。已有其他防护时无需使用。
 */
#define CHECK_SELF_OVERLAP_ALLOWED(src, src_len, dst)                          \
    do {                                                                       \
        if ((src_len) > 0) {                                                   \
            if ((dst) > (src) && (dst) < (src) + (src_len)) {                  \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
            if ((dst) < (src) && (dst) + (src_len) > (src)) {                  \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

/**
 * @brief 在“仅计算长度”模式下提前返回。
 *
 * 适用于所有支持 dst == NULL && dst_buf_size == 0 的函数。
 */
#define RETURN_LEN_IF_NO_DST(dst, dst_buf_size, result_len)                    \
    do {                                                                       \
        if ((dst) == NULL || (dst_buf_size) == 0) {                            \
            return (ssize_t)(result_len);                                      \
        }                                                                      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* BUFFER_UTILS_H */
