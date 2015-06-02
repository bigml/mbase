#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    printf("%d %d %d %d %d\n", sizeof(int), sizeof(size_t), sizeof(ULIST*), sizeof(bool), sizeof(MDF));

    return 0;
}
