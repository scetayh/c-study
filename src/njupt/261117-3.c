#include <stdio.h>

int main() {
    int n, x;
    scanf("%d %d", &n, &x);

    int arr[n];

    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    int flag = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i] == x) {
            flag = 1;
            printf("%d ", i);
        }
    }

    if (flag) {
        putchar('\n');
    } else {
        puts("Not Found");
    }
}