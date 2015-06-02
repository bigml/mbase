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
    PT_ASSERT_STR_EQ(hdf_get_value(node, "aa10", "cc"), "bb");

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

    mdf_empty(mode);

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

    hdf_read_file(node, "data/metanodedbrend.hdf");
    //hdf_set_value(node, "aa", "bb");
    mdf_import_from_hdf(modea, node);

    err = mdf_copy(modeb, modea);
    PT_ASSERT(err == STATUS_OK);
    TRACE_NOK(err);

    PT_ASSERT(modea->num_node == modeb->num_node);
    PT_ASSERT(modea->num_attr == modeb->num_attr);
    PT_ASSERT(modea->num_tble == modeb->num_tble);
    PT_ASSERT(modea->len == modeb->len);

    mdf_remove_tree(modea, NULL, "slots.0");

    mdf_destroy(&modea);
    PT_ASSERT_STR_EQ(hdf_get_value(modeb->node, "areas.0.slots.0.slotid", "xx"), "5");

    hdf_write_string(node, &stra);
    hdf_write_string(modeb->node, &strb);

    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    mdf_remove_tree(modeb, NULL, "areas.0.slots.0.cards.0");
    mdf_remove_tree(modeb, NULL, "areas.0.slots.0.card.0");

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
    mdf_import_from_hdf(mode, node);
    PT_ASSERT_STR_EQ(hdf_get_value(mode->node, "aa", "xx"), "bb");

    hdf_set_value(node, "aab", "aab");
    err = mdf_set_value(mode, NULL, "aab", "aab");
    PT_ASSERT(err == STATUS_OK);
    PT_ASSERT_STR_EQ(hdf_get_value(mode->node, "aab", "xx"), "aab");

    hdf_set_int_value(node, "aa2", 101);
    err = mdf_set_int_value(mode, NULL, "aa2", 101);
    PT_ASSERT(err == STATUS_OK);
    PT_ASSERT(hdf_get_int_value(mode->node, "aa2", 0) == 101);

    hdf_set_value(node, "xx.yy", "123");
    err = mdf_set_value(mode, NULL, "xx.yy", "123");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_attr(node, "xx", "type", "108");
    err = mdf_set_attr(mode, NULL, "xx", "type", "108");
    PT_ASSERT(err == STATUS_OK);

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
    err = mdf_set_value(mode, NULL, "foo", "bar");
    PT_ASSERT(err == STATUS_OK);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    mdf_destroy(&mode);
    hdf_destroy(&node);

    /*
     * import readed
     */
    hdf_init(&node);
    mdf_init(&mode);
    hdf_read_file(node, "data/wordnode.hdf");
    mdf_import_from_hdf(mode, node);

    for (int i = 0; i < 300; i++) {
        hdf_set_valuef(node, "areas.0.tmp%d=%d", i, i);
        hdf_set_valuef(node, "areas.0.slots.0.cards.0.ttzs.0.tmp%d=%d", i, i);
    }
    for (int i = 0; i < 300; i++) {
        char tok[164];

        snprintf(tok, sizeof(tok), "areas.0.tmp%d", i);
        err = mdf_set_int_value(mode, NULL, tok, i);
        PT_ASSERT(err == STATUS_OK);

        snprintf(tok, sizeof(tok), "areas.0.slots.0.cards.0.ttzs.0.tmp%d", i);
        err = mdf_set_int_value(mode, NULL, tok, i);
        PT_ASSERT(err == STATUS_OK);
    }

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    //mdf_destroy(&mode);
    //hdf_destroy(&node);

    /*
     * misc
     */
    //hdf_init(&node);
    //mdf_init(&mode);
    //hdf_read_file(node, "data/wordnode.hdf");
    //mdf_import_from_hdf(mode, node);

    char *val = strdup("hahahhahah");
    char *val2 = strdup("aaaaaa");

    hdf_set_symlink(node, "areas", "ukey1");
    err = mdf_set_symlink(mode, NULL, "areas", "ukey1");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_buf(node, "ukey", val);
    err = mdf_set_buf(mode, NULL, "ukey", val);
    PT_ASSERT(err == STATUS_OK);

    hdf_set_buf(node, "ukey2", val2);
    err = mdf_set_buf(mode, NULL, "ukey2", val2);
    PT_ASSERT(err == STATUS_OK);

    hdf_set_value(node, "ukey3", "val1");
    err = mdf_set_buf(mode, NULL, "ukey3", "val1");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_value(node, "ukey4", "hahah");
    err = mdf_set_buf(mode, NULL, "ukey4", "hahah");
    PT_ASSERT(err == STATUS_OK);

    hdf_set_copy(node, "ukey5", "ukey4");
    err = mdf_set_copy(mode, NULL, "ukey5", "ukey4");
    PT_ASSERT(err == STATUS_OK);
    TRACE_NOK(err);

    hdf_set_valuef(node, "ukey6=%d", 100);
    err = mdf_set_valuef(mode, NULL, "ukey6=%d", 100);
    PT_ASSERT(err == STATUS_OK);

    hdf_set_attr(node, "ukey6", "type2", "val2");
    err = mdf_set_attr(mode, NULL, "ukey6", "type2", "val2");
    PT_ASSERT(err == STATUS_OK);

    hdf_write_string(mode->node, &stra);
    hdf_write_string(node, &strb);
    PT_ASSERT_STR_EQ(stra, strb);

    free(stra);
    free(strb);

    mdf_destroy(&mode);
    hdf_destroy(&node);
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

    /* 当前不支持往空mdf中set 和  remove，暂不测试 */
#if 0
    err = mdf_remove_tree(mode, NULL, "aa");
    PT_ASSERT(err != STATUS_OK); /* todo: right? */

    mdf_set_value(mode, NULL, "aa", "bb");
    err = mdf_remove_tree(mode, NULL, "aa");
    PT_ASSERT(err == STATUS_OK);

    PT_ASSERT(mode->num_node == 0);

    hdf_write_string(mode->node, &stra);
    PT_ASSERT_STR_EQ(stra, "");

    free(stra);
#endif

    /*
     * import mode
     */
    hdf_set_value(node, "aa", "bb");
    hdf_set_value(node, "foo.bar", "123");

    mdf_import_from_hdf(mode, node);

    hdf_remove_tree(node, "foo");
    err = mdf_remove_tree(mode, NULL, "foo");
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
