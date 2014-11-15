#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv)
{
    unsigned char s[100] = "764839acf2d1";
    uint8_t u[6];
    uint8_t ulmax[8];
    unsigned char outs[20];

    mstr_hexstr2bin(s, strlen((char*)s), u);

    for (int i = 0; i < 6; i++) {
        printf("%x", u[i]);
    }

    printf("\n");

    mstr_bin2hexstr(u, 6, outs);

    printf("ori: %s, convert: %s\n", s, outs);

    unsigned long lmax = ULONG_MAX - 28, zmax;
    B642B8(lmax, ulmax);
    B82B64(ulmax, zmax);

    mstr_bin2hexstr((uint8_t*)ulmax, 8, outs);
    printf("%lu, %lx, %s, %lu\n", lmax, lmax, outs, zmax);

    return 0;
}
