#include "mheads.h"

HDF *g_cfg = NULL;
HASH *g_datah = NULL;

static NEOERR* query_callback(mmg_conn *db, HDF *node, bool lastone)
{
    if (node) {
        char *t = hdf_get_value(node, "t", "unknown type");
        hdf_set_valuef(node, "SetByCallback=Value_%s", t);
    }

    return STATUS_OK;
}

int main(int argc, char **argv)
{
    mmg_conn *db;
    HDF *nodea, *nodeb;
    NEOERR *err;

    if (!mtc_init("tut", 7)) {
        printf("can't open tut.log for trace\n");
        return 1;
    }

    /*
     * connect, with 1 second operation timeout
     */
    err = mmg_init("10.1.172.10", 27017, 1000, &db);
    DIE_NOK_MTL(err);

    err = mmg_auth(db, "notexist", "usera", "passa");
    TRACE_NOK(err);

    err = mmg_seed_add(db, "127.0.0.1", 27018);
    TRACE_NOK(err);

    /*
     * drop collection
     */
    if (!mongo_sync_cmd_drop(db->con, "dbtut", "colla")) {
        mtc_err("drop collection failure");
    }


    /*
     * insert
     */

    /*
     * insert through string
     */
    err = mmg_string_insert(db, "dbtut.colla", "{'t': 'strinsert', 'intime': 1356796800}");
    TRACE_NOK(err);

    err = mmg_string_insertf(db, "dbtut.colla",
                             "{'t': 'strinsert', 'intime': %ld}", time(NULL));
    TRACE_NOK(err);

    /*
     * insert through hdf
     * {
     *     t = hdfinsert
     *     intime = 1356796800
     * }
     */
    hdf_init(&nodea);
    hdf_set_value(nodea, "t", "hdfinsert");
    mcs_set_int64_value(nodea, "intime", time(NULL));

    err = mmg_hdf_insert(db, "dbtut.colla", nodea);
    TRACE_NOK(err);

    /*
     * insert through hdf (limit, secure version)
     * data {
     *     typefromclient = hdfinsert
     *     time = 1356796800
     *     noise = i'm noise node, which won't insert to db
     * }
     * limitnode {
     *     t [type=100] = typefromclient
     *     intime [type=106] = _NOW
     * }
     */
    hdf_destroy(&nodea);
    hdf_init(&nodea);
    hdf_set_value(nodea, "typefromclient", "hdfinsert");
    mcs_set_int64_value(nodea, "time", time(NULL));
    hdf_set_value(nodea, "noise", "i'm noise node, which won't insert to db");

    hdf_init(&nodeb);

    mcs_set_value_with_type(nodeb, "t", "typefromclient", CNODE_TYPE_STRING);
    mcs_set_value_with_type(nodeb, "intime", "_NOW", CNODE_TYPE_TIMESTAMP);

    err = mmg_hdf_insertl(db, "dbtut.colla", nodea, nodeb);
    TRACE_NOK(err);

    /*
     * insert through hdf (limit, secure version), with require attribute seted
     * data {
     *     time = 1356796800
     *     noise = i'm noise node, which won't insert to db
     * }
     * limitnode {
     *     t [type=100, require=true] = typefromclient
     *     intime [type=106] = _NOW
     * }
     */
    hdf_set_attr(nodeb, "t", "require", "true");
    hdf_remove_tree(nodea, "typefromclient");
    err = mmg_hdf_insertl(db, "dbtut.colla", nodea, nodeb);
    TRACE_NOK(err);

    hdf_destroy(&nodea);
    hdf_destroy(&nodeb);


    /*
     * update
     */

    /*
     * update through string
     */
    err = mmg_string_update(db, "dbtut.colla", MMG_FLAG_UPDATE_UPSERT,
                            "{'t': 'strupdate', 'intime': 1356796800}",
                            "{'t': 'strupdate'}");
    TRACE_NOK(err);

    err = mmg_string_updatef(db, "dbtut.colla", MMG_FLAG_Z,
                            "{'$inc': {'intime': 1}}",
                             "{'t': 'strupdate', 'intime': %d}", 1356796800);
    TRACE_NOK(err);

    hdf_init(&nodea);
    hdf_init(&nodeb);

    /*
     * update through hdf, limit version
     * data {
     *     typefromclient = hdfupdatelimit
     *     noise = i'm noise node, which won't update to db
     * }
     * limitnode {
     *     t [type=100, require=true] = typefromclient
     * }
     */
    hdf_set_value(nodea, "typefromclient", "hdfupdatelimit");
    hdf_set_value(nodea, "noise", "i'm noise node, which won't update to db");

    mcs_set_value_with_type(nodeb, "t", "typefromclient", CNODE_TYPE_STRING);

    err = mmg_hdf_updatefl(db, "dbtut.colla", MMG_FLAG_Z, nodea, nodeb,
                           "{'t': 'strupdate', 'intime': %d}", 1356796800);
    TRACE_NOK(err);

    /*
     * update through hdf
     * data {
     *     t = hdfupdate
     *     noise = i'm noise node, which will be update to db
     * }
     */
    hdf_destroy(&nodea);
    hdf_init(&nodea);
    hdf_set_value(nodea, "t", "hdfupdate");
    hdf_set_value(nodea, "noise", "i'm noise node, which will be update to db");

    err = mmg_hdf_updatef(db, "dbtut.colla", MMG_FLAG_Z, nodea,
                          "{'t': 'strupdate', 'intime': %d}", 1356796800);
    TRACE_NOK(err);


    /*
     * aux
     */
    int count;

    err = mmg_count(db, "dbtut", "colla", &count, "{'t': 'strupdate'}");
    TRACE_NOK(err);

    mtc_foo("strupdate type %d rows", count);

    char *sintime = mmg_get_valuef(db, "dbtut.colla", "intime", 0,
                                   "{'t': '%s'}", "strupdate");

    mtc_foo("strupdate intime string val %s", sintime);

    int iintime = mmg_get_int_valuef(db, "dbtut.colla", "intime", 0, 100,
                                     "{'t': '%s'}", "strupdate");
    mtc_foo("strupdate intime int val %d", iintime);


    err = mmg_deletef(db, "dbtut.colla", MMG_FLAG_Z,
                      "{'t': 'strupdate', 'intime': %d}", 1356796800);
    TRACE_NOK(err);


    /*
     * query
     */
    hdf_destroy(&nodea);
    hdf_init(&nodea);
    err = mmg_prepare(db, MMG_FLAG_EMPTY | MMG_FLAG_GETOID, 0, 100, NULL,
                      NULL,    /* query all columns from db */
                      "{}");   /* query all rows from db */
    TRACE_NOK(err);

    err = mmg_query(db, "dbtut.colla", NULL, nodea);
    TRACE_NOK(err);

    mtc_foo("query result...");
    TRACE_HDF(nodea);


    hdf_destroy(&nodea);
    hdf_init(&nodea);
    /* querys, can be more complex, refer mmg.h */
    err = mmg_preparef(db, MMG_FLAG_EMPTY, 0, 100, query_callback,
                       "{'t': 1, 'intime': 1}",
                       "{'intime': {'$gte': %d}}", 1356796800);
    TRACE_NOK(err);

    err = mmg_query(db, "dbtut.colla", "tutorialrows.$t$", nodea);
    TRACE_NOK(err);

    mtc_foo("query result with callback...");
    TRACE_HDF(nodea);

    /*
     * custom
     */
    hdf_destroy(&nodea);
    hdf_init(&nodea);
    err = mmg_customf(db, "dbtut", "customresult", nodea,
                      "{'findAndModify': 'colla', "
                      " 'query': {'t': 'strupdate'}, 'update': {'$inc': {'intime': 1}}}");
    TRACE_NOK(err);

    mtc_foo("custom result...");
    TRACE_HDF(nodea);

    err = mmg_customf(db, "dbtut", NULL, NULL,
                      "{'eval': 'baker(\"%s\")'}", 'colla');
    TRACE_NOK(err);

    hdf_destroy(&nodea);
    hdf_destroy(&nodeb);

    return 0;
}
