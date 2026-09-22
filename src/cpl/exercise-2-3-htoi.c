#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/string_utils.h"

#define MAXLEN 1024

int main() {
    char s[MAXLEN];
    str_read(s, sizeof(s));
    s[strcspn(s, "\r\n")] = '\0';

    long long lld = str_htoi(s, sizeof(s));
    if (lld >= 0) {
        printf("%lld\n", lld);
        return 0;
    } else {
        return 1;
    }
}