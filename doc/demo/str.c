#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    const char *s = "haHA ";

    printf("%s\n", mstr_repchr(s, 'a', ' '));

    return 0;
}
