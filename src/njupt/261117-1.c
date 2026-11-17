#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int champion = 0;
    float champion_grade = 0;

    float grade;
    float best_grade = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            scanf("%f", &grade);
            if (grade > best_grade) {
                best_grade = grade;
            }
        }
        if (best_grade > champion_grade) {
            champion = i;
            champion_grade = best_grade;
        }
    }

    printf("%d %.2f", champion, champion_grade);
}