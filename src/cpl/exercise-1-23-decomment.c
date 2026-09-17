#include <stdio.h>

int main() {
    int c;

    /**
     * stat    left      right (c)  next stat
     * 0       foo       /          1
     *                   '          5
     *                   "          6
     *                   other      0
     *
     * 1       /         /          2
     *                   *          3
     *                   '          5
     *                   "          6
     *                   other      0
     *
     * 2       //foo     \n         0
     *                   other      2
     *
     * 3       /*foo     *          4
     *                   other      3
     *
     * 4       /*foo*    /          0
     *                   *          4
     *                   other      3
     *
     * 5       'foo      '          0
     *                   \          7
     *                   other      5
     *
     * 6       "foo      "          0
     *                   \          8
     *                   other      6
     *
     * 7       'foo\     any        5
     *
     * 8       "foo\     any        6
     */
    unsigned int stat = 0;

    while ((c = getchar()) != EOF) {
        switch (stat) {
            case 0:
            switch (c) {
                case '/':
                stat = 1;
                break;
                
                case '\'':
                stat = 5;
                putchar(c);
                break;

                case '\"':
                stat = 6;
                putchar(c);
                break;

                default:
                putchar(c);
                break;
            }
            break;

            case 1:
            switch (c) {
                case '/':
                stat = 2;
                break;

                case '*':
                stat = 3;
                break;

                case '\'':
                stat = 5;
                putchar(c);
                break;

                case '\"':
                stat = 6;
                putchar(c);
                break;

                default:
                stat = 0;
                putchar('/');
                putchar(c);
                break;
            }
            break;

            case 2:
            if (c == '\n') {
                stat = 0;
                putchar(c);
            }
            break;

            case 3:
            if (c == '*') stat = 4;
            break;

            case 4:
            switch (c) {
                case '/':
                stat = 0;
                break;

                case '*':
                break;

                default:
                stat = 3;
                break;
            }
            break;

            case 5:
            switch (c) {
                case  '\'':
                stat = 0;
                break;

                case '\\':
                stat = 7;
                break;
            }
            putchar(c);
            break;

            case 6:
            switch (c) {
                case '\"':
                stat = 0;
                break;

                case '\\':
                stat = 8;
                break;
            }
            putchar(c);
            break;

            case 7:
            stat = 5;
            putchar(c);
            break;

            case 8:
            stat = 6;
            putchar(c);
            break;
        }
    }
}