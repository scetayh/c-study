#include <stdio.h>

int main()
{
    int c;
    int character[256] = {0};
    while ((c = getchar()) != EOF) ++character[c];
    printf("%s\t%s\t%s\n", "char", "code", "number");
    for (int i = 0; i < 256; ++i) {
        if (character[i] != 0) {
            if (i == '\n')
                printf("'\\n'");
            else if (i == '\t')
                printf("'\\t'");
            else if (i == '\r')
                printf("'\\r'");
            // else if (isprint(i))
            //     printf("'%c'", i);
            else
                printf("'0x%02x'", i);
            printf("\t(%d)\t%d\n", i, character[i]);
        }
    }
}