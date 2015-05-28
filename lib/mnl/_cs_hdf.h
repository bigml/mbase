#ifndef ___CSHDF_H__
#define ___CSHDF_H__

#include "mheads.h"

__BEGIN_DECLS

/*
 * only internal use, by mdf
 */

NE_HASHNODE **__cshash_lookup_node (NE_HASH *hash, void *key, UINT32 *o_hashv);

NEOERR* _cshdf_set_attr (HDF *hdf, const char *name, const char *key,
                         const char *value, MDF *mode);


NEOERR* _cshdf_set_value (HDF *hdf, const char *name, const char *value, MDF *mode);

NEOERR* _cshdf_set_value_attr (HDF *hdf, const char *name, const char *value,
                               HDF_ATTR *attr, MDF *mode);

NEOERR* _cshdf_set_symlink (HDF *hdf, const char *src, const char *dest, MDF *mode);

NEOERR* _cshdf_set_int_value (HDF *hdf, const char *name, int value, MDF *mode);

NEOERR* _cshdf_set_buf (HDF *hdf, const char *name, char *value, MDF *mode);

NEOERR* _cshdf_set_copy (HDF *hdf, const char *dest, const char *src, MDF *mode);

NEOERR* _cshdf_set_valuevf (HDF *hdf, MDF *mode, const char *fmt, va_list ap);

NEOERR* _cshdf_get_node (HDF *hdf, const char *name, HDF **ret, MDF *mode);

NEOERR* _cshdf_remove_tree (HDF *hdf, const char *name, MDF *mode);

__END_DECLS
#endif    /* ___CSHDF_H__ */
