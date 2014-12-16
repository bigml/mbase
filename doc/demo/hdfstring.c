#include "mheads.h"

HDF *g_cfg = NULL;
HASH *g_datah = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *node;

    hdf_init(&node);

    hdf_set_value(node, "foo", "111");

    mcs_set_int_attrr(node, "foo", "type", 102);

    char *s;

    hdf_write_string(node, &s);

    printf("%s\n", s);

    HDF *bnode, *cnode;
    hdf_init(&bnode);
    hdf_init(&cnode);

    hdf_dump(node, NULL);

    hdf_read_string(bnode, s);
    hdf_dump(bnode, NULL);

    hdf_copy(cnode, "xxx", node);
    hdf_dump(cnode, NULL);

    hdf_destroy(&node);
    hdf_destroy(&bnode);

    return 0;
}
