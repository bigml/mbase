#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv)
{
    char s[100] = "764839acf2d1";
    uint8_t u[6];
    char outs[20];

    mstr_hexstr2bin(s, strlen(s), u);

    for (int i = 0; i < 6; i++) {
        printf("%x", u[i]);
    }

    printf("\n");

    mstr_bin2hexstr(u, 6, outs);

    printf("ori: %s, convert: %s\n", s, outs);

    return 0;
}

