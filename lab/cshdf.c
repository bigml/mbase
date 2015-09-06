#include "mheads.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    char key[10];
    HDF *bignode;

    mtc_init("cshdf", 7);

    hdf_init(&bignode);

    for (int i = 0; i < 5003929; i++) {
        mstr_rand_string_with_len(key, 10);

        hdf_set_valuef(bignode, "%s.today=10", key);
        hdf_set_valuef(bignode, "%s.toweek=11", key);
        hdf_set_valuef(bignode, "%s.tomonth=12", key);
        hdf_set_valuef(bignode, "%s.total=234", key);

        if (i % 10000 == 0) printf("%d\n", i);
    }

    //hdf_dump(bignode, NULL);

    printf("child num %d\n", mcs_get_child_num(bignode, NULL));

    int count = 0;

    mtimer_start();

    HDF *cnode = hdf_obj_child(bignode);
    while (cnode) {
        char *name = hdf_obj_name(cnode);

        if (mcs_get_int_valuef(bignode, 0, "%s.today", name) != 10) printf("error\n");
        if (mcs_get_int_valuef(bignode, 0, "%s.toweek", name) != 11) printf("error\n");
        if (mcs_get_int_valuef(bignode, 0, "%s.tomonth", name) != 12) printf("error\n");
        if (mcs_get_int_valuef(bignode, 0, "%s.total", name) != 234) printf("error\n");

        count++;

        cnode = hdf_obj_next(cnode);
    }

    mtimer_stop("get time");

    printf("get child count %d\n", count);

    hdf_destroy(&bignode);

    return 0;
}
