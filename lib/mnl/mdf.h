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
    int num_elem;               /* hash node number */

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
 * these function are as fast as clearsilver's
 */
NEOERR* mdf_get_node(MDF *mode, const char *name, HDF **ret);

/*
 * attention: can't write empty mode(param mode must be imported or copied) currently.
 */
NEOERR* mdf_set_value(MDF *mode, const char *name, const char *value);
NEOERR* mdf_set_symlink(MDF *mode, const char *src, const char *dest);
NEOERR* mdf_set_int_value(MDF *mode, const char *name, int value);
NEOERR* mdf_set_buf(MDF *mode, const char *name, char *value);
NEOERR* mdf_set_copy(MDF *mode, const char *dst, const char *src);
NEOERR* mdf_set_valuef(MDF *mode, const char *fmt, ...);

NEOERR* mdf_set_attr(MDF *mode, const char *name, const char *key, const char *value);
NEOERR* mdf_remove_tree(MDF *mode, const char *name);


__END_DECLS
#endif    /* __MDF_H__ */
