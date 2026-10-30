#include <stdio.h>

#define N 100

int main() {
    double sum = 0;
    for (int i = 1; i <= N; i++) {
        sum += 1.0 / (2 * i - 1);
    }
    printf("sum = %.15lf\n", sum);
    return 0;
}