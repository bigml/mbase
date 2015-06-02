#ifndef __MDF_H__
#define __MDF_H__

#include "mheads.h"

__BEGIN_DECLS

/*
 * clearsilver's hdf node is awsome for business caller
 * but not suitable for high-cuncurrency-backend, especially with large number of nodes.
 * so, there is mdf. performance compare is in lab/mdf.c
 */

typedef struct {
    int num_node;               /* hdf node number */
    int num_attr;               /* hdf attribute number */
    int num_tble;               /* hash table number */
    int num_tble_node;          /* hash node size */
    int num_tble_elem;          /* hash node number */

    size_t len;                 /* all length */
    size_t len_str;             /* string buffer length */

    ULIST *memlist;             /* partly allocated memory pointer list */
    bool dirty;                 /* safe to copy, if dirty == false */

    HDF *node;
} MDF;

NEOERR* mdf_init(MDF **mode);
void mdf_destroy(MDF **mode);
/*
 * empty and reset mode's internal memory & member
 */
void mdf_empty(MDF *mode);

/*
 * make a MDF node, from clearsilver hdf node, partly similar with hdf_read_string()
 * this function very slow
 */
NEOERR* mdf_import_from_hdf(MDF *mode, HDF *node);

/*
 * copy src mdf node to dst. similar with hdf_copy(dst, NULL, src)
 * this function very fast if !src->dirty, or, it will be very slow
 */
NEOERR* mdf_copy(MDF *dst, MDF *src);

/*
 * functions will be modify the content of MDF node
 * the read only operation, we can use clearsilver's api.
 * @param mode: the mdf mode
 *        cnode: the hdf node, which name relative with
 *               cnode MUST be a child of mode->node
 *               cnode can be NULL, and you need appoint the full path in @param name
 *        name: the named node to walk to, relative to cnode if cnode != NULL
 *        value: the value to set the node to
 */
NEOERR* mdf_get_node(MDF *mode, HDF *cnode, const char *name, HDF **ret);
NEOERR* mdf_set_value(MDF *mode, HDF *cnode, const char *name, const char *value);
NEOERR* mdf_set_symlink(MDF *mode, HDF *cnode, const char *src, const char *dest);
NEOERR* mdf_set_int_value(MDF *mode, HDF *cnode, const char *name, int value);
NEOERR* mdf_set_float_value(MDF *mode, HDF *cnode, const char *name, float value);
NEOERR* mdf_set_buf(MDF *mode, HDF *cnode, const char *name, char *value);
NEOERR* mdf_set_copy(MDF *mode, HDF *cnode, const char *dst, const char *src);
NEOERR* mdf_set_valuef(MDF *mode, HDF *cnode, const char *fmt, ...);

NEOERR* mdf_set_attr(MDF *mode, HDF *cnode,
                     const char *name, const char *key, const char *value);
NEOERR* mdf_set_int_attr(MDF *mode, HDF *cnode,
                         const char *name, const char *key, int value);
NEOERR* mdf_remove_tree(MDF *mode, HDF *cnode, const char *name);
NEOERR* mdf_remove_treef(MDF *mode, HDF *cnode, const char *fmt, ...);

__END_DECLS
#endif    /* __MDF_H__ */
