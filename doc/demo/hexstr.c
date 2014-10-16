#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv)
{
    char *s = "764839acf2d1";
    uint8_t u[6];
    char outs[20];

    mstr_hexstr2bin(s, strlen(s), u);

    mstr_bin2hexstr(u, 6, outs);

    printf("ori: %s, convert: %s\n", s, outs);

    return 0;
}
