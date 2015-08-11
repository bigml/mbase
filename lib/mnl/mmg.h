#ifndef __MMG_H__
#define __MMG_H__

/*
 * mmg - moon mongodb operation API
 *       api tutorial: doc/tut/mnl/mmg.c
 */

#include "mheads.h"

__BEGIN_DECLS

typedef struct _mmg_conn {
    mongo_sync_connection *con;
    bson *docq;
    bson *docs;
    mongo_packet *p;
    mongo_sync_cursor *c;
    NEOERR* (*query_callback)(struct _mmg_conn *db, HDF *node, bool lastone);
    int flags;
    int skip;
    int limit;
    int rescount;
    bool incallback;
    void *callbackdata;
} mmg_conn;

/*
 * timeout in microsecond, 1000ms = 1s
 * dsn is the collection name
 */
NEOERR* mmg_init(char *host, int port, int timeout, mmg_conn **db);
NEOERR* mmg_auth(mmg_conn *db, char *dsn, char *user, char *pass);
NEOERR* mmg_seed_add(mmg_conn *db, char *host, int port);
void mmg_destroy(mmg_conn *db);

enum {
    MMG_FLAG_Z = 0,

    /* allow entry result. don't return error if no result found */
    MMG_FLAG_EMPTY = 1 << 11,

    /* don't add number as key for each result row */
    MMG_FLAG_MIXROWS = 1 << 12,

    /* we need mongo's _id */
    MMG_FLAG_GETOID = 1 << 13,

    /* when mmg_hdf_update(), you can wrap the value you need to update by '$set' key */
    MMG_FLAG_HDFUPSET = 1 << 14,

    /* don't export type attribute to hdf */
    MMG_FLAG_NO_TYPE = 1 << 15,

    /** When set, inserts if no matching document was found. */
    MMG_FLAG_UPDATE_UPSERT = 0x01,

    /** When set, all matching documents will be updated, not just the first. */
    MMG_FLAG_UPDATE_MULTI = 0x2
};
/*
 * flags: reused flags with mongo_wire_cmd_query().
 *        so, MMG_FLAG_XX start with 1 << 11
 * querys: query condition json object
 *         it can be just "{'obj_id': 10, 'user_id', 10}"
 *         or more complex like this:
 *         "{'$where': 'this.start_date+this.start_time > %ld'}"
 *         "{'$query': {'user_id': %d}, '$orderby': {'user_integral_id': 1}}"
 *
 * qcbk: the query callback on each result document.
 * ATTENTION: we can query recursive, but the callback won't call recursive.
 *            we just call the first NOTNULL qcbk
 *            because following prepare overwrite the formal's callback will coredump
 *            so, later prepare's callback won't be remember in db
 */
NEOERR* mmg_prepare(mmg_conn *db, int flags, int skip, int limit,
                    NEOERR* (*qcbk)(mmg_conn *db, HDF *node, bool lastone),
                    char *sels, char *querys);
NEOERR* mmg_preparef(mmg_conn *db, int flags, int skip, int limit,
                     NEOERR* (*qcbk)(mmg_conn *db, HDF *node, bool lastone),
                     char *sels, char *qfmt, ...)
                     ATTRIBUTE_PRINTF(7, 8);
void mmg_set_callbackdata(mmg_conn *db, void *data);

/*
 * dsn is the namespace, the database and collection name
 * prefix: the prefix keyword db result will store
 *         if (prefix == NULL && limit == 1)
 *             outnode.colakey = colaval
 *             outnode.colbkey = colbval
 *             ^cnode passed to callback (qcbk)
 *         if (prefix == NULL && !(flags & MMG_FLAG_MIXROWS) && limit > 1)
 *             outnode.0.row1colakey = row1colaval
 *             outnode.0.row1colbkey = row1colbval
 *             outnode.1.row1colakey = row2colaval
 *             outnode.1.row1colbkey = row2colbval
 *                     ^cnode passed to callback (qcbk)
 *         if (prefix != NULL && limit == 1)
 *             outnode.prefix.colakey = colaval
 *             outnode.prefix.colbkey = colbval
 *                     ^cnode passed to callback (qcbk)
 *         if (prefix != NULL && !(flags & MMG_FLAG_MIXROWS) && limit > 1)
 *             outnode.prefix.0.row1colakey = row1colaval
 *             outnode.prefix.0.row1colbkey = row1colbval
 *             outnode.prefix.1.row1colakey = row2colaval
 *             outnode.prefix.1.row1colbkey = row2colbval
 *                            ^cnode passed to callback (qcbk)

 *

 *         '$' in prefix mostly used to sort query result. support multi '$' pair
 *         if (strchr(prefix, '$'))
 *             e.g. char *prefix = "$phone$";
 *                  outnode.15111231681.0.row0colakey = row1colaval
 *                  outnode.15111231681.0.row0colbkey = row1colbval
 *                  outnode.15111231681.1.row1colakey = row2colaval
 *                  outnode.15111231681.1.row1colbkey = row2colbval
 *                          ^cnode passed to callback (qcbk)
 *                  char *prefix = "clientinfo.$age$.$unexist$.$phone$";
 *                  outnode.clientinfo.29.15111231681.0.row0colakey = row1colaval
 *                  outnode.clientinfo.29.15111231681.0.row0colbkey = row1colbval
 *                  outnode.clientinfo.31.15111231681.1.row1colakey = row2colaval
 *                  outnode.clientinfo.31.15111231681.1.row1colbkey = row2colbval
 *                                     ^cnode passed to callback (qcbk)
 *
 */
NEOERR* mmg_query(mmg_conn *db, char *dsn, char *prefix, HDF *outnode);

NEOERR* mmg_string_insert(mmg_conn *db, char *dsn, char *str);
NEOERR* mmg_string_insertf(mmg_conn *db, char *dsn, char *fmt, ...)
                           ATTRIBUTE_PRINTF(3, 4);
NEOERR* mmg_hdf_insert(mmg_conn *db, char *dsn, HDF *node);
/*
 * limit insert node(securer)
 * all key from lnode's key, value from node
 * with a little config syntax, please refer mcs_data_rend header
 */
NEOERR* mmg_hdf_insertl(mmg_conn *db, char *dsn, HDF *node, HDF *lnode);
/*
 * bulk insert n documents (n is the nodes's child number)
 * every child of nodes is a document
 * default bulk size is 4M, you can change it through mongo_sync_conn_set_max_insert_size()
 */
NEOERR* mmg_hdf_insert_n(mmg_conn *db, char *dsn, HDF *nodes);

NEOERR* mmg_string_update(mmg_conn *db, char *dsn, int flags, char *up, char *sel);
NEOERR* mmg_string_updatef(mmg_conn *db, char *dsn, int flags, char *up, char *selfmt, ...)
                           ATTRIBUTE_PRINTF(5, 6);
NEOERR* mmg_hdf_update(mmg_conn *db, char *dsn, int flags, HDF *node, char *sel);
NEOERR* mmg_hdf_updatef(mmg_conn *db, char *dsn, int flags, HDF *node, char *selfmt, ...)
                        ATTRIBUTE_PRINTF(5, 6);
/* same as mmg_hdf_insertl() */
NEOERR* mmg_hdf_updatefl(mmg_conn *db, char *dsn, int flags, HDF *node, HDF *lnode,
                         char *selfmt, ...)
                         ATTRIBUTE_PRINTF(6, 7);

NEOERR* mmg_count(mmg_conn *db, char *dbname, char *collname, int *ret, char *querys);
NEOERR* mmg_countf(mmg_conn *db, char *dbname, char *collname, int *ret, char *qfmt, ...)
                   ATTRIBUTE_PRINTF(5, 6);

NEOERR* mmg_delete(mmg_conn *db, char *dsn, int flags, char *sel);
NEOERR* mmg_deletef(mmg_conn *db, char *dsn, int flags, char *selfmt, ...)
                   ATTRIBUTE_PRINTF(4, 5);

/*
 * http://docs.mongodb.org/manual/reference/glossary/#glossary
 *   1. command: A MongoDB operation, other than an insert, update, remove, or query.
 *   2. $cmd:    A special virtual collection that exposes MongoDB’s database commands
 * 具体有哪些命令，和每个命令不同的传参格式，请参考：
 *   1. http://docs.mongodb.org/manual/reference/command/
 *   或
 *   2. > db.listCommands()
 * 这里的命令，与mongo shell中的命令是2个不同概念，
 *   如 > db.getCollectionNames() 可以执行，但db.$cmd.db.getCollectionNames 没有这个东东
 *   server size js 里可以写 var colls = db.getCollectionNames();
 *
 * C API 使用 mmg_custom[f]() 用来执行上面这些 command
 *   （底层是通过查询 .$cmd 来实现，可以用来实现删库、建表、索引、ss js等各种奇葩操作）
 * 用法请参考: tut/mnl/mmg.c
 *
 * dbname: just dbname, without collection name
 */
NEOERR* mmg_custom(mmg_conn *db, char *dbname, int flags,
                   char *prefix, HDF *outnode, char *command);
NEOERR* mmg_customf(mmg_conn *db, char *dbname, int flags,
                    char *prefix, HDF *outnode, char *cmdfmt, ...)
                    ATTRIBUTE_PRINTF(6, 7);

/*
 * get one row's string value
 */
char* mmg_get_valuef(mmg_conn *db, char *dsn, char *key, int skip, char *qfmt, ...)
                     ATTRIBUTE_PRINTF(5, 6);

/*
 * get servral row's int value, and add them
 */
int mmg_get_int_valuef(mmg_conn *db, char *dsn, char *key, int skip, int limit,
                       char *qfmt, ...)
                       ATTRIBUTE_PRINTF(6, 7);

/*
 * 获取查询结果的个数
 */
int mmg_last_query_count(mmg_conn *db);


#define MMG_SET_NTT(hdf, key, db, dbname, collname, selfmt, ...)        \
    do {                                                                \
        int zinta;                                                      \
        err = mmg_countf(db, dbname, collname, &zinta, selfmt, ##__VA_ARGS__); \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        hdf_set_int_value(hdf, key, zinta);                             \
    } while (0)


__END_DECLS
#endif    /* __MMG_H__ */
