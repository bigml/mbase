#include "mheads.h"
#include "ptest.h"

HDF *g_cfg = NULL;

/*
 * suite basic
 */
void test_init()
{
    MDF *mode;
    char *str;
    NEOERR *err;

    err = mdf_init(&mode);
    PT_ASSERT(err == STATUS_OK);

    hdf_write_string(mode->node, &str);

    PT_ASSERT(mode != NULL);
    PT_ASSERT(mode->node == NULL);
    PT_ASSERT_STR_EQ(str, "");

    free(str);
    mdf_destroy(&mode);

    PT_ASSERT(mode == NULL);
}

/*
 * suite new
 */
void test_import()
{
    char *stra, *strb;
    MDF *mode;
    HDF *node;
    NEOERR *err;

    mdf_init(&mode);
    hdf_init(&node);

    hdf_set_value(node, "aa", "bb");
    hdf_set_value(node, "xx", "yy");
    hdf_set_value(node, "aa1", "bb");
    hdf_set_value(node, "aa2", "bb");
    hdf_set_value(node, "aa3", "bb");
    hdf_set_value(node, "aa4", "bb");
    hdf_set_value(node, "aa5", "bb");
    hdf_set_value(node, "aa6", "bb");
    hdf_set_value(node, "aa7", "bb");
    hdf_set_value(node, "aa8", "bb");
    hdf_set_value(node, "aa9", "bb");
    hdf_set_value(node, "aa10", "bb");
    hdf_set_attr(node, "aa", "type", "");
    hdf_set_attr(node, "xx", "type", "108");

    err = mdf_import_from_hdf(mode, node);
    PT_ASSERT(err == STATUS_OK);
    TRACE_NOK(err);

    PT_ASSERT(mode->num_attr == 2);
    PT_ASSERT(mode->dirty == false);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    //mtc_foo("%s", stra);
    //mtc_foo("%s", strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(strb);
    hdf_destroy(&node);
    hdf_write_string(mode->node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    /*
     * TODO import with node not empty?
     */
    hdf_destroy(&node);
    hdf_init(&node);

    hdf_read_file(node, "data/wordnode.hdf");
    hdf_set_value(node, "areas.0.slots.0.cards.0.ttzs.newk", "newv");
    hdf_set_attr(node, "areas.0.slots.0.cards.0.ttzs.newk", "foo", "bar");
    hdf_set_value(node, "areas.1.slots.0.cards.0.ttzs.0.newk", "newv");
    hdf_set_value(node, "areas.1.slots.0.cards.0.ttzs.0.newkk", "newvv");
    err = mdf_import_from_hdf(mode, node);
    PT_ASSERT(err == STATUS_OK);

    PT_ASSERT_STR_EQ(hdf_get_value(mode->node,
                                   "areas.1.slots.0.cards.0.ttzs.0.newk", "aa"),
                     "newv");

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    //mtc_foo("%s", stra);
    //mtc_foo("%s", strb);
    PT_ASSERT(!strcmp(stra, strb));

    free(stra);

    /*
     * import with node empty
     */
    mdf_empty(mode);
    err = mdf_import_from_hdf(mode, node);
    PT_ASSERT(err == STATUS_OK);
    hdf_write_string(mode->node, &stra);

    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    //free(strb);

    hdf_destroy(&node);
    mdf_destroy(&mode);
}

void test_copy()
{
    char *stra, *strb;
    MDF *modea, *modeb;
    HDF *node;
    NEOERR *err;

    hdf_init(&node);
    mdf_init(&modea);
    mdf_init(&modeb);

    hdf_read_file(node, "data/wordnode.hdf");
    hdf_set_value(node, "aa", "bb");
    mdf_import_from_hdf(modea, node);

    err = mdf_copy(modeb, modea);
    PT_ASSERT(err == STATUS_OK);

    PT_ASSERT(modea->num_node == modeb->num_node);
    PT_ASSERT(modea->num_attr == modeb->num_attr);
    PT_ASSERT(modea->num_tble == modeb->num_tble);
    PT_ASSERT(modea->num_elem == modeb->num_elem);
    PT_ASSERT(modea->len == modeb->len);

    mdf_destroy(&modea);
    PT_ASSERT_STR_EQ(hdf_get_value(modeb->node, "areas.0.slots.0.slotid", "xx"), "5");

    hdf_write_string(node, &stra);
    hdf_write_string(modeb->node, &strb);

    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    hdf_destroy(&node);
    mdf_destroy(&modeb);
}

/*
 * suite write
 */
void test_set()
{
    MDF *mode;
    HDF *node;
    char *stra, *strb;
    NEOERR *err;

    /*
     * fresh mode
     */
    mdf_init(&mode);
    hdf_init(&node);

    hdf_set_value(node, "aa", "bb");
    err = mdf_set_value(mode, "aa", "bb");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_value(node, "xx.yy", "123");
    err = mdf_set_value(mode, "xx.yy", "123");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_attr(node, "xx", "type", "108");
    err = mdf_set_attr(mode, "xx", "type", "108");
    PT_ASSERT(err == STATUS_OK);

    PT_ASSERT(mode->num_node == 3);
    PT_ASSERT(mode->num_attr == 1);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    mdf_destroy(&mode);

    /*
     * imported mode
     */
    mdf_init(&mode);
    mdf_import_from_hdf(mode, node);

    hdf_set_value(node, "foo", "bar");
    err = mdf_set_value(mode, "foo", "bar");
    PT_ASSERT(err == STATUS_OK);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    //mdf_destroy(&mode);
    //hdf_destroy(&node);
}

void test_remove()
{
    char *stra, *strb;
    MDF *mode;
    HDF *node;
    NEOERR *err;

    /*
     * fresh mode
     */
    mdf_init(&mode);
    hdf_init(&node);

    err = mdf_remove_tree(mode, "aa");
    PT_ASSERT(err != STATUS_OK); /* todo: right? */

    mdf_set_value(mode, "aa", "bb");
    err = mdf_remove_tree(mode, "aa");
    PT_ASSERT(err == STATUS_OK);

    PT_ASSERT(mode->num_node == 0);

    hdf_write_string(mode->node, &stra);
    PT_ASSERT_STR_EQ(stra, "");

    free(stra);

    /*
     * import mode
     */
    hdf_set_value(node, "aa", "bb");
    hdf_set_value(node, "foo.bar", "123");

    mdf_import_from_hdf(mode, node);

    hdf_remove_tree(node, "foo");
    err = mdf_remove_tree(mode, "foo");
    PT_ASSERT(err == STATUS_OK);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    mdf_destroy(&mode);
    hdf_destroy(&node);
}




void suite_basic()
{
    pt_add_test(test_init, "test init", "suite basic");
}

void suite_new()
{
    pt_add_test(test_import, "test import", "suite new");
    pt_add_test(test_copy, "test copy", "suite new");
}

void suite_write()
{
    pt_add_test(test_set, "test set", "suite write");
    pt_add_test(test_remove, "test remove", "suite write");
}

int main(int argc, char** argv)
{
    mtc_init("mdf", 7);

    pt_add_suite(suite_basic);
    pt_add_suite(suite_new);
    pt_add_suite(suite_write);
    return pt_run();
}
