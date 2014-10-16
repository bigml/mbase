#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    char *s = calloc(1, 10);

    strncpy(s, "xxx", 9);

    printf("%s\n", s);


    return 0;
}
