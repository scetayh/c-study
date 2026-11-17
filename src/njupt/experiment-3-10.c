#include <stdio.h>

int factorsum(int number) {
    int sum = 0;
    for (int i = 1; i < number; i++) {
        sum += number % i ? 0 : i;
    }
    return sum;
}

void PrintPN(int m, int n) {
    int flag = 0;
    
    for (int i = m; i <= n; i++) {
        if (factorsum(i) == i) {
            flag = 1;
            
            printf("%d = 1", i);
            for (int j = 2; j < i; j++) {
                if (!(i % j)) {
                    printf(" + %d", j);
                }
            }
            putchar('\n');
        }
    }

    if (!flag) {
        printf("%s\n", "No perfect number");
    }
}
    
int main()
{
    int m, n;

    scanf("%d %d", &m, &n);
    if ( factorsum(m) == m ) printf("%d is a perfect number\n", m);
    if ( factorsum(n) == n ) printf("%d is a perfect number\n", n);
    PrintPN(m, n);

    return 0;
}