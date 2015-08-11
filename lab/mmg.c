#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    STRING str; string_init(&str);
    mmg_conn *db;
    NEOERR *err;

    mtc_init("mmg", 3);

    err = mmg_init("10.1.172.10", 27017, 0, &db);
    DIE_NOK_MTL(err);

    mtimer_start();

    for (int i = 0; i < 10000; i++) {
        err = mmg_string_insertf(db, "test.labx", "{'t': 'strinsert', 'intime': %d}", i);
        TRACE_NOK(err);
    }

    mtimer_stop("insert x");


    mtimer_start();

    HDF *node;
    hdf_init(&node);

    for (int i = 0; i < 10000; i++) {
        hdf_set_valuef(node, "%d.t=%s", i, "strinsert");
        hdf_set_valuef(node, "%d.intime=%d", i, i);
    }

    err = mmg_hdf_insert_n(db, "test.laby", node);
    TRACE_NOK(err);

    hdf_destroy(&node);

    mtimer_stop("insert y");


    return 0;
}
