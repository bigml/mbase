#include "mheads.h"
#include "ptest.h"

HDF *g_cfg = NULL;

/*
 * suite string
 */
void test_string()
{
    char *s = "m2=__IMEI__&m4=__MAC__&m5=__IDFA__";
    HDF *node;

    hdf_init(&node);
    hdf_set_value(node, "IMEI", "39");
    hdf_set_value(node, "MAC",  "26");

    char *out = mcs_repvstr_byhdf(s, "__", "__", node);

    PT_ASSERT_STR_EQ(out, "m2=39&m4=26&m5=__IDFA__");
    SAFE_FREE(out);

    hdf_set_value(node, "IDFA", NULL);
    out = mcs_repvstr_byhdf(s, "__", "__", node);
    PT_ASSERT_STR_EQ(out, "m2=39&m4=26&m5=");
    SAFE_FREE(out);

    s = "a=__IMEI__&b=__IMEI__&c=__IMEI";
    out = mcs_repvstr_byhdf(s, "__", "__", node);
    PT_ASSERT_STR_EQ(out, "a=39&b=39&c=__IMEI");
    SAFE_FREE(out);

    s = "__IMEI__&b=__IMEI__&c";
    out = mcs_repvstr_byhdf(s, "__", "__", node);
    PT_ASSERT_STR_EQ(out, "39&b=39&c");
    SAFE_FREE(out);

    s = "__IMEI__&b=IMEI__&c=__IMEI";
    out = mcs_repvstr_byhdf(s, "__", "__", node);
    PT_ASSERT_STR_EQ(out, "39&b=IMEI__&c=__IMEI");
    SAFE_FREE(out);


    hdf_destroy(&node);
}

void suite_basic()
{
    pt_add_test(test_string, "test string", "suite string");
}

int main(int argc, char** argv)
{
    mtc_init("mcs", 7);

    pt_add_suite(suite_basic);

    return pt_run();
}
