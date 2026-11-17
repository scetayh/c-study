#include <stdio.h>

int CountDigit(int number, int digit) {
    if (!number && !digit) {
        return 1;
    }
    
    int cnt = 0;
    for (int i = number; i; i /= 10) {
        cnt += i % 10 == digit ? 1 : 0;
    }
    return cnt;
}
    
int main()
{
    int number, digit;

    scanf("%d %d", &number, &digit);
    printf("Number of digit %d in %d: %d\n", digit, number, CountDigit(number, digit));
    
    return 0;
}