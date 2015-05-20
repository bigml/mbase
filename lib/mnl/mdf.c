#include "mheads.h"
#include "_cs_hdf.h"

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

#define PACK_ELEMENTS(htble) do {                                   \
        if (htble) {                                                \
            HDF *parent = (HDF*)pnode;                              \
            HDF *child = parent->child;                             \
            while (child) {                                         \
                err = ne_hash_insert(parent->hash, child, child);   \
                if (err) return nerr_pass(err);                     \
                child = child->next;                                \
            }                                                       \
        }                                                           \
    } while (0)

#define PACK_HASH(htble) do {                                           \
        if (htble) * (unsigned char**)pos = ptble;                      \
        pos = ptble;                                                    \
        *len_tble -= sz_tble;                                           \
        if (*len_tble < 0) return nerr_raise(NERR_ASSERT, "hash table error"); \
        memcpy(pos, htble, sz_tble);                                    \
        pos = ptble + sz_int * 2;                                       \
        * (unsigned char**)pos = pelem;                                 \
        *len_elem -= sz_elem * htble->size;                             \
        if (*len_elem < 0) return nerr_raise(NERR_ASSERT, "elem error"); \
        PACK_ELEMENTS(htble);                                           \
        *pos_elem += sz_elem * htble->size;                             \
        pelem = *pos_elem;                                              \
        *pos_tble += sz_tble;                                           \
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
    }

    /*
     * hash elements
     */
    if (nodetbl) {
        mode->num_elem += nodetbl->size;
        mode->len += nodetbl->size * sizeof(NE_HASHNODE);
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
        }

        /*
         * hash elements
         */
        if (nodetbl) {
            mode->num_elem += nodetbl->size;
            mode->len += nodetbl->size * sizeof(NE_HASHNODE);
        }

        if (hdf_obj_child(cnode)) {
            err = _mdf_calculate_hdf_len(mode, cnode);
            if (err) return nerr_pass(err);
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
    int sz_int, sz_char, sz_pointer, sz_node, sz_attr, sz_tble, sz_elem;
    unsigned char *pos, *pnode, *pattr, *ptble, *pelem, *pchar;
    size_t slen;
    NEOERR *err;

    if (*len_node < 0 || *len_attr < 0 || *len_tble < 0 || *len_elem < 0 || *len_char < 0)
        return nerr_raise(NERR_ASSERT, "something wrong, stop %d %d %d %d %d",
                          *len_node, *len_attr, *len_tble, *len_elem, *len_char);

    sz_int = sizeof(int);
    sz_char = sizeof(char);
    sz_pointer = sizeof(void*);
    sz_node = sizeof(HDF);
    sz_attr = sizeof(HDF_ATTR);
    sz_tble = sizeof(HASH);
    sz_elem = sizeof(NE_HASHNODE);

    /*
     * refresh pointer
     */
    pos   = *pos_node;
    pnode = *pos_node;
    pattr = *pos_attr;
    ptble = *pos_tble;
    pelem = *pos_elem;
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
    }

    if (node->hash) {
        /* hash */
        pos = pnode + sz_int * 4 + sz_pointer * 8;
        /* TODO hash insert memory leak */
        PACK_HASH(node->hash);

    }

    if (node->last_child) {
        /* last_child */
        pos = pnode + sz_int * 4 + sz_pointer * 9;
        * (unsigned char**)pos = 0x0;
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
    int sz_int, sz_char, sz_pointer, sz_node, sz_attr, sz_tble, sz_elem;
    unsigned char *pos, *pnode, *pattr, *ptble, *pelem, *pchar;
    size_t slen;
    HDF *cnode;
    NEOERR *err;

    if (*len_node < 0 || *len_attr < 0 || *len_tble < 0 || *len_elem < 0 || *len_char < 0)
        return nerr_raise(NERR_ASSERT, "something wrong, stop %d %d %d %d %d",
                          *len_node, *len_attr, *len_tble, *len_elem, *len_char);

    sz_int = sizeof(int);
    sz_char = sizeof(char);
    sz_pointer = sizeof(void*);
    sz_node = sizeof(HDF);
    sz_attr = sizeof(HDF_ATTR);
    sz_tble = sizeof(HASH);
    sz_elem = sizeof(NE_HASHNODE);

    cnode = hdf_obj_child(node);
    while (cnode) {
        /*
         * refresh pointer
         */
        pos   = *pos_node;
        pnode = *pos_node;
        pattr = *pos_attr;
        ptble = *pos_tble;
        pelem = *pos_elem;
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
            pos = pnode + sz_int * 4 + sz_pointer * 8;
            /* TODO memory leak */
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

NEOERR* _mdf_import_hdf(MDF *mode, HDF *node)
{
    unsigned char *pos_node, *pos_attr, *pos_tble, *pos_elem, *pos_char;
    int len_node, len_attr, len_tble, len_elem, len_char;
    NEOERR *err;

    len_node = mode->num_node * sizeof(HDF);
    len_attr = mode->num_attr * sizeof(HDF_ATTR);
    len_tble = mode->num_tble * sizeof(HASH);
    len_elem = mode->num_elem * sizeof(NE_HASHNODE);
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

    err = _mdf_pack_hdf_node(node, (unsigned char*)mode->node,
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

NEOERR* _mdf_shift_offset(MDF *mode, int offset)
{
    int sz_int, sz_char, sz_pointer, sz_node, sz_attr, sz_tble, sz_elem;
    int len_node, len_attr, len_tble, len_elem, len_char;
    unsigned char *pnode, *pattr, *ptble, *pelem;
    unsigned char *pos;

#define SHIFT_MEMORY(pos, offset) do {                                  \
        if (*(uint64_t*)pos != 0) *(uint64_t*)pos = *(uint64_t*)pos + offset; \
    } while (0)

    len_node = mode->num_node * sizeof(HDF);
    len_attr = mode->num_attr * sizeof(HDF_ATTR);
    len_tble = mode->num_tble * sizeof(HASH);
    len_elem = mode->num_elem * sizeof(NE_HASHNODE);
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
    sz_elem = sizeof(NE_HASHNODE);

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

    for (int x = 0; x < mode->num_tble; x++) {
        if (x > 0) ptble += sz_tble;
        len_tble -= sz_tble;
        if (len_tble < 0) return nerr_raise(NERR_ASSERT, "tble memory error");

        /* nodes */
        pos = ptble + sz_int * 2;
        SHIFT_MEMORY(pos, offset);
    }

    for (int x = 0; x < mode->num_elem; x++) {
        if (x > 0) pelem += sz_elem;
        len_elem -= sz_elem;
        if (len_elem < 0) return nerr_raise(NERR_ASSERT, "elem memory error");

        /* key */
        pos = pelem + sz_int * 2;
        SHIFT_MEMORY(pos, offset);

        /* next */
        pos += sz_pointer * 2;
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
    rnode->num_elem = 0;

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
    mode->num_elem = 0;
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

    mtc_dbg("hdf node %d, hdf attr %d, hash table %d, hash element %d, len %ld",
            mode->num_node, mode->num_attr,
            mode->num_tble, mode->num_elem, mode->len);

    err = _mdf_import_hdf(mode, node);
    if (err) return nerr_pass(err);

    return STATUS_OK;
}

NEOERR* mdf_copy(MDF *dst, MDF *src)
{
    int offset;
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

    offset = (size_t)(dst->node) - (size_t)(src->node);

    err = _mdf_shift_offset(dst, offset);
    if (err) return nerr_pass(err);

    return STATUS_OK;
}


NEOERR* mdf_get_node(MDF *mode, const char *name, HDF **ret)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't get node from empty mode");

    return nerr_pass(_cshdf_get_node(mode->node, name, ret, mode));
}

NEOERR* mdf_set_value(MDF *mode, const char *name, const char *value)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_value(mode->node, name, value, mode));
}

NEOERR* mdf_set_value_attr(MDF *mode, const char *name, const char *value, HDF_ATTR *attr)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_value_attr(mode->node, name, value, attr, mode));
}

NEOERR* mdf_set_symlink(MDF *mode, const char *src, const char *dest)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_symlink(mode->node, src, dest, mode));
}

NEOERR* mdf_set_int_value(MDF *mode, const char *name, int value)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_int_value(mode->node, name, value, mode));
}

NEOERR* mdf_set_buf(MDF *mode, const char *name, char *value)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_buf(mode->node, name, value, mode));
}

NEOERR* mdf_set_copy(MDF *mode, const char *dst, const char *src)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_copy(mode->node, dst, src, mode));
}

NEOERR* mdf_set_valuef(MDF *mode, const char *fmt, ...)
{
    NEOERR *err;
    va_list ap;

    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    va_start(ap, fmt);
    err = _cshdf_set_valuevf(mode->node, mode, fmt, ap);
    va_end(ap);

    return nerr_pass(err);
}

NEOERR* mdf_set_attr(MDF *mode, const char *name, const char *key, const char *value)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't set with empty mode");

    return nerr_pass(_cshdf_set_attr(mode->node, name, key, value, mode));
}

NEOERR* mdf_remove_tree(MDF *mode, const char *name)
{
    if (!mode) return nerr_raise(NERR_ASSERT, "param null");
    if (!mode->node) return nerr_raise(NERR_ASSERT, "can't remove with empty mode");

    return nerr_pass(_cshdf_remove_tree(mode->node, name, mode));
}
