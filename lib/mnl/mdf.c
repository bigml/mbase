#include "mheads.h"
#include "_cs_hdf.h"

/*
 * memory map of mdf->node:
 *
 * /-----------------\    \
 * | hdf node 1      |    |
 * | hdf node 2      |    |
 * | ...             |    |
 * | hdf node x      |    |
 * |-----------------|    |
 * | hdf attribute 1 |    |
 * | hdf attribute 2 |    |
 * | ...             |     > fix length for each unit
 * | hdf attribute x |    |
 * |-----------------|    |
 * | hash table 1    |    |
 * | hash table 2    |    |
 * | ...             |    |
 * | hash table x    |    |
 * |-----------------|    |
 * | hash node 1     |    |
 * | hash node 2     |    |
 * | ...             |    |
 * | hash node x     |    |
 * |-----------------|    /
 * |                 |
 * |-----------------|    \
 * | char string 1   |    |
 * | char string 2   |     > variable length for each unit
 * | ...             |    |
 * | char string x   |    |
 * \-----------------/    /
 */

#define PACK_STR(s) do {                                                \
        if (s) {                                                        \
            slen = strlen(s) + 1;                                       \
            *len_char -= slen;                                          \
            if (*len_char < 0) return nerr_raise(NERR_ASSERT, "str error %s %d %ld", s, *len_char, slen); \
            * (unsigned char**)pos = pchar;                             \
            strcpy((char*)pchar, s);                                    \
            *pos_char += slen;                                          \
            pchar = *pos_char;                                          \
        }                                                               \
    } while (0)

#define PACK_ATTR(node) do {                                            \
        HDF_ATTR *attr = hdf_obj_attr(node);                            \
        if (attr) * (unsigned char**)pos = pattr;                       \
        while (attr) {                                                  \
            *len_attr -= sz_attr;                                       \
            if (*len_attr < 0) return nerr_raise(NERR_ASSERT, "attr error"); \
            pos = pattr;                                                \
            PACK_STR(attr->key);                                        \
            pos = pattr + sz_pointer;                                   \
            PACK_STR(attr->value);                                      \
            if (attr->next) {                                           \
                pos = pos + sz_pointer * 2;                             \
                * (unsigned char**)pos = pattr + sz_attr;               \
            }                                                           \
            *pos_attr += sz_attr;                                       \
            pattr = *pos_attr;                                          \
            attr = attr->next;                                          \
        }                                                               \
    } while (0)

#define PACK_HASH_NODES(htble) do {                                     \
        HDF *child = ((HDF*)pnode)->child;                              \
        UINT32 hashv;                                                   \
        while (child) {                                                 \
            /* mtc_dbg("pack %s", hdf_obj_name(child)); */              \
            NE_HASHNODE **hnode = __cshash_lookup_node(((HDF*)pnode)->hash, child, &hashv); \
            if (*hnode) {                                               \
                (*hnode)->value = child;                                \
            } else {                                                    \
                *len_elem -= sz_elem;                                   \
                if (*len_elem < 0) return nerr_raise(NERR_ASSERT, "elem error"); \
                *hnode = (NE_HASHNODE*)pelem;                           \
                *(UINT32*)pos = hashv;                                  \
                /* node->key */                                         \
                pos += sz_uint32 * 2;                                   \
                *(void**)pos = (void*)child;                            \
                /* node->value */                                       \
                pos += sz_pointer;                                      \
                *(void**)pos = (void*)child;                            \
                /* node->next */                                        \
                pos += sz_pointer;                                      \
                *(NE_HASHNODE**)pos = NULL;                             \
                /* next pelem */                                        \
                *pos_elem += sz_elem;                                   \
                pelem = *pos_elem;                                      \
            }                                                           \
            pos = pelem;                                                \
            child = child->next;                                        \
        }                                                               \
    } while (0)

#define PACK_HASH(htble) do {                                           \
        UINT32 size = htble->size;                                      \
        UINT32 step = sz_tble + size * sz_tble_node;                    \
        if (htble) *(unsigned char**)pos = ptble;                       \
        *len_tble -= step;                                              \
        if (*len_tble < 0) return nerr_raise(NERR_ASSERT, "hash table error"); \
        pos = ptble;                                                    \
        memcpy(pos, htble, sz_tble);                                    \
        /* htble->nodes */                                              \
        pos += sz_int * 2 + sz_pointer * 3;                             \
        * (unsigned char**)pos = pos + sz_pointer;                      \
        /* init all empty nodes */                                      \
        pos += sz_pointer;                                              \
        memset(pos, 0x0, sz_pointer * size);                            \
        pos = pelem;                                                    \
        PACK_HASH_NODES(htble);                                         \
        *pos_tble += step;                                              \
        ptble = *pos_tble;                                              \
    } while (0);

/*
 * all _mdf_xxx fuction will not check input
 */
NEOERR* _mdf_calculate_top_len(MDF *mode, HDF *node)
{
    HDF_ATTR *attr;
    HASH *nodetbl;
    size_t slen;

    /*
     * hdf node itself
     */
    mode->num_node++;
    mode->len += sizeof(HDF);

    if (node->name) {
        slen = strlen(node->name) + 1;
        mode->len += slen;
        mode->len_str += slen;
    }
    if (node->value) {
        slen = strlen(node->value) + 1;
        mode->len += slen;
        mode->len_str += slen;
    }

    /*
     * attribute
     */
    attr = hdf_obj_attr(node);
    while (attr) {
        mode->num_attr++;
        mode->len += sizeof(HDF_ATTR);
        if (attr->key) {
            slen = strlen(attr->key) + 1;
            mode->len += slen;
            mode->len_str += slen;
        }
        if (attr->value) {
            slen = strlen(attr->value) + 1;
            mode->len += slen;
            mode->len_str += slen;
        }

        attr = attr->next;
    }

    /*
     * hash table
     */
    nodetbl = node->hash;
    if (nodetbl) {
        mode->num_tble++;
        mode->len += sizeof(HASH);
        mode->num_tble_node += nodetbl->size;
        mode->len += nodetbl->size * sizeof(NE_HASHNODE*);
    }

    /*
     * hash elements
     */
    if (nodetbl) {
        mode->num_tble_elem += nodetbl->num;
        mode->len += nodetbl->num * sizeof(NE_HASHNODE);
    }

    return STATUS_OK;
}

NEOERR* _mdf_calculate_hdf_len(MDF *mode, HDF *node)
{
    HDF_ATTR *attr;
    HASH *nodetbl;
    HDF *cnode;
    size_t slen;
    NEOERR *err;

    cnode = hdf_obj_child(node);
    while (cnode) {
        /*
         * hdf node itself
         */
        mode->num_node++;
        mode->len += sizeof(HDF);

        if (cnode->name) {
            slen = strlen(cnode->name) + 1;
            mode->len += slen;
            mode->len_str += slen;
        }
        if (cnode->value) {
            slen = strlen(cnode->value) + 1;
            mode->len += slen;
            mode->len_str += slen;
        }

        /*
         * attribute
         */
        attr = hdf_obj_attr(cnode);
        while (attr) {
            mode->num_attr++;
            mode->len += sizeof(HDF_ATTR);
            if (attr->key) {
                slen = strlen(attr->key) + 1;
                mode->len += slen;
                mode->len_str += slen;
            }
            if (attr->value) {
                slen = strlen(attr->value) + 1;
                mode->len += slen;
                mode->len_str += slen;
            }

            attr = attr->next;
        }

        /*
         * hash table
         */
        nodetbl = cnode->hash;
        if (nodetbl) {
            mode->num_tble++;
            mode->len += sizeof(HASH);
            mode->num_tble_node += nodetbl->size;
            mode->len += nodetbl->size * sizeof(NE_HASHNODE*);
        }

        /*
         * hash elements
         */
        if (nodetbl) {
            mode->num_tble_elem += nodetbl->num;
            mode->len += nodetbl->num * sizeof(NE_HASHNODE);
        }

        if (hdf_obj_child(cnode)) {
            err = _mdf_calculate_hdf_len(mode, cnode);
            if (err) return nerr_pass(err);
        }

        cnode = hdf_obj_next(cnode);
    }

    return STATUS_OK;
}

NEOERR* _mdf_pack_hdf_node(HDF *node, unsigned char *pos_top,
                           int *len_node, int *len_attr, int *len_tble,
                           int *len_elem, int *len_char,
                           unsigned char **pos_node, unsigned char **pos_attr,
                           unsigned char **pos_tble, unsigned char **pos_elem,
                           unsigned char **pos_char)
{
    int sz_int, sz_uint32, sz_char, sz_pointer;
    int sz_node, sz_attr, sz_tble, sz_tble_node, sz_elem;
    unsigned char *pos, *pnode, *pattr, *ptble, *pelem, *pchar;
    size_t slen;
    HDF *cnode;
    NEOERR *err;

    if (*len_node < 0 || *len_attr < 0 || *len_tble < 0 || *len_elem < 0 || *len_char < 0)
        return nerr_raise(NERR_ASSERT, "something wrong, stop %d %d %d %d %d",
                          *len_node, *len_attr, *len_tble, *len_elem, *len_char);

    sz_int = sizeof(int);
    sz_uint32 = sizeof(UINT32);
    sz_char = sizeof(char);
    sz_pointer = sizeof(void*);
    sz_node = sizeof(HDF);
    sz_attr = sizeof(HDF_ATTR);
    sz_tble = sizeof(HASH);
    sz_tble_node = sizeof(NE_HASHNODE*);
    sz_elem = sizeof(NE_HASHNODE);

    cnode = hdf_obj_child(node);
    while (cnode) {
        /*
         * refresh pointer
         */
        pos   = *pos_node;
        pnode = *pos_node;
        pattr = *pos_attr;
        pchar = *pos_char;

        /*
         * node
         */
        *len_node -= sz_node;
        *pos_node += sz_node;
        if (*len_node < 0) return nerr_raise(NERR_ASSERT, "node memory error");
        memcpy(pos, cnode, sz_node);

        /* name */
        pos = pnode + sz_int * 4;
        PACK_STR(cnode->name);

        /* value */
        pos = pnode + sz_int * 4 + sz_pointer;
        PACK_STR(cnode->value);

        /* attr */
        if (hdf_obj_attr(cnode)) {
            pos = pnode + sz_int * 4 + sz_pointer * 2;
            PACK_ATTR(cnode);
        }

        /* top */
        pos = pnode + sz_int * 4 + sz_pointer * 3;
        * (unsigned char**)pos = pos_top;

        /* TODO last_hp */
        /* TODO last_hs */
        pos = pnode + sz_int * 4 + sz_pointer * 6;
        * (unsigned char**)pos = 0x0;
        pos = pnode + sz_int * 4 + sz_pointer * 7;
        * (unsigned char**)pos = 0x0;

        /* child */
        if (hdf_obj_child(cnode)) {
            pos = pnode + sz_int * 4 + sz_pointer * 5;
            * (unsigned char**)pos = pnode + sz_node;

            err = _mdf_pack_hdf_node(cnode, pos_top,
                                     len_node, len_attr, len_tble, len_elem, len_char,
                                     pos_node, pos_attr, pos_tble, pos_elem, pos_char);
            if (err) return nerr_pass(err);
        }

        if (cnode->hash) {
            /* hash */
            ptble = *pos_tble;
            pelem = *pos_elem;
            pos = pnode + sz_int * 4 + sz_pointer * 8;
            PACK_HASH(cnode->hash);
        }

        if (node->last_child) {
            /* last_child */
            pos = pnode + sz_int * 4 + sz_pointer * 9;
            * (unsigned char**)pos = *pos_node - sz_node;
        }

        /* next */
        if (hdf_obj_next(cnode)) {
            pos = pnode + sz_int * 4 + sz_pointer * 4;
            * (unsigned char**)pos = *pos_node;
        }

        cnode = hdf_obj_next(cnode);
    }

    return STATUS_OK;
}

NEOERR* _mdf_pack_top_node(HDF *node, unsigned char *pos_top,
                           int *len_node, int *len_attr, int *len_tble,
                           int *len_elem, int *len_char,
                           unsigned char **pos_node, unsigned char **pos_attr,
                           unsigned char **pos_tble, unsigned char **pos_elem,
                           unsigned char **pos_char)
{
    int sz_int, sz_uint32, sz_char, sz_pointer;
    int sz_node, sz_attr, sz_tble, sz_tble_node, sz_elem;
    unsigned char *pos, *pnode, *pattr, *ptble, *pelem, *pchar;
    size_t slen;
    NEOERR *err;

    if (*len_node < 0 || *len_attr < 0 || *len_tble < 0 || *len_elem < 0 || *len_char < 0)
        return nerr_raise(NERR_ASSERT, "something wrong, stop %d %d %d %d %d",
                          *len_node, *len_attr, *len_tble, *len_elem, *len_char);

    sz_int = sizeof(int);
    sz_uint32 = sizeof(UINT32);
    sz_char = sizeof(char);
    sz_pointer = sizeof(void*);
    sz_node = sizeof(HDF);
    sz_attr = sizeof(HDF_ATTR);
    sz_tble = sizeof(HASH);
    sz_tble_node = sizeof(NE_HASHNODE*);
    sz_elem = sizeof(NE_HASHNODE);

    /*
     * refresh pointer
     */
    pos   = *pos_node;
    pnode = *pos_node;
    pattr = *pos_attr;
    pchar = *pos_char;

    /*
     * node
     */
    *len_node -= sz_node;
    *pos_node += sz_node;
    if (*len_node < 0) return nerr_raise(NERR_ASSERT, "node memory error");
    memcpy(pos, node, sz_node);

    /* name */
    pos = pnode + sz_int * 4;
    PACK_STR(node->name);

    /* value */
    pos = pnode + sz_int * 4 + sz_pointer;
    PACK_STR(node->value);

    /* attr */
    if (hdf_obj_attr(node)) {
        pos = pnode + sz_int * 4 + sz_pointer * 2;
        PACK_ATTR(node);
    }

    /* top */
    pos = pnode + sz_int * 4 + sz_pointer * 3;
    * (unsigned char**)pos = pos_top;

    /* TODO last_hp */
    /* TODO last_hs */
    pos = pnode + sz_int * 4 + sz_pointer * 6;
    * (unsigned char**)pos = 0x0;
    pos = pnode + sz_int * 4 + sz_pointer * 7;
    * (unsigned char**)pos = 0x0;

    /* child */
    if (hdf_obj_child(node)) {
        pos = pnode + sz_int * 4 + sz_pointer * 5;
        * (unsigned char**)pos = pnode + sz_node;

        err = _mdf_pack_hdf_node(node, pos_top,
                                 len_node, len_attr, len_tble, len_elem, len_char,
                                 pos_node, pos_attr, pos_tble, pos_elem, pos_char);
        if (err) return nerr_pass(err);
    }

    if (node->hash) {
        /* hash */
        ptble = *pos_tble;
        pelem = *pos_elem;
        pos = pnode + sz_int * 4 + sz_pointer * 8;
        PACK_HASH(node->hash);
    }

    if (node->last_child) {
        /* last_child */
        pos = pnode + sz_int * 4 + sz_pointer * 9;
        * (unsigned char**)pos = *pos_node - sz_node;
    }

    /* next */
    pos = pnode + sz_int * 4 + sz_pointer * 4;
    * (unsigned char**)pos = 0x0;

    return STATUS_OK;
}

NEOERR* _mdf_import_hdf(MDF *mode, HDF *node)
{
    unsigned char *pos_node, *pos_attr, *pos_tble, *pos_elem, *pos_char;
    int len_node, len_attr, len_tble, len_elem, len_char;
    NEOERR *err;

    len_node = mode->num_node * sizeof(HDF);
    len_attr = mode->num_attr * sizeof(HDF_ATTR);
    len_tble = mode->num_tble * sizeof(HASH) + mode->num_tble_node * sizeof(NE_HASHNODE*);
    len_elem = mode->num_tble_elem * sizeof(NE_HASHNODE);
    len_char = mode->len_str;

    if (mode->len != (len_node + len_attr + len_tble + len_elem + len_char)) {
        mtc_err("size error %ld %d %d %d %d %d", mode->len,
                len_node, len_attr, len_tble, len_elem, len_char);

        return nerr_raise(NERR_ASSERT, "size error %ld %d %d %d %d %d", mode->len,
                          len_node, len_attr, len_tble, len_elem, len_char);
    }

    mode->node = calloc(1, mode->len);
    if (!mode->node) return nerr_raise(NERR_NOMEM, "alloc for node");

    pos_node = (unsigned char*)mode->node;
    pos_attr = pos_node + len_node;
    pos_tble = pos_attr + len_attr;
    pos_elem = pos_tble + len_tble;
    pos_char = pos_elem + len_elem;

    err = _mdf_pack_top_node(node, (unsigned char*)mode->node,
                             &len_node, &len_attr, &len_tble, &len_elem, &len_char,
                             &pos_node, &pos_attr, &pos_tble, &pos_elem, &pos_char);
    if (err) return nerr_pass(err);

    if (len_node != 0 || len_attr != 0 || len_tble != 0 ||
        len_elem != 0 || len_char != 0) {
        mtc_err("pack error node:%d attr:%d table:%d element:%d string:%d",
                len_node, len_attr, len_tble, len_elem, len_char);
        return nerr_raise(NERR_ASSERT, "pack error %d %d %d %d %d",
                          len_node, len_attr, len_tble, len_elem, len_char);
    }

    return STATUS_OK;
}

NEOERR* _mdf_shift_offset(MDF *mode, size_t offset, bool add)
{
    int sz_int, sz_char, sz_pointer, sz_node, sz_attr, sz_tble, sz_tble_node, sz_elem;
    int len_node, len_attr, len_tble, len_elem, len_char;
    size_t szval;
    unsigned char *pnode, *pattr, *ptble, *pelem;
    unsigned char *pos;

#define SHIFT_MEMORY(pos, offset) do {                                  \
        if (*(size_t*)pos != 0) {                                       \
            if (add) szval = *(size_t*)pos + offset;                    \
            else szval = *(size_t*)pos - offset;                        \
            /* mtc_dbg("set val %lx", szval); */                        \
            if (szval < (size_t)mode->node || szval > (size_t)mode->node + mode->len) \
                return nerr_raise(NERR_ASSERT, "memory %lx nok %lx %lx", szval, (size_t)mode->node, mode->len); \
            *(size_t*)pos = szval;                                      \
        }                                                               \
    } while (0)

    len_node = mode->num_node * sizeof(HDF);
    len_attr = mode->num_attr * sizeof(HDF_ATTR);
    len_tble = mode->num_tble * sizeof(HASH) + mode->num_tble_node * sizeof(NE_HASHNODE*);
    len_elem = mode->num_tble_elem * sizeof(NE_HASHNODE);
    len_char = mode->len_str;

    pnode = (unsigned char*)mode->node;
    pattr = pnode + len_node;
    ptble = pattr + len_attr;
    pelem = ptble + len_tble;

    sz_int = sizeof(int);
    sz_char = sizeof(char);
    sz_pointer = sizeof(void*);
    sz_node = sizeof(HDF);
    sz_attr = sizeof(HDF_ATTR);
    sz_tble = sizeof(HASH);
    sz_tble_node = sizeof(NE_HASHNODE*);
    sz_elem = sizeof(NE_HASHNODE);

    mtc_dbg("shift %lx %lx %lx", (size_t)mode->node, offset, mode->len);

    if (mode->len != (len_node + len_attr + len_tble + len_elem + len_char)) {
        mtc_err("size error %ld %d %d %d %d %d", mode->len,
                len_node, len_attr, len_tble, len_elem, len_char);

        return nerr_raise(NERR_ASSERT, "size error %ld %d %d %d %d %d", mode->len,
                          len_node, len_attr, len_tble, len_elem, len_char);
    }

    for (int x = 0; x < mode->num_node; x++) {
        if (x > 0) pnode += sz_node;
        len_node -= sz_node;
        if (len_node < 0) return nerr_raise(NERR_ASSERT, "node memory error");

        /* name */
        pos = pnode + sz_int * 4;
        //if (*(uint64_t*)pos != 0)
        //    *(uint64_t*)pos = *(uint64_t*)pos + offset;
        SHIFT_MEMORY(pos, offset);

        /* value */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* attr */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* top */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* next */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* child */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* last_hp */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* last_hs */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* hash */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* last_child */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);
    }

    for (int x = 0; x < mode->num_attr; x++) {
        if (x > 0) pattr += sz_attr;
        len_attr -= sz_attr;
        if (len_attr < 0) return nerr_raise(NERR_ASSERT, "attr memory error");

        /* key */
        pos = pattr;
        SHIFT_MEMORY(pos, offset);

        /* value */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* next */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);
    }

    UINT32 size = 0, step = 0;
    for (int x = 0; x < mode->num_tble; x++) {
        if (x > 0) step = sz_tble + size * sz_tble_node;
        else step = 0;

        ptble += step;
        len_tble -= step;
        if (len_tble < 0) return nerr_raise(NERR_ASSERT, "tble memory error");

        size = *(UINT32*)ptble;

        /* nodes */
        for (int z = 0; z < size; z++) {
            pos = ptble + sz_int * 2 + sz_pointer * 3 + sz_pointer * z;
            SHIFT_MEMORY(pos, offset);
        }
    }

    for (int x = 0; x < mode->num_tble_elem; x++) {
        if (x > 0) pelem += sz_elem;
        len_elem -= sz_elem;
        if (len_elem < 0) return nerr_raise(NERR_ASSERT, "elem memory error");

        /* key */
        pos = pelem + sz_int * 2;
        SHIFT_MEMORY(pos, offset);

        /* value */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);

        /* next */
        pos += sz_pointer;
        SHIFT_MEMORY(pos, offset);
    }

    return STATUS_OK;
}


/*
 * all mdf_xxx fuction MUST check input
 */
NEOERR* mdf_init(MDF **mode)
{
    MDF *rnode;

    *mode = NULL;

    rnode = calloc(1, sizeof(MDF));
    if (!rnode) return nerr_raise(NERR_NOMEM, "unable to allocate mode");

    rnode->num_node = 0;
    rnode->num_attr = 0;
    rnode->num_tble = 0;
    rnode->num_tble_node = 0;
    rnode->num_tble_elem = 0;

    rnode->len = 0;
    rnode->len_str = 0;

    uListInit(&rnode->memlist, 10, 0);
    rnode->dirty = false;

    rnode->node = NULL;

    *mode = rnode;

    return STATUS_OK;
}

void mdf_destroy(MDF **mode)
{
    if (*mode == NULL) return;

    uListDestroy(&((*mode)->memlist), ULIST_FREE);

    free((*mode)->node);
    free(*mode);
    *mode = NULL;
}

void mdf_empty(MDF *mode)
{
    if (!mode) return;

    if (mode->memlist) {
        uListDestroy(&mode->memlist, ULIST_FREE);
        mode->memlist = NULL;
    }

    if (mode->node && mode->num_node > 0) free(mode->node);
    mode->node = NULL;

    mode->num_node = 0;
    mode->num_attr = 0;
    mode->num_tble = 0;
    mode->num_tble_node = 0;
    mode->num_tble_elem = 0;
    mode->len = 0;
    mode->len_str = 0;
    mode->dirty = false;
}


NEOERR* mdf_import_from_hdf(MDF *mode, HDF *node)
{
    NEOERR *err;

    if (!mode) return nerr_raise(NERR_ASSERT, "unable to import on null mdf");

    if (!node) return STATUS_OK;

    mdf_empty(mode);

    uListInit(&mode->memlist, 10, 0);

    err = _mdf_calculate_top_len(mode, node);
    if (err) return nerr_pass(err);

    err = _mdf_calculate_hdf_len(mode, node);
    if (err) return nerr_pass(err);

    mtc_dbg("hdf node %d, hdf attr %d, table %d, size %d, element %d, len %ld",
            mode->num_node, mode->num_attr,
            mode->num_tble, mode->num_tble_node, mode->num_tble_elem, mode->len);

    err = _mdf_import_hdf(mode, node);
    if (err) return nerr_pass(err);

    return STATUS_OK;
}

NEOERR* mdf_copy(MDF *dst, MDF *src)
{
    size_t offset;
    NEOERR *err;

    if (!dst || !src) return nerr_raise(NERR_ASSERT, "paramter null");

    if (src->dirty) return nerr_raise(NERR_ASSERT, "unable to copy dirty mode");

    if (!src->node) return STATUS_OK;

    mdf_empty(dst);

    memcpy(dst, src, sizeof(MDF));

    uListInit(&dst->memlist, 10, 0);

    dst->node = calloc(1, dst->len);
    if (!dst->node) return nerr_raise(NERR_NOMEM, "unable to allocate node");

    memcpy(dst->node, src->node, dst->len);

    if (dst->node > src->node) offset = (size_t)(dst->node) - (size_t)(src->node);
    else offset = (size_t)(src->node) - (size_t)(dst->node);

    err = _mdf_shift_offset(dst, offset, dst->node > src->node ? true : false);
    if (err) return nerr_pass(err);

    return STATUS_OK;
}


NEOERR* mdf_get_node(MDF *mode, HDF *cnode, const char *name, HDF **ret)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't get node from empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_get_node(node, name, ret, mode));
}

NEOERR* mdf_set_value(MDF *mode, HDF *cnode, const char *name, const char *value)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_value(node, name, value, mode));
}

NEOERR* mdf_set_value_attr(MDF *mode, HDF *cnode,
                           const char *name, const char *value, HDF_ATTR *attr)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_value_attr(node, name, value, attr, mode));
}

NEOERR* mdf_set_symlink(MDF *mode, HDF *cnode, const char *src, const char *dest)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_symlink(node, src, dest, mode));
}

NEOERR* mdf_set_int_value(MDF *mode, HDF *cnode, const char *name, int value)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_int_value(node, name, value, mode));
}

NEOERR* mdf_set_float_value(MDF *mode, HDF *cnode, const char *name, float value)
{
    char buf[256];
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    snprintf(buf, sizeof(buf), "%f", value);

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_value(node, name, buf, mode));
}

NEOERR* mdf_set_buf(MDF *mode, HDF *cnode, const char *name, char *value)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_buf(node, name, value, mode));
}

NEOERR* mdf_set_copy(MDF *mode, HDF *cnode, const char *dst, const char *src)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_copy(node, dst, src, mode));
}

NEOERR* mdf_set_valuef(MDF *mode, HDF *cnode, const char *fmt, ...)
{
    HDF *node;
    NEOERR *err;
    va_list ap;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    va_start(ap, fmt);
    err = _cshdf_set_valuevf(node, mode, fmt, ap);
    va_end(ap);

    return nerr_pass(err);
}

NEOERR* mdf_set_attr(MDF *mode, HDF *cnode,
                     const char *name, const char *key, const char *value)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_set_attr(node, name, key, value, mode));
}

NEOERR* mdf_set_int_attr(MDF *mode, HDF *cnode,
                         const char *name, const char *key, int value)
{
    char tok[64] = {0};
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    snprintf(tok, sizeof(tok), "%d", value);

    return nerr_pass(_cshdf_set_attr(node, name, key, tok, mode));
}

NEOERR* mdf_merge_from_hdf(MDF *mode, HDF *cnode, const char *name, HDF *node)
{
    HDF *lnode, *xnode;
    NEOERR *err;

    if (!mode || !node) return nerr_raise(NERR_ASSERT, "paramter null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't remove with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    lnode = cnode ? cnode : mode->node;

    err = mdf_get_node(mode, lnode, name, &xnode);
    if (err) return nerr_pass(err);

    err = _cshdf_merge_nodes(mode, xnode, node);
    return nerr_pass(err);
}

NEOERR* mdf_remove_tree(MDF *mode, HDF *cnode, const char *name)
{
    HDF *node;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't remove with empty mode");
    if (cnode && cnode->top != mode->node) return nerr_raise(NERR_ASSERT, "not child");

    node = cnode ? cnode : mode->node;

    return nerr_pass(_cshdf_remove_tree(node, name, mode));
}

NEOERR* mdf_remove_treef(MDF *mode, HDF *cnode, const char *fmt, ...)
{
    char key[256];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(key, sizeof(key), fmt, ap);
    va_end(ap);

    return nerr_pass(mdf_remove_tree(mode, cnode, key));
}
