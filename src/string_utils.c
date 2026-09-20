#include "../include/string_utils.h"

static inline unsigned int advance_tab_offset(unsigned int offset,
                                              int tab_width) {
    return (offset + 1) % tab_width;
}

static inline size_t entab_compressed_len(unsigned int start_offset,
                                          size_t space_run, int tab_width) {
    if (space_run == 0) {
        return 0;
    }
    size_t total_cols = start_offset + space_run;
    return total_cols / tab_width + total_cols % tab_width;
}

static inline void entab_write_compressed(char dst[], size_t *dst_i,
                                          size_t max_write,
                                          unsigned int start_offset,
                                          size_t space_run, int tab_width) {
    size_t total_cols = start_offset + space_run;
    size_t tabs = total_cols / tab_width;
    size_t spaces = total_cols % tab_width;

    for (size_t j = 0; j < tabs && *dst_i < max_write; j++) {
        dst[(*dst_i)++] = '\t';
    }
    for (size_t j = 0; j < spaces && *dst_i < max_write; j++) {
        dst[(*dst_i)++] = ' ';
    }
}

ssize_t str_read(char dst[], size_t dst_buf_size) {
    CHECK_READ_PARAMS(dst, dst_buf_size);

    int ch;
    size_t dst_len = 0;

    while (dst_len < dst_buf_size - 1 && (ch = getchar()) != EOF) {
        dst[dst_len++] = (char)ch;
    }
    dst[dst_len] = '\0';
    while ((ch = getchar()) != EOF) {
        dst_len++;
    }

    if (dst_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }

    return (ssize_t)dst_len;
}

ssize_t str_copy(const char src[], size_t src_buf_size, char dst[],
                 size_t dst_buf_size) {
    CHECK_PRESERVE_PARAMS(src, dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);
    const size_t dst_len = MIN(src_len, dst_buf_size - 1);

    if (dst_len > 0) {
        memmove(dst, src, dst_len);
    }
    dst[dst_len] = '\0';

    if (src_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }
    return (ssize_t)src_len;
}

ssize_t str_reverse(const char src[], size_t src_buf_size, char dst[],
                    size_t dst_buf_size) {
    CHECK_PRESERVE_PARAMS(src, dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);
    const size_t dst_len = MIN(src_len, dst_buf_size - 1);

    if (src_len > 0 && dst >= src && dst < src + src_len) {
        errno = EINVAL;
        return -1;
    }

    for (size_t i = 0; i < dst_len; i++) {
        dst[i] = src[src_len - i - 1];
    }

    dst[dst_len] = '\0';

    if (src_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }
    return (ssize_t)src_len;
}

ssize_t str_detab(const char src[], size_t src_buf_size, char dst[],
                  size_t dst_buf_size, int tab_width) {
    CHECK_RESIZE_PARAMS(src, dst, dst_buf_size);
    CHECK_POSITIVE_PARAMS(tab_width);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t expanded_len = 0;
    unsigned int tab_offset = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '\t') {
            expanded_len += tab_width - tab_offset;
            tab_offset = 0;
        } else {
            expanded_len++;
            if (src[i] == '\n') {
                tab_offset = 0;
            } else {
                tab_offset = advance_tab_offset(tab_offset, tab_width);
            }
        }
    }

    if (expanded_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }

    if (dst == NULL || dst_buf_size == 0) {
        return (ssize_t)expanded_len;
    }

    if (src_len > 0) {
        if (dst >= src && dst < src + src_len) {
            errno = EINVAL;
            return -1;
        }
        if (dst < src && dst + expanded_len > src) {
            errno = EINVAL;
            return -1;
        }
    }

    size_t dst_i = 0;
    tab_offset = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '\t') {
            for (unsigned int s = 0; s < tab_width - tab_offset; s++) {
                if (dst_i < dst_buf_size - 1) {
                    dst[dst_i++] = ' ';
                }
            }
            tab_offset = 0;
        } else {
            if (dst_i < dst_buf_size - 1) {
                dst[dst_i++] = src[i];
            }
            if (src[i] == '\n') {
                tab_offset = 0;
            } else {
                tab_offset = advance_tab_offset(tab_offset, tab_width);
            }
        }
    }
    dst[dst_i] = '\0';

    return (ssize_t)expanded_len;
}

ssize_t str_entab(const char src[], size_t src_buf_size, char dst[],
                  size_t dst_buf_size, int tab_width) {
    CHECK_RESIZE_PARAMS(src, dst, dst_buf_size);
    CHECK_POSITIVE_PARAMS(tab_width);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t compressed_len = 0;
    unsigned int tab_offset = 0;
    unsigned int space_run = 0;
    size_t start_offset = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == ' ') {
            if (space_run == 0) {
                start_offset = tab_offset;
            }
            tab_offset = advance_tab_offset(tab_offset, tab_width);
            space_run++;
        } else {
            // 先处理之前的连续 ' ' 串（如果有）
            if (space_run > 0) {
                compressed_len +=
                    entab_compressed_len(start_offset, space_run, tab_width);
                space_run = 0;
            }

            // 再处理当前的字符 src[i]
            compressed_len++;
            if (src[i] == '\t' || src[i] == '\n') {
                tab_offset = 0;
            } else {
                tab_offset = advance_tab_offset(tab_offset, tab_width);
            }
        }
    }
    if (space_run > 0) {
        compressed_len +=
            entab_compressed_len(start_offset, space_run, tab_width);
    }

    if (compressed_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }

    if (dst == NULL || dst_buf_size == 0) {
        return (ssize_t)compressed_len;
    }

    size_t dst_i = 0;
    tab_offset = 0;
    space_run = 0;
    start_offset = 0;
    for (size_t i = 0; i < src_len && dst_i < dst_buf_size - 1; i++) {
        if (src[i] == ' ') {
            if (space_run == 0) {
                start_offset = tab_offset;
            }
            tab_offset = advance_tab_offset(tab_offset, tab_width);
            space_run++;
        } else {
            if (space_run > 0) {
                entab_write_compressed(dst, &dst_i, dst_buf_size - 1,
                                       start_offset, space_run, tab_width);
                space_run = 0;
            }

            dst[dst_i++] = src[i];
            if (src[i] == '\t' || src[i] == '\n') {
                tab_offset = 0;
            } else {
                tab_offset = advance_tab_offset(tab_offset, tab_width);
            }
        }
    }
    if (space_run > 0) {
        entab_write_compressed(dst, &dst_i, dst_buf_size - 1, start_offset,
                               space_run, tab_width);
    }
    dst[dst_i] = '\0';

    return (ssize_t)compressed_len;
}

ssize_t str_collapse_blank(const char src[], size_t src_buf_size, char dst[],
                           size_t dst_buf_size) {
    CHECK_RESIZE_PARAMS(src, dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t collapsed_len = 0;
    unsigned int blank_run = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == ' ' || src[i] == '\t') {
            if (blank_run == 0) {
                collapsed_len++;
            }
            blank_run++;
        } else {
            blank_run = 0;
            collapsed_len++;
        }
    }

    if (collapsed_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }

    if (dst == NULL || dst_buf_size == 0) {
        return (ssize_t)collapsed_len;
    }

    size_t dst_i = 0;
    blank_run = 0;
    for (size_t i = 0; i < src_len && dst_i < dst_buf_size - 1; i++) {
        if (src[i] == ' ' || src[i] == '\t') {
            if (blank_run == 0) {
                dst[dst_i++] = ' ';
            }
            blank_run++;
        } else {
            blank_run = 0;
            dst[dst_i++] = src[i];
        }
    }
    dst[dst_i] = '\0';

    return (ssize_t)collapsed_len;
}

ssize_t str_wrap(const char src[], size_t src_buf_size, char dst[],
                 size_t dst_buf_size, int tab_width, size_t col_lim) {
    CHECK_RESIZE_PARAMS(src, dst, dst_buf_size);
    CHECK_POSITIVE_PARAMS(tab_width);
    if (tab_width > col_lim) {
        errno = EINVAL;
        return -1;
    }

    const size_t src_len = strnlen(src, src_buf_size);

    int delta = 0;

    int col = 0; // == -1 时正在折行

    ssize_t whitespace_start_i = -1;  // == -1 时当前行暂无空白符
    ssize_t whitespace_end_i = -1;    // 同上
    int whitespace_end_col_next = -1; // 同上

    for (size_t i = 0; i < src_len; i++) {
        switch (src[i]) {
        case ' ':
        case '\t':
            // 如果不在折行
            if (col != -1) {
                // 如果当前行暂无空白符，或当前空白符与之前的空白符不紧邻
                if (whitespace_start_i == -1 || i > whitespace_end_i + 1) {
                    // 最近空白符串从此开始
                    whitespace_start_i = i;
                }
                // 无论如何当前空白符是目前最后一个空白符
                whitespace_end_i = i;
                // 按空白符类型计算当前空白符串的最后一列的下一列（和 col
                // 一样，永远为下一轮循环做准备）
                whitespace_end_col_next = col +=
                    src[i] == ' ' ? 1 : tab_width - col % tab_width;
                // 如果正在折行
            } else {
                // 丢弃折行后新行首的空白符
                delta--;
            }
            break;

        case '\n':
            // 如果不在折行但遇到了换行符
            if (col != -1) {
                // 新行暂无空白符
                whitespace_start_i = whitespace_end_i =
                    whitespace_end_col_next = -1;
                // 如果正在折行而遇到了换行符
            } else {
                // 既然折行时增加了一个换行符，那么固有的换行符直接丢弃
                delta--;
            }
            // 既然是固有的换行符，那么无论如何都结束折行，栏数从 0 开始
            col = 0;
            break;

        // 如果当前为普通字符
        default:
            // 如果不在折行，那么栏数自增；如果正在折行，那么结束折行，栏数变为
            // 0 ，随后计算当前普通字符，栏数变为 1
            col = col != -1 ? col + 1 : 1;
            break;
        }

        // 如果栏数超出限制，且当前字符不是 src 中的最后一个，那么开始折行
        if (col >= (int)col_lim && i + 1 < src_len) {
            // 如果当前行有空白符串
            if (whitespace_start_i != -1) {
                // 从最近空白符串折行，丢弃整个空白符串
                delta -= whitespace_end_i - whitespace_start_i + 1;
                // 计算折行时用到的换行符
                delta++;
                // 计算新行栏数
                if ((ssize_t)i == whitespace_end_i) {
                    // 当前字符是空白，整个空白串被丢弃，新行无字符
                    col = -1;
                } else {
                    // 当前字符是普通字符，新行已有该字符
                    col = col - whitespace_end_col_next;
                }
                // 如果当前行没有空白符串
            } else {
                // 用换行符强行折断单词（也可能是紧贴单词尾折断，如 'apple| '）
                delta++;
                // 设置折行状态
                col = -1;
            }
            // 新行暂无空白符
            whitespace_start_i = whitespace_end_i = whitespace_end_col_next =
                -1;
        }
    }

    size_t expanded_len = src_len + delta;
    if (expanded_len > (size_t)SSIZE_MAX) {
        errno = EFBIG;
        return -1;
    }

    if (dst == NULL || dst_buf_size == 0) {
        return (ssize_t)expanded_len;
    }

    // declarations

    for (; ; ) {

    }

    dst[dst_i] = '\0';

    return (ssize_t)expanded_len;
}