#ifndef __MSTR_H__
#define __MSTR_H__

#include "mheads.h"

__BEGIN_DECLS

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
/* string's utf-8 length */
size_t mstr_ulen(const char *s);
/* string's strlen(), to positon pos */
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
 * make sure charout has inlen*2+1 len
 * hexin: usually uint8_t*, the value is 0~15
 * charout: '0123456789abcdef'
 */
void mstr_hex2str(unsigned char *hexin, unsigned int inlen, unsigned char *charout);

/*
 * stringfy a hex data(ofen hex array or hex pointer)
 * uint8_t in[4] = {12,5,6,15} ==> 0c05060f
 * make sure charout has inlen/2 len
 * charin: '0123456789abcdef'
 * hexout: usually uint8_t*, the value is 0~15
 */
void mstr_str2hex(unsigned char *charin, unsigned int inlen, unsigned char *hexout);

/*
 * more wide rage bin converter
 * mstr_hex2str()'s in should be 0~15, this can be 0~255
 */
void mstr_bin2char(unsigned char *in, unsigned int inlen, unsigned char *out);

__END_DECLS
#endif    /* __MSTR_H__ */
