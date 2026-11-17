#include <stdio.h>

int main() {
    int a, b;
    scanf("%d %d", &a, &b);

    a += a % 2 ? 0 : 1;
    b -= b % 2 ? 0 : 1;

    printf("%d\n", (a + b) * ((b - a) / 2 + 1) / 2);
}