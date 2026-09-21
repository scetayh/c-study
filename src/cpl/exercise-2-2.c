#include <stdio.h>

#define LIM 1000

int main() {
    unsigned int i;
    unsigned int lim = LIM;
    int c;
    char s[lim];

    for (i = 0; i < lim - 1; ++i) {
        if ((c = getchar()) != '\n') {
            if (c != EOF) {
                s[i] = c;
            }
        }
    }

    printf("%s\n", s);

    return 0;
}