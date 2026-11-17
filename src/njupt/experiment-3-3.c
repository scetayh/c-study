#include <stdio.h>

int i_pow(int base, int exp) {
    if (!base) {
        return 0;
    }

    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }

    return result;
}

int narcissistic(int number) {
    if (number < 100) {
        return 0;
    }

    int digits = 0;
    for (int i = number; i; i /= 10) {
        digits += 1;
    }

    int result = 0;
    for (int i = number; i; i /= 10) {
        result += i_pow(i % 10, digits);
    }

    return result == number ? 1 : 0;
}

void PrintN(int m, int n) {
    for (int i = m + 1; i < n; i++) {
        if (narcissistic(i)) {
            printf("%d\n", i);
        }
    }
}
    
int main()
{
    int m, n;
  
    scanf("%d %d", &m, &n);
    if ( narcissistic(m) ) printf("%d is a narcissistic number\n", m);
    PrintN(m, n);
    if ( narcissistic(n) ) printf("%d is a narcissistic number\n", n);

    return 0;
}