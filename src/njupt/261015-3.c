#include <stdio.h>

int main() {
    double sum = 0;
    for (int i = 1; i <= 100; i++) {
        sum += 1.0 / i;
    }
    printf("sum = %.15lf\n", sum);
    return 0;
}