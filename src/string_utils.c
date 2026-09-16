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
    CHECK_TAB_PARAMS(tab_width);

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
    CHECK_TAB_PARAMS(tab_width);

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
                 size_t dst_buf_size, int tab_width, size_t col_limit) {
    CHECK_RESIZE_PARAMS(src, dst, dst_buf_size);
    CHECK_TAB_PARAMS(tab_width);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t expanded_len = 0; 

    unsigned int col = 0;
    unsigned int tab_offset = 0;

    size_t whitespace_i;
    size_t whitespace_col;
    unsigned int whitespace_run = 0;
    unsigned int whitespace_span = 0;

    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == ' ') {
            if (whitespace_run == 0) {
                whitespace_i = i;
                whitespace_col = col;
            }
            whitespace_run++;
            whitespace_span++;

            col++;
            tab_offset = advance_tab_offset(tab_offset, tab_width);
        } else if (src[i] == '\t') {
            if (whitespace_run == 0) {
                whitespace_i = i;
                whitespace_col = col;
            }
            whitespace_run++;
            whitespace_span += tab_width - tab_offset;

            col += tab_width - tab_offset;
            tab_offset = 0;
        } else if (src[i] == '\n') {
            whitespace_run = 0;
            whitespace_span = 0;

            col = 0;
            tab_offset = 0;
        } else {
            whitespace_run = 0;
            whitespace_span = 0;

            col++;
            tab_offset = advance_tab_offset(tab_offset, tab_width);
        }

        expanded_len++;
        

    }

    // size_t expanded_len = 0;
    // unsigned int col = 0;
    // unsigned int tab_offset = 0;
    // ssize_t whitespace_start_i = -1; // 为 -1 时当前行不存在空白符，截断单词
    // unsigned int whitespace_run = 0;
    // bool is_wrapping = false; // 非折行即原有 '\n' 之后的空白符不应被丢弃
    // for (size_t i = 0; i < src_len; i++) {
    //     if (src[i] == ' ' && !is_wrapping) {
    //         if (whitespace_run == 0) {
    //             whitespace_start_i = i;
    //         }
    //         whitespace_run++;
    //         tab_offset = advance_tab_offset(tab_offset, tab_width);
    //         col++;
    //         expanded_len++;
    //     } else if (src[i] == '\t' && !is_wrapping) {
    //         if (whitespace_run == 0) {
    //             whitespace_start_i = i;
    //         }
    //         whitespace_run++;
    //         col += tab_width - tab_offset;
    //         tab_offset = 0;
    //         expanded_len++;
    //     } else if (src[i] == '\n') {
    //         is_wrapping = false;
    //         whitespace_start_i = -1;
    //         whitespace_run = 0;
    //         tab_offset = 0;
    //         col = 0;
    //         expanded_len++;
    //     } else {
    //         is_wrapping = false;
    //         whitespace_run = 0;
    //         tab_offset = advance_tab_offset(tab_offset, tab_width);
    //         col++;
    //         expanded_len++;
    //     }

    //     if (col >= col_limit) { // 折行
    //         if (whitespace_run > 0) {
    //             expanded_len -= whitespace_run;
    //         } else {
    //             if (whitespace_start_i > -1) {
    //                 expanded_len -= i - whitespace_start_i + 1;
    //                 i = whitespace_start_i - 1; // 移动光标
    //             } // else 折断单词，无事可做
    //         }
    //         expanded_len++; // 折行用 '\n'

    //         is_wrapping = true; // 折行之后的空白符应在下面几轮循环中被丢弃
    //         whitespace_run = 0;
    //         whitespace_start_i = -1;
    //         tab_offset = 0;
    //         col = 0;
    //     } // else 不折行，无事可做
    // }

    // // 第一轮循环结束
}