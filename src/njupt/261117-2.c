#include <stdio.h>

#define MAXLEN 46

int main() {
    int arr[MAXLEN];
    
    arr[0] = arr[1] = 1;
    for (int i = 2; i < (int)(sizeof(arr) / sizeof(arr[0])); i++) {
        arr[i] = arr[i - 1] + arr[i - 2];
    }

    printf("%d %d %d", arr[0], arr[1], arr[2]);
}