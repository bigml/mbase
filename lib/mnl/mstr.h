#ifndef __MSTR_H__
#define __MSTR_H__

#include "mheads.h"

__BEGIN_DECLS

/*
 * neo_rand(max) return rand integer between [0, max)
 */

/*
 * meory size of s MUST >= max, or len
 */
void mstr_rand_string(char *s, int max);
void mstr_rand_string_with_len(char *s, int len);
void mstr_rand_digit_with_len(char *s, int len);

/*
 * there are 3 web escape function sets:
 * 1. mstr_xxx_[un]escape();
 * 2. cgi_xxx[un]escape();
 * 3. neos_xxx_[un]escape();
 */
void mstr_html_escape(HDF *hdf, char *name);
void mstr_html_unescape(HDF *hdf, char *name);
/*
 * escape <script...>...</script> to &lt;script...>....&lt;/script&gt;
 */
void mstr_script_escape(HDF *node, char *name);

/*
 * memory size of out MUST == LEN_MD5
 */
void mstr_md5_buf(unsigned char *in, size_t len, char out[LEN_MD5]);
void mstr_md5_str(char *in, char out[LEN_MD5]);

bool mstr_isdigit(char *s);
bool mstr_isdigitn(char *buf, size_t len);

/*
 * 2-28, 2  -28, 2-   28
 * validate upper format buf, and read them into left, right
 */
bool mstr_israngen(char *buf, size_t len, int *left, int *right);

/*
 * escape sql string for safe query
 * memory size of tobuf MUST >= len * 2 + 4
 * caller take care of tobuf
 */
void mstr_real_escape_string(char *tobuf, char *from, size_t len);
/* to, an allocated, escaped string */
char* mstr_real_escape_string_nalloc(char **to, char *from, size_t len);

/*
 * repchr will modify input string. make sure s is modifiable (not const)
 */
char* mstr_repchr(char *s, char from, char to);
/*
 * replace string with string
 * return an allocated string, remember to free it
 * mstr_repstr(s, "from1", "to1", "from2", "to2", ...)
 * make sure offer suitable rep_count, or, random errors will occur.
 */
char* mstr_repstr(int rep_count, char *s, ...);
/*
 * replace variable string, around by parameter c, by dst(can be NULL)
 * in:
 * src = $level$, $level$$name$,  name.$level$.in$desc$,  or name.$level$
 * c = $
 * dst foo
 *
 * out:
 *
 * foo, foofoo, name.foo.infoo, or name.foo
 */
char* mstr_repvstr(char *src, char c, char *dst);
/*
 * strip will modify input string. make sure s is modifiable (not const)
 */
char* mstr_strip (char *s, char n);

/*
 * string's utf-8 length
 * e.g. mstr_ulen("我爱你") return 3;
 */
size_t mstr_ulen(const char *s);
/*
 * string's strlen(), at utf-8 positon
 * e.g. mstr_upos2len("hi,我爱你", 5) return 9
 */
size_t mstr_upos2len(const char *s, long int pos);

/*
 * toxx will modify input string. make sure s is modifiable (not const)
 */
char* mstr_tolower(char *s);
char* mstr_toupper(char *s);

/* DJB Hash (left to right, ....abc)*/
unsigned int hash_string(const char *str);
/* DJB Hash revert (right to left, abc... )*/
unsigned int hash_string_rev(const char *str);

/*
 * stringfy a set of binary data
 * usually used to make binary message printable
 * 0 - 31, 127 - 256, except 9, 10 will convert to hex format
 * so, 0 will convert to '0'
 */
void mstr_bin2str(uint8_t *in, unsigned int inlen, unsigned char *out);

/*
 * stringfy a set of binary data as hex format output
 * uint8_t in[4] = {12,5,6,15} ==> 0c05060f
 * 1 hex need 2 char(FF), to present, so, memory size of charout MUST >= inlen * 2 + 1
 * hexin: usually uint8_t*, value range is 0~15
 * charout: '0123456789abcdef'
 *
 * what a fuck: after hours work, we can do it use sprintf(charout[i], "%x", hexin[j])
 *
 */
void mstr_bin2hexstr(uint8_t *hexin, unsigned int inlen, unsigned char *charout);

/*
 * binarify a hex string
 * memory size of hexout MUST == inlen/2
 * charin: '0123456789abcdef'
 * hexout: usually uint8_t*, the value is 0~15
 */
void mstr_hexstr2bin(unsigned char *charin, unsigned int inlen, uint8_t *hexout);

/*
 * compare two string by version method
 * return 1 if a > b, 0 if a == b, -1 if a < b
 * e.g. "4.1.3" < "4.2"
 */
int mstr_version_compare(char *a, char *b);


__END_DECLS
#endif    /* __MSTR_H__ */
