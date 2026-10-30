#include <stdio.h>

#define N 20

int main() {
    unsigned long long sum = 1;
    printf("2 ^ 0\t= 1\n");
    for (int i = 1; i <= N; i++) {
        sum *= 2;
        printf("2 ^ %d\t= %llu\n", i, sum);
    }
    return 0;
}