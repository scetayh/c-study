#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int months = 0;
    int couples[3] = {1, 0, 0};

    for (; (couples[0] + couples[1] + couples[2]) * 2 < n; months++) {
        int borns = couples[2];
        couples[2] += couples[1];
        couples[1] = couples[0];
        couples[0] = borns;
    }

    printf("%d\n", months);
}