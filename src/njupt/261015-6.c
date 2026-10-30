#include <stdio.h>

#define N 20

int main() {
    unsigned long long sum = 1;
    for (int i = 1; i <= N; i++) {
        sum *= i;
    }
    printf("sum = %llu\n", sum);
    return 0;
}