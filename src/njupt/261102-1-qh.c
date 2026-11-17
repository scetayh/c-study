#include <stdio.h>

int main() {
    int n = 2, x1 = 1, x2 = 1, total = 0, x;

    while (total <= 1000) {
        total = x1 + x2;

        x = x1;
        x1 = x2;

        x2 += x;
        n++;
    }

    printf("%d\n", n);
}