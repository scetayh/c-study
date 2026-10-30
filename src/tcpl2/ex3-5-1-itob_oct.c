#include <stdio.h>
#include <stdint.h>
#include "string_utils.h"
#include "bit32_utils.h"

int main() {
    char a[1024];

    for (int i = 0; i <= 100; i++) {
        printf("[%ld] %d -> %s\n", u32_to_oct_str(i, a, sizeof(a)), i, a);
    }
    
    return 0;
}