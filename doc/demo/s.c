#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    char *s = calloc(1, 10);
    float f = 22 / 100.0;

    strncpy(s, "xxx", 9);

    printf("%s %d %d %.2f\n", s, (int)( (50946) / 3600), (int)(50946 % 3600), f);


    return 0;
}
