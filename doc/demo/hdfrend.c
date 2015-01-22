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

    hdf_read_file(datanode, "rend.data.hdf");
    hdf_read_file(confignode, "rend.config.hdf");

    err = mcs_data_rend(confignode, datanode, outnode);
    OUTPUT_NOK(err);

    hdf_dump_format(outnode, 0, stdout);

    hdf_destroy(&datanode);
    hdf_destroy(&confignode);
    hdf_destroy(&outnode);

    return 0;
}
