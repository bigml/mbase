#include "mheads.h"

/*
 * all _mdf_xxx fuction will not check input
 */
NEOERR* _mdf_calculate_len(MDF *mode, HDF *node)
{
    HDF *cnode;
    HDF_ATTR *attr;
    HASH *nodetbl;
    NEOERR *err;

    cnode = hdf_obj_child(node);
    while (cnode) {
        mode->num_node++;
        /*
         * hdf node itself
         */
        mode->len += sizeof(HDF);
        if (cnode->name) mode->len += strlen(cnode->name) + 1;
        if (cnode->value) mode->len += strlen(cnode->value) + 1;

        /*
         * attribute
         */
        attr = hdf_obj_attr(cnode);
        while (attr) {
            mode->num_attr++;
            mode->len += sizeof(HDF_ATTR);
            if (attr->key) mode->len += strlen(attr->key) + 1;
            if (attr->value) mode->len += strlen(attr->value) + 1;

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
            mode->num_elem += nodetbl->num;
            mode->len += nodetbl->num * sizeof(NE_HASHNODE);
        }

        if (hdf_obj_child(cnode)) {
            err = _mdf_calculate_len(mode, cnode);
            if (err) return nerr_pass(err);
        }

        cnode = hdf_obj_next(cnode);
    }

    return STATUS_OK;
}

NEOERR* _mdf_pack_hdf(HDF *node, unsigned char *pos_node, unsigned char *pos_attr,
                      unsigned char *pos_tble, unsigned char *pos_elem,
                      unsigned char *pos_char, int len)
{
    int sz_int, sz_char, sz_pointer, sz_node, sz_attr, sz_tble, sz_elem;
    unsigned char *pos;
    HDF *cnode;
    NEOERR *err;

    if (len <= 0) return nerr_raise(NERR_ASSERT, "something wrong, stop");

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
         * node
         */
        pos = pos_node;

        memcpy(pos, cnode, sz_node);
        * ()

        /*
         * attribute
         */

        /*
         * hash table
         */

        /*
         * hash elements
         */

        if (hdf_obj_child(cnode)) {
            err = _mdf_pack_hdf(mode, cnode);
            if (err) return nerr_pass(err);
        }

        cnode = hdf_obj_next(cnode);
    }

    return STATUS_OK;
}

NEOERR* _mdf_import_hdf(MDF *mode, HDF *node)
{
    unsigned char *pos, *pos_node, *pos_attr, *pos_tble, *pos_elem, *pos_char;
    NEOERR *err;

    /*
     * for top node
     */
    mode->len += sizeof(HDF);

    mode->node = calloc(1, mode->len);
    if (!mode->node) return nerr_raise(NERR_NOMEM, "alloc for node");

    pos_node = mode->node;
    pos_attr = pos_node + mode->num_node * sizeof(HDF);
    pos_tble = pos_attr + mode->num_attr * sizeof(HDF_ATTR);
    pos_elem = pos_tble + mode->num_tble * sizeof(HASH);
    pos_char = pos_elem + mode->num_elem * sizeof(NE_HASHNODE);

    if (pos_char > mode->node + mode->len)
        return nerr_raise(NERR_ASSERT, "something wrong, stop");

    pos = pos_node;
    pos += sizeof(int) * 3 + sizeof(void*) * 5;
    * (HDF *) pos = pos_node + sizeof(HDF);
    pos_node += sizeof(HDF);

    err = _mdf_pack_hdf(node, pos_node, pos_attr, pos_tble, pos_elem, pos_char,
                        len - sizeof(HDF));
    if (err) return nerr_pass(err);

    return STATUS_OK;
}


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
    rnode->dirty = false;

    rnode->node = NULL;

    *mode = rnode;

    return STATUS_OK;
}

void mdf_destroy(MDF **mode)
{
    if (*mode == NULL) return;

    free((*mode)->node);
    free(*mode);
    *mode = NULL;
}

void mdf_empty(MDF *mode)
{
    if (!mode || !mode->node || mode->num_node <= 0) return;

    free(mode->node);
    memset(mode, 0x0, sizeof(mode));
}


NEOERR* mdf_import_from_hdf(MDF *mode, HDF *node)
{
    NEOERR *err;

    if (!mode) return nerr_raise(NERR_ASSERT, "unable to import on null mdf");

    if (!node) return STATUS_OK;

    mdf_empty(mode);

    err = _mdf_calculate_len(mode, node);
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
    return STATUS_OK;
}


NEOERR* mdf_set_value(MDF *mode, const char *name, const char *value)
{
    return STATUS_OK;
}

NEOERR* mdf_set_attr(MDF *mode, const char *name, const char *key, const char *value)
{
    return STATUS_OK;
}

NEOERR* mdf_remove_tree(MDF *mode, const char *name)
{
    return STATUS_OK;
}
