#include "mheads.h"

HDF *g_cfg = NULL;
HASH *g_datah = NULL;

int main(int argc, char **argv, char **envp)
{
    HDF *datanode, *confignode, *outnode;
    NEOERR *err;

    hdf_init(&datanode);
    hdf_init(&confignode);

    hdf_read_file(datanode, "rend.data.hdf");
    hdf_read_file(confignode, "rend.config.hdf");

    for (int i = 0; i < 2; i++) {
        hdf_init(&outnode);
        err = mcs_data_rend(confignode, datanode, outnode, MCS_FLAG_Z);
        OUTPUT_NOK(err);

        hdf_dump_format(outnode, 0, stdout);
        printf("\n\n");
    }

    hdf_destroy(&datanode);
    hdf_destroy(&confignode);
    hdf_destroy(&outnode);

    return 0;
}
