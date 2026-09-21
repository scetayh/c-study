/**
 * @file   string_utils.h
 * @brief  安全字符串操作函数库（复制、反转、制表符展开/压缩等）
 * @author scetayh
 * @date   2026-09-21
 *
 * 所有函数均遵循 "snprintf 契约"：
 *
 * 返回所需总长度（不含 '\0'），调用者通过判断返回值是否 >=
 * dst_buf_size 来检测截断；
 *
 * 传入的 src_buf_size 必须为对应数组的 sizeof(数组名) 结果。
 *
 * 本函数仅适用于纯文本，其中退格、回车、ANSI
 * 转义序列可能干扰制表位对齐。
 */

#pragma once
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <bits/posix1_lim.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * @brief 确保源指针非空。
 *
 * 适用于所有需要读取源字符串的函数。
 */
#define CHECK_SRC_NOT_NULL(src)                                                \
    do {                                                                       \
        if ((src) == NULL) {                                                   \
            errno = EINVAL;                                                    \
            return -1;                                                         \
        }                                                                      \
    } while (0)

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
 * @brief 一次性检查多个 int 参数为正数。
 */
#define CHECK_POSITIVE_PARAMS(...)                                             \
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
 * @brief 确保制表符宽度不超过列宽限制。
 */
#define CHECK_TAB_WIDTH_LE_COL_LIM(tab_width, col_lim)                         \
    do {                                                                       \
        if ((tab_width) > (col_lim)) {                                         \
            errno = EINVAL;                                                    \
            return -1;                                                         \
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
 * @brief 拒绝 src 与 dst 的完全重叠，但拒绝部分重叠（包括右侧重叠和左侧重叠）。
 *
 * 适用于反转操作。
 */
#define CHECK_REVERSE_OVERLAP(src, src_len, dst)                               \
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

/**
 * @brief 安全写入单个字符，自动检查边界并递增 dst_i 。
 */
#define WRITE_DST(ch)                                                          \
    do {                                                                       \
        if (dst_i < dst_buf_size - 1) {                                        \
            dst[dst_i] = ch;                                                   \
        }                                                                      \
        dst_i++;                                                               \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 从标准输入（stdin）读取字符，直到遇到 EOF，并复制到目标缓冲区。
 *
 * 该函数持续调用 getchar() 读取字符，直至文件结束（EOF），
 * 并将读取的字符依次存入 dst。如果输入长度超过 dst_buf_size - 1，
 * 则只存储前 dst_buf_size - 1
 * 个字符，后续读取的字符被丢弃（不会影响后续调用）。
 *
 * 目标缓冲区始终以 '\0' 结尾。
 *
 * @param dst           目标缓冲区（不必预先初始化）
 * @param dst_buf_size  目标缓冲区的物理大小（字节数），必须 > 0
 *
 * @return 成功时返回从输入流中读取的总字符数（不含结尾的 '\0'）：
 *
 *         - 若返回值 < dst_buf_size，表示完整读取。
 *
 *         - 若返回值 >= dst_buf_size，表示发生截断（输入过长），
 *           此时 dst 中存放前 (dst_buf_size - 1) 个字符，且以 '\0' 结尾。
 *
 *         若 dst 为 NULL 或 dst_buf_size == 0，返回 -1 并设置 errno = EINVAL。
 *
 *         若读取过程中发生 I/O 错误（如 getchar 返回 EOF 且 ferror(stdin)
 * 为真）， 返回 -1 并设置 errno = EIO。
 *
 *         若读取的总字符数超过 SSIZE_MAX，返回 -1 并设置 errno = EFBIG。
 *
 * @note
 * - 换行符 '\n' 会被当作普通字符读取并存入缓冲区（除非被截断）。
 *
 * - 读取到 EOF 后，输入流状态被置为结束，后续调用将立即返回 0。
 *
 * - 若输入过长，被截断的字符不会被保留在输入流中（已消耗）。
 *
 * @warning dst_buf_size 必须至少为 1，否则无法存放 '\0'。
 *
 * @warning 调用者应确保 dst 指向有效的内存区域。
 */
ssize_t str_read(char *dst, size_t dst_buf_size);

/**
 * @brief 将源字符串复制到目标缓冲区，支持截断安全。
 *
 * @param src           源字符串（不必以 '\0' 结尾，受 src_buf_size 限制）
 * @param src_buf_size  源缓冲区的物理大小（字节数）
 * @param dst           目标缓冲区
 * @param dst_buf_size  目标缓冲区的物理大小（必须 > 0）
 *
 * @return 成功时返回源字符串所需的总长度（不含 '\0'）；
 *
 *         若返回值 >= dst_buf_size，则发生截断；
 *
 *         若参数无效（NULL 或 dst_buf_size==0），返回 -1 并设置 errno =
 * EINVAL。
 *
 * @note 若 src 与 dst 重叠，函数内部使用 memmove 安全处理。
 *
 * @warning dst_buf_size 必须至少为 1，否则无法存放 '\0'。
 */
ssize_t str_copy(const char *src, size_t src_buf_size, char *dst,
                 size_t dst_buf_size);

/**
 * @brief 从标准输入（stdin）读取字符，直到遇到 EOF，并复制到目标缓冲区。
 *
 * 该函数持续调用 getchar() 读取字符，直至文件结束（EOF），
 * 并将读取的字符依次存入 dst。如果输入长度超过 dst_buf_size - 1，
 * 则只存储前 dst_buf_size - 1
 * 个字符，后续读取的字符被丢弃（不会影响后续调用）。
 *
 * 目标缓冲区始终以 '\0' 结尾。
 *
 * @param dst           目标缓冲区（不必预先初始化）
 * @param dst_buf_size  目标缓冲区的物理大小（字节数），必须 > 0
 *
 * @return 成功时返回从输入流中读取的总字符数（不含结尾的 '\0'）：
 *
 *         - 若返回值 < dst_buf_size，表示完整读取。
 *
 *         - 若返回值 >= dst_buf_size，表示发生截断（输入过长），
 *           此时 dst 中存放前 (dst_buf_size - 1) 个字符，且以 '\0' 结尾。
 *
 *         若 dst 为 NULL 或 dst_buf_size == 0，返回 -1 并设置 errno = EINVAL。
 *
 *         若读取过程中发生 I/O 错误（如 getchar 返回 EOF 且 ferror(stdin)
 * 为真）， 返回 -1 并设置 errno = EIO。
 *
 *         若读取的总字符数超过 SSIZE_MAX，返回 -1 并设置 errno = EFBIG。
 *
 * @note
 * - 换行符 '\n' 会被当作普通字符读取并存入缓冲区（除非被截断）。
 *
 * - 读取到 EOF 后，输入流状态被置为结束，后续调用将立即返回 0。
 *
 * - 若输入过长，被截断的字符不会被保留在输入流中（已消耗）。
 *
 * @warning dst_buf_size 必须至少为 1，否则无法存放 '\0'。
 *
 * @warning 调用者应确保 dst 指向有效的内存区域。
 */
ssize_t str_reverse(const char *src, size_t src_buf_size, char *dst,
                    size_t dst_buf_size);

/**
 * @brief 将源字符串中的制表符（'\t'）展开为空格，并写入目标缓冲区。
 *
 * 该函数将 src 中的每个制表符替换为适当数量的空格，使得展开后的字符位置
 * 在视觉上与原始制表符对齐（基于列号和制表位宽度）。展开规则为：
 *
 * - 当前列号从 0 开始计数。
 *
 * - 遇到制表符时，计算需要填充的空格数为 `tab_width - (col % tab_width)`。
 *
 * - 换行符（'\n'）将当前列号重置为 0。
 *
 * - 普通字符（包括换行符）占一列。
 *
 * @param src           源字符数组（不必以 '\0' 结尾，受 src_buf_size 限制）
 * @param src_buf_size  源缓冲区的物理大小（字节数）
 * @param dst           目标缓冲区
 * @param dst_buf_size  目标缓冲区的物理大小（字节数）。若 dst 为
 *                      NULL，则此参数必须为 0；若 dst 非空，则必须 > 0
 * @param tab_width     制表符宽度（列数），必须 > 0（例如 4 或 8）
 *
 * @return 成功时返回展开后所需的总长度（不含结尾的 '\0'）：
 *
 *         - 若返回值 < dst_buf_size，表示完整展开并写入。
 *
 *         - 若返回值 >= dst_buf_size，表示发生截断，dst 中仅存放前
 *         (dst_buf_size-1) 个字符。
 *
 *         若参数无效（src 为 NULL，或 dst 非空但 dst_buf_size == 0，或
 *         tab_width <= 0），返回 -1 并设置 errno = EINVAL。
 *
 *         若 src 与 dst 存在重叠（包括完全重叠和部分重叠），返回 -1 并设置
 *         errno = EINVAL。 若展开后所需长度超过 SSIZE_MAX，返回 -1 并设置 errno
 * = EFBIG。
 *
 *         若 dst == NULL 且 dst_buf_size ==
 *         0，函数仅计算所需长度，不执行任何写入操作，直接返回该长度（无错误）。
 *
 * @note
 * - 该函数采用两遍扫描：第一遍计算展开后的总长度并检测重叠；
 *   第二遍实际写入数据。因此即使 src 不包含 '\0'，也能安全处理。
 *
 * - 若 src 与 dst 指向同一缓冲区（或部分重叠），函数会检测并拒绝，
 *   因为展开是长度增加操作，正向写入必然覆盖尚未读取的源数据。
 *
 * - 截断时，dst 依然保证以 '\0' 结尾，且不产生缓冲区溢出。
 *
 * - 制表符展开后的总长度一定 >= 源长度（仅当 tab_width >= 2 时通常如此）。
 *
 * - 该函数支持“仅计算长度”模式：传入 dst = NULL, dst_buf_size = 0
 *   时，函数不进行写入和重叠检测，直接返回所需长度，可用于调用者预先分配缓冲区。
 *
 * @warning 若 dst 非空，则 dst_buf_size 必须至少为 1，否则无法存放结尾的
 *          '\0'。若 dst == NULL，则 dst_buf_size 必须为 0。
 *
 * @warning tab_width 必须为正整数，传入 0 或负数将导致 EINVAL 错误。
 *
 * @warning 调用者应确保 src 和 dst 指向有效的内存区域，且不重叠。
 */
ssize_t str_detab(const char *src, size_t src_buf_size, char *dst,
                  size_t dst_buf_size, int tab_width);

/**
 * @brief 将源字符串中的连续空格压缩为制表符和空格，并写入目标缓冲区。
 *
 * 该函数将 src 中的连续空格序列尽可能用制表符（'\t'）替换，同时保留
 * 必要的空格，以保证转换前后文本的列对齐位置不变。压缩策略为：
 *
 * - 从当前列偏移开始，计算连续空格覆盖的总列数。
 *
 * - 尽可能多地使用制表符（每个制表符占据 tab_width 列）。
 *
 * - 剩余不足一个制表位的空格保留为普通空格。
 *
 * - 制表符（'\t'）和换行符（'\n'）会重置列偏移为 0。
 *
 * @param src           源字符数组（不必以 '\0' 结尾，受 src_buf_size 限制）
 * @param src_buf_size  源缓冲区的物理大小（字节数）
 * @param dst           目标缓冲区
 * @param dst_buf_size  目标缓冲区的物理大小（字节数）。若 dst 为
 *                      NULL，则此参数必须为 0；若 dst 非空，则必须 > 0
 * @param tab_width     制表符宽度（列数），必须 > 0（例如 4 或 8）
 *
 * @return 成功时返回压缩后所需的总长度（不含结尾的 '\0'）：
 *
 *         - 若返回值 < dst_buf_size，表示完整压缩并写入。
 *
 *         - 若返回值 >= dst_buf_size，表示发生截断，dst 中仅存放
 *           前 (dst_buf_size - 1) 个字符。
 *
 *         若参数无效（src 为 NULL，或 dst 非空但 dst_buf_size == 0，或
 *         tab_width <= 0），返回 -1 并设置 errno = EINVAL。
 *
 *         若压缩后所需长度超过 SSIZE_MAX，返回 -1 并设置 errno = EFBIG。
 *
 *         若 dst == NULL 且 dst_buf_size ==
 *         0，函数仅计算所需长度，不执行任何写入操作，直接返回该长度（无错误）。
 *
 * @note
 * - 该函数采用两遍扫描：第一遍计算压缩后的总长度，第二遍实际写入。
 *
 * - 由于压缩操作不会使字符串变长（总字符数 ≤ 源长度），因此允许
 *   src 和 dst 指向相同或部分重叠的内存区域（例如原地压缩）。
 *
 * - 若目标缓冲区不足以容纳全部压缩结果，函数会截断并安全终止，
 *   调用者可通过返回值判断是否发生截断。
 *
 * - 制表符和换行符会重置列偏移为 0，从而影响后续空格的压缩决策。
 *
 * - 压缩后的总长度总是 ≤ src_len（仅当 tab_width >= 2 时通常如此）。
 *
 * - 该函数支持“仅计算长度”模式：传入 dst = NULL, dst_buf_size = 0
 *   时，函数不进行写入和重叠检测，直接返回所需长度，可用于调用者预先分配缓冲区。
 *
 * @warning 若 dst 非空，则 dst_buf_size 必须至少为 1，否则无法存放结尾的
 *          '\0'。若 dst == NULL，则 dst_buf_size 必须为 0。
 *
 * @warning tab_width 必须为正整数，传入 0 或负数将导致 EINVAL 错误。
 *
 * @warning 调用者应确保 src 和 dst 指向有效的内存区域；虽然函数允许重叠，
 *          但 dst 缓冲区必须足够容纳完整压缩结果（或截断后的前缀）。
 */
ssize_t str_entab(const char *src, size_t src_buf_size, char *dst,
                  size_t dst_buf_size, int tab_width);

/**
 * @brief 将源字符串中的连续水平空白字符（空格和制表符）折叠为单个空格。
 *
 * 该函数扫描源字符串 src，将其中任意长度的连续空格（' '）和制表符（'\t'）
 * 序列压缩为一个空格字符（' '）。其他字符（包括换行符 '\n'、回车符 '\r' 等）
 * 保持不变，并按原样复制到目标缓冲区。
 *
 * 折叠策略：
 *
 * - 连续的 ' ' 和 '\t' 组合被视为一个空白序列。
 *
 * - 该序列被替换为单个空格。
 *
 * - 非空白字符（包括换行符）不会触发折叠，且会重置空白序列计数。
 *
 * @param src           源字符数组（不必以 '\0' 结尾，受 src_buf_size 限制）
 * @param src_buf_size  源缓冲区的物理大小（字节数）
 * @param dst           目标缓冲区
 * @param dst_buf_size  目标缓冲区的物理大小（字节数）。若 dst 为
 *                      NULL，则此参数必须为 0；若 dst 非空，则必须 > 0
 *
 * @return 成功时返回折叠后所需的总长度（不含结尾的 '\0'）：
 *
 *         - 若返回值 < dst_buf_size，表示完整折叠并写入。
 *
 *         - 若返回值 >= dst_buf_size，表示发生截断，dst 中仅存放
 *           前 (dst_buf_size - 1) 个字符。
 *
 *         若参数无效（src 为 NULL，或 dst 非空但 dst_buf_size == 0，或
 *         tab_width <= 0），返回 -1 并设置 errno = EINVAL。
 *
 *         若折叠后所需长度超过 SSIZE_MAX，返回 -1 并设置 errno = EFBIG。
 *
 *         若 dst == NULL 且 dst_buf_size ==
 *         0，函数仅计算所需长度，不执行任何写入操作，直接返回该长度（无错误）。
 *
 * @note
 * - 该函数采用两遍扫描：第一遍计算折叠后的长度，第二遍实际写入。
 *
 * - 折叠操作不会改变字符串的总列数语义（仅压缩空白），但会减少字符数。
 *
 * - 换行符（'\n'）不会被视为空白，因此不会被折叠或删除。
 *
 * - 如果目标缓冲区不足，函数会安全截断，且保证 dst 以 '\0' 结尾。
 *
 * - 该函数允许 src 与 dst 指向相同或重叠的内存区域（原地操作安全），
 *   因为折叠操作是长度缩减的，写入指针不会超过读取指针。
 *
 * - 该函数支持“仅计算长度”模式：传入 dst = NULL, dst_buf_size = 0
 *   时，函数不进行写入和重叠检测，直接返回所需长度，可用于调用者预先分配缓冲区。
 *
 * @warning 若 dst 非空，则 dst_buf_size 必须至少为 1，否则无法存放结尾的
 *          '\0'。若 dst == NULL，则 dst_buf_size 必须为 0。
 *
 * @warning 调用者应确保 src 和 dst 指向有效的内存区域。
 */
ssize_t str_collapse_blank(const char *src, size_t src_buf_size, char *dst,
                           size_t dst_buf_size);

/**
 * @brief 将源字符串按指定列宽智能折行，输出到目标缓冲区。
 *
 * 该函数扫描源字符串 src，将其按 col_lim 列宽限制折行，尽量在空白处断行，
 * 以保证每行长度不超过 col_lim。
 *
 * 折行策略：
 *
 * - 每行从第 0 列开始计数，制表符按 tab_width 展开计算视觉宽度。
 *
 * - 当列数达到 col_lim 时，若当前行存在空白符串，则在最近一段连续空白的
 *   起始处折行，并丢弃该空白串（用换行符替代）。
 *
 * - 若当前行没有空白符（即超长单词），则在当前字符处强制断行，单词被拆分。
 *
 * - 源字符串中已有的换行符（'\n'）会保留，并重置列计数。
 *
 * @param src           源字符数组（不必以 '\0' 结尾，受 src_buf_size 限制）
 * @param src_buf_size  源缓冲区的物理大小（字节数）
 * @param dst           目标缓冲区。若为 NULL，则必须同时设置 dst_buf_size = 0，
 *                      此时函数仅计算所需长度，不进行写入
 * @param dst_buf_size  目标缓冲区的物理大小（字节数）。若 dst 非空，则必须 >
 * 0； 若 dst 为 NULL，则必须为 0
 * @param tab_width     制表符宽度（列数），必须 > 0，且必须 ≤ col_lim
 * @param col_lim       每行最大列数，必须 > 0，且必须 ≥ tab_width
 *
 * @return 成功时返回折行后所需的总长度（不含结尾的 '\0'）：
 *
 *         - 若 dst == NULL 且 dst_buf_size == 0，仅计算长度并返回，不写入。
 *
 *         - 若 dst 非空且返回值 < dst_buf_size，表示完整折行并写入。
 *
 *         - 若 dst 非空且返回值 >= dst_buf_size，表示发生截断，dst 中仅存放
 *           前 (dst_buf_size - 1) 个字符（折行逻辑可能不完整，但保证安全）。
 *
 *         若参数无效（src 为 NULL，或 dst 非空但 dst_buf_size == 0，
 *         或 tab_width <= 0，或 col_lim <= 0，或 tab_width > col_lim），
 *         返回 -1 并设置 errno = EINVAL。
 *
 *         若折行后所需长度超过 SSIZE_MAX，返回 -1 并设置 errno = EFBIG。
 *
 * @note
 * - 该函数采用两遍扫描：第一遍计算折行后的总长度，第二遍实际写入。
 *
 * - 折行后长度可能大于、等于或小于源长度：插入换行符会增加长度，丢弃空白会
 *   减少长度，因此最终长度取决于具体内容。
 *
 * - 换行符（'\n'）不会被折叠或删除，它会重置列计数并保留在输出中。
 *
 * - 该函数**不支持** src 与 dst 重叠：若两者指向同一缓冲区或部分重叠，
 *   行为未定义。调用者必须保证两者不重叠。
 *
 * - 若目标缓冲区不足以容纳完整折行结果，函数会截断写入（通过 WRITE_DST 宏
 *   保护），但返回值仍为完整所需长度，调用者可据此重新分配缓冲区。
 *
 * @warning dst_buf_size 应至少为返回值 + 1，以确保完整写入。若过小，输出可能
 *          被截断，但不会发生缓冲区溢出。
 *
 * @warning src 和 dst 不得重叠，否则行为未定义。
 *
 * @warning tab_width 必须 ≤ col_lim，否则无法将制表符放入任何一行。
 */
ssize_t str_wrap(const char *src, size_t src_buf_size, char *dst,
                 size_t dst_buf_size, int tab_width, int col_limit);

/**
 * @brief 将单个十六进制字符转换为 long long。
 *
 * @param ch 源字符
 *
 * @return 转换后的值；失败返回 -1 并设置 errno。
 */
signed char str_hdtoi(char ch);

/**
 * @brief 将十六进制字符串转换为 long long。
 *
 * @param src           源字符串（可带 "0x" 或 "0X" 前缀）
 * @param src_buf_size  源缓冲区的物理大小
 *
 * @return 转换后的值；失败返回 -1 并设置 errno。
 */
long long str_htoi(const char *src, size_t src_buf_size);

ssize_t str_squeeze(const char *src, size_t src_buf_size, char *dst,
                    size_t dst_buf_size, char ch);

#ifdef __cplusplus
}
#endif

#endif /* STRING_UTILS_H */