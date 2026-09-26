/**
 * @file   bit_utils.h
 * @brief  安全位运算操作函数库
 * @author scetayh
 * @date   2026-09-26
 */

#pragma once
#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t setbits(const uint32_t src, int pos, int width, uint32_t set);

#ifdef __cplusplus
}
#endif

#endif /* BIT_UTILS_H */
