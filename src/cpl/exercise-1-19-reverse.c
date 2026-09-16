#include <stdio.h>
#include "string_utils.h"

#define MAXLEN 1000

char s[MAXLEN];
int i = 0;

void wrapup() {
    s[i] = '\0';
    char s_1[MAXLEN];
    str_reverse(s, sizeof(s), s_1, sizeof(s_1));
    printf("%s\n", s_1);
}

int main() {
    int c;

    while ((c = getchar()) != EOF) {
        if (c != '\n') {
            s[i++] = c;
        } else {
            wrapup();
            i = 0;
        }
    }

    wrapup();

    return 0;
}