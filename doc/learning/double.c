#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *s = "23.01084057";

    float a = strtof(s, NULL), a1 = 23.01084057;
    double b = strtod(s, NULL), b1 = 23.01084057;

    printf("%d %.8f %.8f\n", sizeof(a), a, a1);
    printf("%d %.8f %.8f\n", sizeof(b), b, b1);

    char *s2 = "2301084057e-8";
    float x = strtof(s2, NULL), x1 = 2301084057e-8;
    double y = strtod(s2, NULL), y1 = 2301084057e-8;
    printf("%.8f %.8f\n", x, x1);
    printf("%.8f %.8f\n", y, y1);

    return 0;
}
