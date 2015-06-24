#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *node;

    hdf_init(&node);

    hdf_set_value(node, "ttzs.4.ttzid", "4");
    hdf_set_value(node, "ttzs.4.ttzurl", "baidu.com");

    //hdf_copy(node, "ttzs.0", hdf_get_obj(node, "ttzs.4"));

    hdf_set_symlink(node, "ttzs.0", "ttzs.4");

    hdf_dump(node, NULL);

    printf("4 url %s, 0 url %s\n", hdf_get_value(node, "ttzs.4.ttzurl", NULL),
           hdf_get_value(node, "ttzs.0.ttzurl", NULL));


    return 0;
}
