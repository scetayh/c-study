#include <stdio.h>

#define MONTHS 17

int main() {
    unsigned long long arr[MONTHS];
    arr[0] = arr[1] = 1;

    printf("Month %llu: %llu\n", 1llu, 1llu);
    printf("Month %llu: %llu\n", 2llu, 1llu);

    for (unsigned long long i = 0; i + 2 < MONTHS; i++) {
        arr[i + 2] = arr[i] + arr[i + 1];
        printf("Month %llu: %llu\n", i + 3, arr[i + 2]);
    }
}