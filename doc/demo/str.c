#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    //const char *s = "ha我爱你";
    char s[100];

    strcpy(s, "ha我爱你 ");

    printf("%d\n", mstr_upos2len(s, 2));

    return 0;
}
