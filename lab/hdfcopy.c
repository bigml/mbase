#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *node, *nodeb;

    hdf_init(&node);
    hdf_init(&nodeb);

    hdf_set_value(node, "ttzs.4.ttzid", "4");
    hdf_set_value(node, "ttzs.4.ttzurl", "baidu.com");

    hdf_set_value(nodeb, "_fordirnode.a.name", "xx");
    hdf_set_value(nodeb, "_fordirnode.a.name.1", "xx1");
    hdf_set_value(nodeb, "_fordirnode.b.name.1", "yyy1");
    hdf_set_value(nodeb, "_fordirnode.b.value", "yy");


    //hdf_copy(node, "ttzs.0", hdf_get_obj(node, "ttzs.4"));
    hdf_copy(node, "ttzs", hdf_obj_child(nodeb));

    hdf_set_symlink(node, "ttzs.0", "ttzs.4");

    hdf_dump(node, NULL);

    printf("4 url %s, 0 url %s\n", hdf_get_value(node, "ttzs.4.ttzurl", NULL),
           hdf_get_value(node, "ttzs.0.ttzurl", NULL));


    return 0;
}
