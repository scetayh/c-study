#include <stdio.h>
#include <limits.h>

int main() {
    printf("%-20s | %-20s | %-20s\n", "TYPE", "MIN", "MAX");

    for (int i = 0; i < 20 * 3 + 3 * 2; i++) {
        printf("=");
    }
    printf("\n");

    printf("%-20s | %20d | %20d\n", "(signed) char", SCHAR_MIN, SCHAR_MAX);
    printf("%-20s | %20d | %20d\n", "(signed) short (int)", SHRT_MIN, SHRT_MAX);
    printf("%-20s | %20d | %20d\n", "(signed) int", INT_MIN, INT_MAX);
    printf("%-20s | %20ld | %20ld\n", "(signed) long (int)", LONG_MIN, LONG_MAX);

    printf("%-20s | %20u | %20u\n", "unsigned char", 0, UCHAR_MAX);
    printf("%-20s | %20u | %20u\n", "unsigned short (int)", 0, USHRT_MAX);
    printf("%-20s | %20u | %20u\n", "unsigned int", 0, UINT_MAX);
    printf("%-20s | %20u | %20lu\n", "unsigned long (int)", 0, ULONG_MAX);
}