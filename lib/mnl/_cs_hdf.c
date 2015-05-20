#include "mheads.h"

static int _mem_compare(const void *a, const void *b)
{
    const void *pa, *pb;

    if (!a) return -1;
    if (!b) return 1;

    pa = a;
    pb = *(void**)b;

    return pa - pb;
}

static void _post_new_mem(MDF *mode, void *p)
{
    if (!mode || !mode->memlist || !p) return;
    uListAppend(mode->memlist, p);
    mode->dirty = true;
}

static void _post_remove_mem(MDF *mode, void *p)
{
    if (!mode || !mode->memlist || !p) return;
    int pos = uListIndex(mode->memlist, p, _mem_compare);
    if (pos >= 0) uListDelete(mode->memlist, pos, NULL);
    else mtc_err("can't found %lx", (uint64_t)p);
}

static NE_HASHNODE **__cshash_lookup_node (NE_HASH *hash, void *key, UINT32 *o_hashv)
{
    UINT32 hashv, bucket;
    NE_HASHNODE **node;

    hashv = hash->hash_func(key);
    if (o_hashv) *o_hashv = hashv;
    bucket = hashv & (hash->size - 1);
    /* ne_warn("Lookup %s %d %d", key, hashv, bucket); */

    node = &(hash->nodes[bucket]);

    if (hash->comp_func)
    {
        while (*node && !(hash->comp_func((*node)->key, key)))
            node = &(*node)->next;
    }
    else
    {
        /* No comp_func means we're doing pointer comparisons */
        while (*node && (*node)->key != key)
            node = &(*node)->next;
    }

    /* ne_warn("Node %x", node); */
    return node;
}

static NEOERR *__cshash_resize(NE_HASH *hash, MDF *mode)
{
    NE_HASHNODE **new_nodes;
    NE_HASHNODE *entry, *prev;
    int x, next_bucket;
    int orig_size = hash->size;
    UINT32 hash_mask;

    if (hash->size > hash->num)
        return STATUS_OK;

    /* We always double in size */
    new_nodes = (NE_HASHNODE **) realloc (hash->nodes, (hash->size*2) * sizeof(NE_HASHNODE));
    if (new_nodes == NULL)
        return nerr_raise(NERR_NOMEM, "Unable to allocate memory to resize NE_HASH");

    _post_remove_mem(mode, hash->nodes);
    hash->nodes = new_nodes;
    _post_new_mem(mode, hash->nodes);
    orig_size = hash->size;
    hash->size = hash->size*2;

    /* Initialize new parts */
    for (x = orig_size; x < hash->size; x++)
    {
        hash->nodes[x] = NULL;
    }

    hash_mask = hash->size - 1;

    for (x = 0; x < orig_size; x++)
    {
        prev = NULL;
        next_bucket = x + orig_size;
        for (entry = hash->nodes[x];
             entry;
             entry = prev ? prev->next : hash->nodes[x])
        {
            if ((entry->hashv & hash_mask) != x)
            {
                if (prev)
                {
                    prev->next = entry->next;
                }
                else
                {
                    hash->nodes[x] = entry->next;
                }
                entry->next = hash->nodes[next_bucket];
                hash->nodes[next_bucket] = entry;
            }
            else
            {
                prev = entry;
            }
        }
    }

    return STATUS_OK;
}

static NEOERR *_csne_hash_insert(NE_HASH *hash, void *key, void *value, MDF *mode)
{
    UINT32 hashv;
    NE_HASHNODE **node;

    node = __cshash_lookup_node(hash, key, &hashv);

    if (*node)
    {
        (*node)->value = value;
    }
    else
    {
        *node = (NE_HASHNODE *) malloc(sizeof(NE_HASHNODE));
        if (node == NULL)
            return nerr_raise(NERR_NOMEM, "Unable to allocate NE_HASHNODE");
        _post_new_mem(mode, *node);

        (*node)->hashv = hashv;
        (*node)->key = key;
        (*node)->value = value;
        (*node)->next = NULL;
    }
    hash->num++;

    return __cshash_resize(hash, mode);
}

static void *_csne_hash_remove(NE_HASH *hash, void *key)
{
  NE_HASHNODE **node, *rem;
  void *value = NULL;

  node = __cshash_lookup_node(hash, key, NULL);
  if (*node)
  {
    rem = *node;
    *node = rem->next;
    value = rem->value;
    if (hash->destroy_func) hash->destroy_func(rem);
    // moon free(rem);
    hash->num--;
  }
  return value;
}

static int hash_hdf_comp(const void *a, const void *b)
{
    HDF *ha = (HDF *)a;
    HDF *hb = (HDF *)b;

    return (ha->name_len == hb->name_len) && !strncmp(ha->name, hb->name, ha->name_len);
}

static UINT32 hash_hdf_hash(const void *a)
{
    HDF *ha = (HDF *)a;
    return ne_crc((UINT8 *)(ha->name), ha->name_len);
}

static NEOERR *_alloc_hdf (HDF **hdf, const char *name, size_t nlen,
                           const char *value, int dupl, int wf, HDF *top, MDF *mode)
{
    *hdf = calloc (1, sizeof (HDF));
    if (*hdf == NULL)
    {
        return nerr_raise (NERR_NOMEM, "Unable to allocate memory for hdf element");
    }

    _post_new_mem(mode, *hdf);

    (*hdf)->top = top;

    if (name != NULL)
    {
        (*hdf)->name_len = nlen;
        (*hdf)->name = (char *) malloc (nlen + 1);
        if ((*hdf)->name == NULL)
        {
            free((*hdf));
            (*hdf) = NULL;
            _post_remove_mem(mode, *hdf);
            return nerr_raise (NERR_NOMEM,
                               "Unable to allocate memory for hdf element: %s", name);
        }
        strncpy((*hdf)->name, name, nlen);
        (*hdf)->name[nlen] = '\0';
        _post_new_mem(mode, (*hdf)->name);
    }
    if (value != NULL)
    {
        if (dupl)
        {
            (*hdf)->alloc_value = 1;
            (*hdf)->value = strdup(value);
            if ((*hdf)->value == NULL)
            {
                free((*hdf)->name);
                free((*hdf));
                (*hdf) = NULL;
                _post_remove_mem(mode, (*hdf)->name);
                _post_remove_mem(mode, (*hdf));
                return nerr_raise (NERR_NOMEM,
                                   "Unable to allocate memory for hdf element %s", name);
            }
            _post_new_mem(mode, (*hdf)->value);
        }
        else
        {
            (*hdf)->alloc_value = wf;
            /* We're overriding the const of value here for the set_buf case
             * where we overrode the char * to const char * earlier, since
             * alloc_value actually keeps track of the const-ness for us */
            (*hdf)->value = (char *)value;
        }
    }
    return STATUS_OK;
}

static int _walk_hdf (HDF *hdf, const char *name, HDF **node)
{
    HDF *parent = NULL;
    HDF *hp = hdf;
    HDF hash_key;
    int x = 0;
    const char *s, *n;
    int r;

    *node = NULL;

    if (hdf == NULL) return -1;
    if (name == NULL || name[0] == '\0')
    {
        *node = hdf;
        return 0;
    }

    if (hdf->link)
    {
        r = _walk_hdf (hdf->top, hdf->value, &hp);
        if (r) return r;
        if (hp)
        {
            parent = hp;
            hp = hp->child;
        }
    }
    else
    {
        parent = hdf;
        hp = hdf->child;
    }
    if (hp == NULL)
    {
        return -1;
    }

    n = name;
    s = strchr (n, '.');
    x = (s == NULL) ? strlen(n) : s - n;

    while (1)
    {
        if (parent && parent->hash)
        {
            hash_key.name = (char *)n;
            hash_key.name_len = x;
            hp = ne_hash_lookup(parent->hash, &hash_key);
        }
        else
        {
            while (hp != NULL)
            {
                if (hp->name && (x == hp->name_len) && !strncmp(hp->name, n, x))
                {
                    break;
                }
                else
                {
                    hp = hp->next;
                }
            }
        }
        if (hp == NULL)
        {
            return -1;
        }
        if (s == NULL) break;

        if (hp->link)
        {
            r = _walk_hdf (hp->top, hp->value, &hp);
            if (r) {
                return r;
            }
            parent = hp;
            hp = hp->child;
        }
        else
        {
            parent = hp;
            hp = hp->child;
        }
        n = s + 1;
        s = strchr (n, '.');
        x = (s == NULL) ? strlen(n) : s - n;
    }
    if (hp->link)
    {
        return _walk_hdf (hp->top, hp->value, node);
    }

    *node = hp;
    return 0;
}

static NEOERR* __cshdf_hash_level(HDF *hdf, MDF *mode)
{
    NEOERR *err;
    HDF *child;

    err = ne_hash_init(&(hdf->hash), hash_hdf_hash, hash_hdf_comp, NULL);
    if (err) return nerr_pass(err);
    _post_new_mem(mode, hdf->hash);
    _post_new_mem(mode, hdf->hash->nodes);

    child = hdf->child;
    while (child)
    {
        err = _csne_hash_insert(hdf->hash, child, child, mode);
        if (err) return nerr_pass(err);
        child = child->next;
    }
    return STATUS_OK;
}

static NEOERR* _set_value (HDF *hdf, const char *name, const char *value,
                           int dupl, int wf, int lnk, HDF_ATTR *attr,
                           HDF **set_node, MDF *mode)
{
    NEOERR *err;
    HDF *hn, *hp, *hs;
    HDF hash_key;
    int x = 0;
    const char *s = name;
    const char *n = name;
    int count = 0;

    if (set_node != NULL) *set_node = NULL;
    if (hdf == NULL)
    {
        return nerr_raise(NERR_ASSERT, "Unable to set %s on NULL hdf", name);
    }

    /* HACK: allow setting of this node by passing an empty name */
    if (name == NULL || name[0] == '\0')
    {
        /* handle setting attr first */
        if (hdf->attr == NULL)
        {
            hdf->attr = attr;
        }
        else
        {
            // moon _merge_attr(hdf->attr, attr);
            ;
        }
        /* set link flag */
        if (lnk) hdf->link = 1;
        else hdf->link = 0;
        /* if we're setting ourselves to ourselves... */
        if (hdf->value == value)
        {
            if (set_node != NULL) *set_node = hdf;
            return STATUS_OK;
        }
        if (hdf->alloc_value)
        {
            // moon free(hdf->value);
            hdf->value = NULL;
        }
        if (value == NULL)
        {
            hdf->alloc_value = 0;
            hdf->value = NULL;
        }
        else if (dupl)
        {
            hdf->alloc_value = 1;
            hdf->value = strdup(value);
            if (hdf->value == NULL)
                return nerr_raise (NERR_NOMEM, "Unable to duplicate value %s for %s",
                                   value, name);
            _post_new_mem(mode, hdf->value);
        }
        else
        {
            hdf->alloc_value = wf;
            hdf->value = (char *)value;
        }
        if (set_node != NULL) *set_node = hdf;
        return STATUS_OK;
    }

    n = name;
    s = strchr (n, '.');
    x = (s != NULL) ? s - n : strlen(n);
    if (x == 0)
    {
        return nerr_raise(NERR_ASSERT, "Unable to set Empty component %s", name);
    }

    if (hdf->link)
    {
        char *new_name = (char *) malloc(strlen(hdf->value) + 1 + strlen(name) + 1);
        if (new_name == NULL)
        {
            return nerr_raise(NERR_NOMEM, "Unable to allocate memory");
        }
        strcpy(new_name, hdf->value);
        strcat(new_name, ".");
        strcat(new_name, name);
        err = _set_value (hdf->top, new_name, value, dupl, wf, lnk, attr, set_node, mode);
        free(new_name);
        return nerr_pass(err);
    }
    else
    {
        hn = hdf;
    }

    while (1)
    {
        /* examine cache to see if we have a match */
        count = 0;
        hp = hn->last_hp;
        hs = hn->last_hs;

        if ((hs == NULL && hp == hn->child) || (hs && hs->next == hp))
        {
            if (hp && hp->name && (x == hp->name_len) && !strncmp (hp->name, n, x))
            {
                goto skip_search;
            }
        }

        hp = hn->child;
        hs = NULL;

        /* Look for a matching node at this level */
        if (hn->hash != NULL)
        {
            hash_key.name = (char *)n;
            hash_key.name_len = x;
            hp = ne_hash_lookup(hn->hash, &hash_key);
            hs = hn->last_child;
        }
        else
        {
            while (hp != NULL)
            {
                if (hp->name && (x == hp->name_len) && !strncmp(hp->name, n, x))
                {
                    break;
                }
                hs = hp;
                hp = hp->next;
                count++;
            }
        }

        /* save in cache any value we found */
        if (hp) {
            hn->last_hp = hp;
            hn->last_hs = hs;
        }

    skip_search:

        if (hp == NULL)
        {
            /* If there was no matching node at this level, we need to
             * allocate an intersitial node (or the actual node if we're
             * at the last part of the HDF name) */
            if (s != NULL)
            {
                /* intersitial */
                err = _alloc_hdf (&hp, n, x, NULL, 0, 0, hdf->top, mode);
            }
            else
            {
                err = _alloc_hdf (&hp, n, x, value, dupl, wf, hdf->top, mode);
                if (lnk) hp->link = 1;
                else hp->link = 0;
                hp->attr = attr;
            }
            if (err != STATUS_OK)
                return nerr_pass (err);
            if (hn->child == NULL)
                hn->child = hp;
            else
                hs->next = hp;
            hn->last_child = hp;

            /* This is the point at which we convert to a hash table
             * at this level, if we're over the count */
            if (count > FORCE_HASH_AT && hn->hash == NULL)
            {
                err = __cshdf_hash_level(hn, mode);
                if (err) return nerr_pass(err);
            }
            else if (hn->hash != NULL)
            {
                err = _csne_hash_insert(hn->hash, hp, hp, mode);
                if (err) return nerr_pass(err);
            }
        }
        else if (s == NULL)
        {
            /* If there is a matching node and we're at the end of the HDF
             * name, then we update the value of the node */
            /* handle setting attr first */
            if (hp->attr == NULL)
            {
                hp->attr = attr;
            }
            else
            {
                // moon _merge_attr(hp->attr, attr);
                ;
            }

            if (hp->value != value)
            {
                if (hp->alloc_value)
                {
                    // moon free(hp->value);
                    hp->value = NULL;
                }
                if (value == NULL)
                {
                    hp->alloc_value = 0;
                    hp->value = NULL;
                }
                else if (dupl)
                {
                    hp->alloc_value = 1;
                    hp->value = strdup(value);
                    if (hp->value == NULL)
                        return nerr_raise (NERR_NOMEM, "Unable to duplicate value %s for %s",
                                           value, name);
                    _post_new_mem(mode, hdf->value);
                }
                else
                {
                    hp->alloc_value = wf;
                    hp->value = (char *)value;
                }
            }
            if (lnk) hp->link = 1;
            else hp->link = 0;
        }
        else if (hp->link)
        {
            char *new_name = (char *) malloc(strlen(hp->value) + strlen(s) + 1);
            if (new_name == NULL)
            {
                return nerr_raise(NERR_NOMEM, "Unable to allocate memory");
            }
            strcpy(new_name, hp->value);
            strcat(new_name, s);
            err = _set_value (hdf->top, new_name, value, dupl,
                              wf, lnk, attr, set_node, mode);
            free(new_name);
            return nerr_pass(err);
        }
        /* At this point, we're done if there is not more HDF name space to
         * traverse */
        if (s == NULL)
            break;
        /* Otherwise, we need to find the next part of the namespace */
        n = s + 1;
        s = strchr (n, '.');
        x = (s != NULL) ? s - n : strlen(n);
        if (x == 0)
        {
            return nerr_raise(NERR_ASSERT, "Unable to set Empty component %s", name);
        }
        hn = hp;
    }
    if (set_node != NULL) *set_node = hp;
    return STATUS_OK;
}

NEOERR* _cshdf_set_attr (HDF *hdf, const char *name, const char *key,
                         const char *value, MDF *mode)
{
    HDF *obj;
    HDF_ATTR *attr, *last;

    _walk_hdf(hdf, name, &obj);
    if (obj == NULL)
        return nerr_raise(NERR_ASSERT, "Unable to set attribute on none existant node");

    if (obj->attr != NULL)
    {
        attr = obj->attr;
        last = attr;
        while (attr != NULL)
        {
            if (!strcmp(attr->key, key))
            {
                // moon if (attr->value) free(attr->value);
                /* a set of NULL deletes the attr */
                if (value == NULL)
                {
                    if (attr == obj->attr)
                        obj->attr = attr->next;
                    else
                        last->next = attr->next;
                    // moon free(attr->key);
                    // moon free(attr);
                    return STATUS_OK;
                }
                attr->value = strdup(value);
                if (attr->value == NULL)
                    return nerr_raise(NERR_NOMEM, "Unable to set attr %s to %s", key, value);
                _post_new_mem(mode, attr->value);
                return STATUS_OK;
            }
            last = attr;
            attr = attr->next;
        }
        last->next = (HDF_ATTR *) calloc(1, sizeof(HDF_ATTR));
        if (last->next == NULL)
            return nerr_raise(NERR_NOMEM, "Unable to set attr %s to %s", key, value);
        _post_new_mem(mode, last->next);
        attr = last->next;
    }
    else
    {
        if (value == NULL) return STATUS_OK;
        obj->attr = (HDF_ATTR *) calloc(1, sizeof(HDF_ATTR));
        if (obj->attr == NULL)
            return nerr_raise(NERR_NOMEM, "Unable to set attr %s to %s", key, value);
        attr = obj->attr;
        _post_new_mem(mode, obj->attr);
    }
    attr->key = strdup(key);
    attr->value = strdup(value);
    if (attr->key == NULL || attr->value == NULL)
        return nerr_raise(NERR_NOMEM, "Unable to set attr %s to %s", key, value);
    _post_new_mem(mode, attr->key);
    _post_new_mem(mode, attr->value);

    return STATUS_OK;
}

NEOERR* _cshdf_set_value (HDF *hdf, const char *name, const char *value, MDF *mode)
{
    return nerr_pass(_set_value (hdf, name, value, 1, 1, 0, NULL, NULL, mode));
}

NEOERR* _cshdf_set_value_attr (HDF *hdf, const char *name, const char *value,
                               HDF_ATTR *attr, MDF *mode)
{
    return nerr_pass(_set_value (hdf, name, value, 1, 1, 0, attr, NULL, mode));
}

NEOERR* _cshdf_set_symlink (HDF *hdf, const char *src, const char *dest, MDF *mode)
{
    return nerr_pass(_set_value (hdf, src, dest, 1, 1, 1, NULL, NULL, mode));
}

NEOERR* _cshdf_set_int_value (HDF *hdf, const char *name, int value, MDF *mode)
{
    char buf[256];

    snprintf (buf, sizeof(buf), "%d", value);
    return nerr_pass(_set_value (hdf, name, buf, 1, 1, 0, NULL, NULL, mode));
}

NEOERR* _cshdf_set_buf (HDF *hdf, const char *name, char *value, MDF *mode)
{
    return nerr_pass(_set_value (hdf, name, value, 0, 1, 0, NULL, NULL, mode));
}

NEOERR* _cshdf_set_copy (HDF *hdf, const char *dest, const char *src, MDF *mode)
{
    HDF *node;
    if ((_walk_hdf(hdf, src, &node) == 0) && (node->value != NULL))
    {
        return nerr_pass(_set_value (hdf, dest, node->value, 0, 0, 0, NULL, NULL, mode));
    }
    return nerr_raise (NERR_NOT_FOUND, "Unable to find %s", src);
}

NEOERR* _cshdf_set_valuevf (HDF *hdf, MDF *mode, const char *fmt, va_list ap)
{
    NEOERR *err;
    char *k;
    char *v;

    k = vsprintf_alloc(fmt, ap);
    if (k == NULL)
    {
        return nerr_raise(NERR_NOMEM, "Unable to allocate memory for format string");
    }
    v = strchr(k, '=');
    if (v == NULL)
    {
        err = nerr_raise(NERR_ASSERT, "No equals found: %s", k);
        free(k);
        return err;
    }
    *v++ = '\0';
    err = _cshdf_set_value(hdf, k, v, mode);
    free(k);
    return nerr_pass(err);
}

NEOERR* _cshdf_get_node (HDF *hdf, const char *name, HDF **ret, MDF *mode)
{
    _walk_hdf(hdf, name, ret);
    if (*ret == NULL)
    {
        return nerr_pass(_set_value (hdf, name, NULL, 0, 1, 0, NULL, ret, mode));
    }
    return STATUS_OK;
}

NEOERR* _cshdf_remove_tree (HDF *hdf, const char *name, MDF *mode)
{
    HDF *hp = hdf;
    HDF *lp = NULL, *ln = NULL; /* last parent, last node */
    int x = 0;
    const char *s = name;
    const char *n = name;

    if (hdf == NULL) return STATUS_OK;

    hp = hdf->child;
    if (hp == NULL)
    {
        return STATUS_OK;
    }

    lp = hdf;
    ln = NULL;

    n = name;
    s = strchr (n, '.');
    x = (s == NULL) ? strlen(n) : s - n;

    while (1)
    {
        while (hp != NULL)
        {
            if (hp->name && (x == hp->name_len) && !strncmp(hp->name, n, x))
            {
                break;
            }
            else
            {
                ln = hp;
                hp = hp->next;
            }
        }
        if (hp == NULL)
        {
            return STATUS_OK;
        }
        if (s == NULL) break;

        lp = hp;
        ln = NULL;
        hp = hp->child;
        n = s + 1;
        s = strchr (n, '.');
        x = (s == NULL) ? strlen(n) : s - n;
    }

    if (lp->hash != NULL)
    {
        _csne_hash_remove(lp->hash, hp);
    }
    if (ln)
    {
        ln->next = hp->next;
        /* check to see if we are the last parent's last_child, if so
         * repoint so hash table inserts will go to the right place */
        if (hp == lp->last_child)
            lp->last_child = ln;
        hp->next = NULL;
    }
    else
    {
        lp->child = hp->next;
        hp->next = NULL;
    }
    if (lp && lp->last_hp == hp) lp->last_hp = NULL;
    if (lp && lp->last_hs == hp) lp->last_hs = NULL;
    // moon _dealloc_hdf (&hp);

    return STATUS_OK;
}
