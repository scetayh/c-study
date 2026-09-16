#include <stdio.h>

int main(void)
{
    int c, flag = 0;

    while ((c = getchar()) != EOF) {
        if (c != ' ') {
            putchar(c);
            flag = 0;
        } else {
            if (flag == 0) {
                putchar(c);
                flag = 1;
            }
        }
    }

    return 0;
}