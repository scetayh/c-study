/**
 * @file   utils_common.h
 * @brief  工具函数共享宏库
 * @author scetayh
 * @date   2026-09-26
 */

#pragma once
#ifndef UTILS_COMMON_H
#define UTILS_COMMON_H

#include <errno.h>     // errno, EINVAL, EFBIG
#include <limits.h>    // SSIZE_MAX
#include <stddef.h>    // size_t, NULL
#include <sys/types.h> // ssize_t

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a, b) (((a) > (b)) ? (a - b) : (b - a))
#endif

/**
 * @brief 一次性检查多个指针非空。
 *
 * 适用于所有需要读取源字符串的函数。
 */
#define CHECK_NOT_NULL(...)                                                    \
    do {                                                                       \
        const void *_ptrs[] = {__VA_ARGS__};                                   \
        for (size_t _i = 0; _i < sizeof(_ptrs) / sizeof(_ptrs[0]); _i++) {     \
            if (_ptrs[_i] == NULL) {                                           \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

/**
 * @brief 一次性检查多个 int 参数为正数。
 */
#define CHECK_INT_POSITIVE(...)                                                \
    do {                                                                       \
        int _args[] = {__VA_ARGS__};                                           \
        for (size_t _i = 0; _i < sizeof(_args) / sizeof(_args[0]); _i++) {     \
            if (_args[_i] <= 0) {                                              \
                errno = EINVAL;                                                \
                return -1;                                                     \
            }                                                                  \
        }                                                                      \
    } while (0)

/**
 * @brief 检查计算结果是否超过 ssize_t 正数范围。
 */
#define CHECK_LEN_OVERFLOW(result_len)                                         \
    do {                                                                       \
        if ((result_len) > (size_t)SSIZE_MAX) {                                \
            errno = EFBIG;                                                     \
            return -1;                                                         \
        }                                                                      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* UTILS_COMMON_H */
