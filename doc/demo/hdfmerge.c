#include "mheads.h"

HDF *g_cfg = NULL;
HASH *g_datah = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *datanode, *confignode, *outnode;
    NEOERR *err;

    hdf_init(&datanode);
    hdf_init(&confignode);
    hdf_init(&outnode);

    hdf_read_file(datanode, "merge.data.hdf");
    hdf_read_file(confignode, "merge.m.hdf");

    err = mcs_merge_data_and_config(datanode, confignode, outnode);
    OUTPUT_NOK(err);

    hdf_dump(outnode, NULL);

    hdf_destroy(&datanode);
    hdf_destroy(&confignode);
    hdf_destroy(&outnode);

    return 0;
}
