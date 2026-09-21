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

static inline void entab_write_compressed(char *dst, size_t *dst_i,
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

ssize_t str_read(char *dst, size_t dst_buf_size) {
    CHECK_DST_REQUIRED(dst, dst_buf_size);

    size_t dst_len = 0;

    int ch;
    while (dst_len < dst_buf_size - 1 && (ch = getchar()) != EOF) {
        dst[dst_len++] = (char)ch;
    }
    dst[dst_len] = '\0';

    while ((ch = getchar()) != EOF) {
        dst_len++;
    }

    CHECK_LEN_OVERFLOW(dst_len);

    return (ssize_t)dst_len;
}

ssize_t str_copy(const char *src, size_t src_buf_size, char *dst,
                 size_t dst_buf_size) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_REQUIRED(dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);
    const size_t dst_len = MIN(src_len, dst_buf_size - 1);

    if (dst_len > 0) {
        memmove(dst, src, dst_len);
    }
    dst[dst_len] = '\0';

    CHECK_LEN_OVERFLOW(dst_len);

    return (ssize_t)src_len;
}

ssize_t str_reverse(const char *src, size_t src_buf_size, char *dst,
                    size_t dst_buf_size) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_REQUIRED(dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);
    const size_t dst_len = MIN(src_len, dst_buf_size - 1);

    CHECK_REVERSE_OVERLAP(src, src_len, dst);

    for (size_t i = 0; i < dst_len; i++) {
        dst[i] = src[src_len - i - 1];
    }

    dst[dst_len] = '\0';

    CHECK_LEN_OVERFLOW(src_len);

    return (ssize_t)src_len;
}

ssize_t str_detab(const char *src, size_t src_buf_size, char *dst,
                  size_t dst_buf_size, int tab_width) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_OPTIONAL(dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t result_len = 0;

    unsigned int tab_offset = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '\t') {
            result_len += tab_width - tab_offset;
            tab_offset = 0;
        } else {
            result_len++;
            tab_offset =
                src[i] == '\n' ? 0 : advance_tab_offset(tab_offset, tab_width);
        }
    }

    CHECK_LEN_OVERFLOW(result_len);

    RETURN_LEN_IF_NO_DST(dst, dst_buf_size, result_len);

    CHECK_NO_OVERLAP(src, src_len, dst, result_len);

    size_t dst_i = 0;
    tab_offset = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == '\t') {
            for (unsigned int s = 0; s < tab_width - tab_offset; s++) {
                if (dst_i < dst_buf_size - 1) {
                    WRITE_DST(' ');
                }
            }
            tab_offset = 0;
        } else {
            if (dst_i < dst_buf_size - 1) {
                WRITE_DST(src[i]);
            }
            tab_offset =
                src[i] == '\n' ? 0 : advance_tab_offset(tab_offset, tab_width);
        }
    }
    dst[dst_i] = '\0';

    return (ssize_t)result_len;
}

ssize_t str_entab(const char *src, size_t src_buf_size, char *dst,
                  size_t dst_buf_size, int tab_width) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_OPTIONAL(dst, dst_buf_size);
    CHECK_POSITIVE_PARAMS(tab_width);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t result_len = 0;

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
                result_len +=
                    entab_compressed_len(start_offset, space_run, tab_width);
                space_run = 0;
            }

            // 再处理当前的字符 src[i]
            tab_offset = (src[i] == '\t' || src[i] == '\n')
                             ? 0
                             : advance_tab_offset(tab_offset, tab_width);
        }
    }
    result_len += space_run > 0
                      ? entab_compressed_len(start_offset, space_run, tab_width)
                      : 0;

    CHECK_LEN_OVERFLOW(result_len);

    RETURN_LEN_IF_NO_DST(dst, dst_buf_size, result_len);

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

            WRITE_DST(src[i]);
            tab_offset = (src[i] == '\t' || src[i] == '\n')
                             ? 0
                             : advance_tab_offset(tab_offset, tab_width);
        }
    }
    if (space_run > 0) {
        entab_write_compressed(dst, &dst_i, dst_buf_size - 1, start_offset,
                               space_run, tab_width);
    }
    dst[dst_i] = '\0';

    return (ssize_t)result_len;
}

ssize_t str_collapse_blank(const char *src, size_t src_buf_size, char *dst,
                           size_t dst_buf_size) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_OPTIONAL(dst, dst_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);

    size_t result_len = 0;

    unsigned int blank_run = 0;
    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == ' ' || src[i] == '\t') {
            if (blank_run == 0) {
                result_len++;
            }
            blank_run++;
        } else {
            blank_run = 0;
            result_len++;
        }
    }

    CHECK_LEN_OVERFLOW(result_len);

    RETURN_LEN_IF_NO_DST(dst, dst_buf_size, result_len);

    size_t dst_i = 0;
    blank_run = 0;
    for (size_t i = 0; i < src_len && dst_i < dst_buf_size - 1; i++) {
        if (src[i] == ' ' || src[i] == '\t') {
            if (blank_run == 0) {
                WRITE_DST(' ');
            }
            blank_run++;
        } else {
            blank_run = 0;
            WRITE_DST(src[i]);
        }
    }
    dst[dst_i] = '\0';

    return (ssize_t)result_len;
}

ssize_t str_wrap(const char *src, size_t src_buf_size, char *dst,
                 size_t dst_buf_size, int tab_width, int col_lim) {
    CHECK_SRC_NOT_NULL(src);
    CHECK_DST_OPTIONAL(dst, dst_buf_size);
    CHECK_POSITIVE_PARAMS(tab_width, col_lim);
    CHECK_TAB_WIDTH_LE_COL_LIM(tab_width, col_lim);

    const size_t src_len = strnlen(src, src_buf_size);

    ssize_t delta = 0;
    int col = 0; // == -1 时正在折行
    size_t line_start_i = 0;
    ssize_t whitespace_start_i = -1;  // == -1 时当前行暂无空白符
    ssize_t whitespace_end_i = -1;    // 同上
    int whitespace_end_col_next = -1; // 同上
    for (size_t i = 0; i < src_len; i++) {
        switch (src[i]) {
        case ' ':
        case '\t':
            if (col != -1) { // 如果不在折行
                // 如果当前行暂无空白符，或当前空白符与之前的空白符不紧邻
                if (whitespace_start_i == -1 ||
                    i > (size_t)whitespace_end_i + 1) {
                    // 最近空白符串从此开始
                    whitespace_start_i = i;
                }
                // 无论如何当前空白符是目前最后一个空白符
                whitespace_end_i = i;
                // 按空白符类型计算当前空白符串的最后一列的下一列（和 col
                // 一样，永远为下一轮循环做准备）
                whitespace_end_col_next = col +=
                    src[i] == ' ' ? 1 : tab_width - col % tab_width;
            } else { // 如果正在折行
                // 丢弃折行后新行首的空白符
                delta--;
            }
            break;
        case '\n':
            if (col != -1) { // 如果不在折行但遇到了换行符
                if (whitespace_end_i == (ssize_t)i - 1) {
                    delta -= whitespace_end_i - whitespace_start_i + 1;
                }
                // 新行暂无空白符
                whitespace_start_i = whitespace_end_i =
                    whitespace_end_col_next = -1;
            } else { // 如果正在折行而遇到了换行符
                // 既然折行时增加了一个换行符，那么固有的换行符直接丢弃
                delta--;
            }
            // 既然是固有的换行符，那么无论如何都结束折行，栏数从 0 开始
            col = 0;
            line_start_i = i + 1;
            break;
        // 如果当前为普通字符
        default:
            // 如果不在折行，那么栏数自增；如果正在折行，那么结束折行，栏数变为
            // 0 ，随后计算当前普通字符，栏数变为 1
            col = col != -1 ? col + 1 : 1;
            break;
        }

        // 如果栏数超出限制，且当前字符不是 src 中的最后一个，那么开始折行
        if ((col > (int)col_lim ||
             (col == (int)col_lim && i + 1 < src_len && src[i + 1] != ' ' &&
              src[i + 1] != '\t' && src[i + 1] != '\n')) &&
            i + 1 < src_len &&
            (whitespace_start_i == -1 ||
             whitespace_start_i > (ssize_t)line_start_i)) {
            if (whitespace_start_i != -1 &&
                whitespace_start_i > (ssize_t)line_start_i) {
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
            } else { // 如果当前行没有空白符串
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
    delta -= col != -1 && whitespace_end_i == (ssize_t)src_len - 1
                 ? whitespace_end_i - whitespace_start_i + 1
                 : 0;

    size_t result_len = src_len + delta;

    CHECK_LEN_OVERFLOW(result_len);

    RETURN_LEN_IF_NO_DST(dst, dst_buf_size, result_len);

    size_t dst_i = 0;
    col = 0;
    line_start_i = 0;
    whitespace_start_i = -1;
    whitespace_end_i = -1;
    whitespace_end_col_next = -1;
    for (size_t i = 0; i < src_len; i++) {
        switch (src[i]) {
        case ' ':
        case '\t':
            if (col != -1) { // 如果不在折行
                // 如果当前行暂无空白符，或当前空白符与之前的空白符不紧邻
                if (whitespace_start_i == -1 ||
                    i > (size_t)whitespace_end_i + 1) {
                    // 最近空白符串从此开始
                    whitespace_start_i = i;
                }
                // 无论如何当前空白符是目前最后一个空白符
                whitespace_end_i = i;
                // 无论如何当前空白符应当写入 dst
                WRITE_DST(src[i]);
                // 按空白符类型计算当前空白符串的最后一列的下一列（和 col
                // 一样，永远为下一轮循环做准备）
                whitespace_end_col_next = col +=
                    src[i] == ' ' ? 1 : tab_width - col % tab_width;
            } // 如果正在折行，那么丢弃折行后新行首的空白符，即不作写入处理
            break;
        case '\n':
            if (col != -1) { // 如果不在折行但遇到了换行符
                if (whitespace_end_i == (ssize_t)i - 1) {
                    dst_i -= i - whitespace_start_i;
                }
                // 将当前换行符写入 dst
                WRITE_DST(src[i]);
                // 新行暂无空白符
                whitespace_start_i = whitespace_end_i =
                    whitespace_end_col_next = -1;
            } // 如果正在折行而遇到了换行符，既然折行时增加了一个换行符，那么固有的换行符直接丢弃，即不作写入处理
            // 既然是固有的换行符，那么无论如何都结束折行，栏数从 0 开始
            col = 0;
            line_start_i = i + 1;
            break;
        // 如果当前为普通字符
        default:
            // 写入 dst
            WRITE_DST(src[i]);
            // 如果不在折行，那么栏数自增；如果正在折行，那么结束折行，栏数变为
            // 0 ，随后计算当前普通字符，栏数变为 1
            col = col != -1 ? col + 1 : 1;
            break;
        }

        // 如果栏数超出限制，且当前字符不是 src 中的最后一个，那么开始折行
        if ((col > (int)col_lim ||
             (col == (int)col_lim && i + 1 < src_len && src[i + 1] != ' ' &&
              src[i + 1] != '\t' && src[i + 1] != '\n')) &&
            i + 1 < src_len &&
            (whitespace_start_i == -1 ||
             whitespace_start_i > (ssize_t)line_start_i)) {
            if (whitespace_start_i != -1 &&
                whitespace_start_i > (ssize_t)line_start_i) {
                // 倒退 dst_i 光标
                dst_i -= i - whitespace_start_i + 1;
                // 写入换行符
                WRITE_DST('\n');
                // 写入折行处之后的新行字符
                for (size_t j = whitespace_end_i + 1; j <= i; j++) {
                    WRITE_DST(src[j]);
                }
                // 计算新行栏数
                if ((ssize_t)i == whitespace_end_i) {
                    // 当前字符是空白，整个空白串被丢弃，新行无字符
                    col = -1;
                } else {
                    // 当前字符是普通字符，新行已有该字符
                    col = col - whitespace_end_col_next;
                }
            } else { // 如果当前行没有空白符串
                // 写入换行符强行折断单词（也可能是紧贴单词尾折断）
                WRITE_DST('\n');
                // 设置折行状态
                col = -1;
            }
            // 新行暂无空白符
            whitespace_start_i = whitespace_end_i = whitespace_end_col_next =
                -1;
        }
    }
    dst_i -= col != -1 && whitespace_end_i == (ssize_t)src_len - 1
                 ? src_len - whitespace_start_i
                 : 0;
    dst[MIN(dst_i, dst_buf_size - 1)] = '\0';

    return (ssize_t)result_len;
}

signed char str_hdtoi(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    } else if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    } else {
        errno = EINVAL;
        return -1;
    }
}

long long str_htoi(const char *src, size_t src_buf_size) {
    CHECK_DST_REQUIRED(src, src_buf_size);

    const size_t src_len = strnlen(src, src_buf_size);

    if (src_len == 0) {
        errno = EINVAL;
        return -1;
    }

    size_t i = src_len >= 2 && src[0] == '0' && (src[1] == 'x' || src[1] == 'X')
                   ? 2
                   : 0;

    if (i == src_len) {
        errno = EINVAL;
        return -1;
    }

    long long lld = 0;

    signed char digit;
    for (; i < src_len; i++) {
        digit = str_hdtoi(src[i]);
        if (digit == -1) {
            return -1;
        }

        if (lld > (LLONG_MAX - digit) / 16) {
            errno = ERANGE;
            return -1;
        }

        lld = lld * 16 + digit;
    }

    return lld;
}

ssize_t str_squeeze(const char *src, size_t src_buf_size, char *dst,
                    size_t dst_buf_size, char ch) {
    
}