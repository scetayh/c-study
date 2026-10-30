#include <stdio.h>

int main() {
    int a, b;
    scanf("%d %d", &a, &b);
    a += b;
    b = a - b;
    printf("%d %d\n", a -= b, b);
    return 0;
}