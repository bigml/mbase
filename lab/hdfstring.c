#include "mheads.h"

HDF *g_cfg = NULL;
HASH *g_datah = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *node, *rnode;

    hdf_init(&node);

    //hdf_set_value(node, "foo", "111");
    hdf_set_value(node, "aa", "100");
    hdf_get_node(node, "bb", &rnode);
    //hdf_set_value(node, "bb", "{ }");

    mcs_set_int_attr(node, "bb", "type", CNODE_TYPE_ARRAY);

    char *s;

    hdf_write_string(node, &s);

    printf("%s\n", s);

    struct json_object *out = NULL;
    mjson_import_from_hdf(node, &out);
    printf("%s\n", json_object_to_json_string(out));

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
