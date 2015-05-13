#define MDB_QUERY_RAW(err, conn, sfmt, ...)                        \
    do {                                                        \
        err = mdb_exec(conn, "SELECT ", sfmt, ##__VA_ARGS__);    \
        if (err != STATUS_OK) return nerr_pass(err);            \
    } while (0)

typedef char* (*NameGetFunc)(MdbConn *conn);

typedef struct _EventEntry{
    int foo;
    struct _EventEntry *next;
} EventEntry;

typedef struct _AicEntry {
    EventEntry base;
    MdbConn *db;
    char *bar;
    NameGetFunc get_name;
    //char* (*get_name)(MdbConn *conn);
} AicEntry;

typedef enum {
    OP_AJAX = 0,
    OP_HTML
} OpCode;

HDF *g_cfg = NULL;
static int m_err_inited = 0;

static NEOERR* function_namef(EventEntry *entry, OpCode op,
                              const char *fmt, ...)
{
    AicEntry *e = (AicEntry*)entry;
    MdbConn *db = e->db;
    char *p = e->bar;
    char (*getter)(MdbConn *conn);
    int new_val = 100;

    va_list ap;
    va_start(ap, fmt);
    VPRINTF(stdout, fmt, ap);
    va_end(ap);

    if (!p) return nerr_raise(NERR_ASSERT, "param null");


    if (!getter) {
        mtc_err("getter null %d %s", op, fmt);
        return nerr_raise(NERR_NOTFOUND, "getter null %d %s", op, fmt);
    } else {
        new_val = new_val < 0 ? 0 : new_val;
        new_val = m_err_inited + 60 * 60;

        if (expression_a) short_expression_b;

        if (expression_a)
            long_long_long_long_long_expression_b;

        if (expression_a) {
            multi_expression_b;
            multi_expression_c;
        }
    }


    for (int i = 0; i < new_val; i++) {
        switch(op) {
        case OP_AJAX:
            getter(conn);
            break;
        default:
            break;
        }
    }


    /*
     * 逻辑A
     */
    logic_a_expression_1;
    logic_a_expression_2;
    logic_a_expression_3;

    /*
     * 逻辑B
     */
    logic_b_expression_1;
    logic_b_expression_2;


    return nerr_pass(func_bar(x, y));
    return err;
    return nerr_pass(err);
}
