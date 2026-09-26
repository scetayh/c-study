#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"

#define MAXLEN 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s SOURCE SET1 ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    char dst[MAXLEN];
    strcpy(dst, argv[1]);  // 从源字符串开始

    for (int i = 2; i < argc; i++) {
        ssize_t ret = str_squeeze(dst, sizeof(dst),
                                  argv[i], strlen(argv[i]) + 1,
                                  dst, sizeof(dst));
        if (ret < 0) {
            perror("str_squeeze");
            return EXIT_FAILURE;
        }
    }

    printf("%s\n", dst);
    return EXIT_SUCCESS;
}