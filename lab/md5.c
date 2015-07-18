#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    char *s = "hello world";
    char out[16] = {0};
    char md5string[33];

    md5_signature(s, strlen(s), out);

    for(int i = 0; i < 16; ++i)
        sprintf(&md5string[i*2], "%02x", (unsigned int)out[i]);


    printf("%s\n", md5string);

    char outx[LEN_MD5];
    mstr_md5_str("hello world", outx);
    printf("%s\n", outx);

    return 0;
}
