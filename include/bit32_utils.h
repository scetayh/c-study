/**
 * @file   bit32_utils.h
 * @brief  针对 uint32_t 的安全位运算函数库
 * @author scetayh
 * @date   2026-09-26
 */

#pragma once
#ifndef BIT32_UTILS_H
#define BIT32_UTILS_H

#include "buffer_utils.h"
#include "utils_common.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h> // ssize_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 将 src 中从第 src_pos 位开始的 width 个位，替换为 set 中从第 set_pos
 * 位开始的 width 个位。
 *
 * 该函数从 set 中提取从 set_pos 开始的 width 位，并将其插入到 src 的 src_pos
 * 位置， 保持 src 中其他位不变。位编号从 0（最低有效位，LSB）到
 * 31（最高有效位，MSB）。
 *
 * @param src       源 32 位无符号整数。
 * @param src_pos   目标起始位位置（0 <= src_pos <= 31）。
 * @param set       提供替换位的 32 位无符号整数。
 * @param set_pos   从 set 中提取位的起始位置（0 <= set_pos <= 31）。
 * @param width     要替换的位数（1 <= width <= 32）。
 * @param result    输出参数，指向存放结果的 uint32_t 变量。不能为 NULL。
 *
 * @return 成功时返回 true，并将结果写入 *result。
 * 
 *         若参数无效（src_pos 或 set_pos 超出 [0,31]，width 不在 [1,32]，
 *         src_pos + width > 32，set_pos + width > 32，或 result == NULL），
 *         返回 false，设置 errno = EINVAL，且不修改 *result。
 *
 * @note
 * - 该函数是纯函数，不修改 src 和 set 的值。
 * 
 * - 当 width == 32 时，仅当 src_pos == 0 且 set_pos == 0 时有效，此时整个 src
 * 被 set 替换。
 * 
 * - 调用者必须检查返回值，以区分成功与参数错误。
 *
 * @warning result 不能为 NULL，否则函数返回 false 并设置 errno = EINVAL。
 */
bool u32_bit_replace(uint32_t src, int src_pos, uint32_t set, int set_pos,
                     int width, uint32_t *result);

/**
 * @brief 将 uint32_t 转换为 32 位二进制字符串。
 *
 * @param src           待转换的无符号 32 位整数。
 * @param dst           目标缓冲区，长度至少为 33（32 位 + '\0'）。
 * @param dst_buf_size  目标缓冲区的物理大小。
 *
 * @return 成功时返回 32（二进制字符串长度，不含 '\0'）；
 *         若 dst_buf_size < 33，返回 -1 并设置 errno = EINVAL。
 */
ssize_t u32_to_bin_str(uint32_t src, char *dst, size_t dst_buf_size);

#ifdef __cplusplus
}
#endif

#endif /* BIT32_UTILS_H */
