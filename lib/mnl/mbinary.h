#ifndef __MBINARY_H__
#define __MBINARY_H__

#include "mheads.h"

__BEGIN_DECLS

/*
 * Binary constant generator macro
 * By Tom Torfs - donated to the public domain
 */

/*
 * %x %o %d
 */

/* All macro's evaluate to compile-time constants */

/*
 *** helper macros ***
 */

/*
 * turn a numeric literal into a hex constant
 * (avoids problems with leading zeroes)
 * 8-bit constants max value 0x11111111, always fits in unsigned long
 */
#define HEX__(n) 0x##n##LU

/* 8-bit conversion function */
#define B8__(x) ((x&0x0000000FLU)?1:0)          \
    +((x&0x000000F0LU)?2:0)                     \
    +((x&0x00000F00LU)?4:0)                     \
    +((x&0x0000F000LU)?8:0)                     \
    +((x&0x000F0000LU)?16:0)                    \
    +((x&0x00F00000LU)?32:0)                    \
    +((x&0x0F000000LU)?64:0)                    \
    +((x&0xF0000000LU)?128:0)

/*
 *** user macros ***
 */

/* for upto 8-bit binary constants */
#define B8(d) ((unsigned char)B8__(HEX__(d)))

/* for upto 16-bit binary constants, MSB first */
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8)   \
                        + B8(dlsb))

/* for upto 32-bit binary constants, MSB first */
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24)   \
                                + ((unsigned long)B8(db2)<<16)  \
                                + ((unsigned long)B8(db3)<<8)   \
                                + B8(dlsb))

/* for upto 64-bit binary constants, MSB first */
#define B64(dmsb,db2,db3,db4,db5,db6,db7,dlsb) (((unsigned long)B8(dmsb)<<56) \
                                                + ((unsigned long)B8(db2)<<48) \
                                                + ((unsigned long)B8(db3)<<40) \
                                                + ((unsigned long)B8(db4)<<32) \
                                                + ((unsigned long)B8(db5)<<24) \
                                                + ((unsigned long)B8(db6)<<16) \
                                                + ((unsigned long)B8(db7)<<8) \
                                                + B8(dlsb))

/*
 * Sample usage:
 * B8(01010101) = 85
 * B16(10101010,01010101) = 43605
 * B32(10000000,11111111,10101010,01010101) = 2164238933
 * B64(00000000,11111111,11111111,11111111,10000000,11111111,10101010,01010101) = ?
 */

/*
 * convert 64 bit int (usually unsigned long) to 8bit array
 * combine with mstr_bin2hexstr(u8, ...) equal to printf(%lx)
 * u8 MUST be uint8_t u8[8]
 */
#define B642B8(u64, u8)                         \
    do {                                        \
        u8[0] = u64 >> 56 & 0xFF;               \
        u8[1] = u64 >> 48 & 0xFF;               \
        u8[2] = u64 >> 40 & 0xFF;               \
        u8[3] = u64 >> 32 & 0xFF;               \
        u8[4] = u64 >> 24 & 0xFF;               \
        u8[5] = u64 >> 16 & 0xFF;               \
        u8[6] = u64 >> 8 & 0xFF;                \
        u8[7] = u64 & 0xFF;                     \
    } while (0)

/*
 * convert 8 bit array back to 64bit int (usually unsigned long)
 * u8 MUST be uint8_t u8[8]
 */
#define B82B64(u8, u64) (u64 = (((unsigned long)u8[0] << 56)   \
                                + ((unsigned long)u8[1] << 48) \
                                + ((unsigned long)u8[2] << 40) \
                                + ((unsigned long)u8[3] << 32) \
                                + ((unsigned long)u8[4] << 24) \
                                + ((unsigned long)u8[5] << 16) \
                                + ((unsigned long)u8[6] << 8)  \
                                + (u8[7])))


__END_DECLS
#endif    /* __MBINARY_H__ */
