#ifndef __MUTIL_H__
#define __MUTIL_H__

#include "mheads.h"

__BEGIN_DECLS

#define SAFE_FREE(str)                          \
    do {                                        \
        if ((str) != NULL) {                      \
            free(str);                          \
            (str) = NULL;                         \
        }                                       \
    } while (0)

bool mutil_client_attack(HDF *hdf, char *action, char *cname,
                         uint64_t limit, time_t exp);
bool mutil_client_attack_cookie(HDF *hdf, char *action,
                                uint64_t limit, time_t exp);
void mutil_makesure_coredump();
int  mutil_systemf(char *fmt, ...)
                   ATTRIBUTE_PRINTF(1, 2);
int mutil_execvf(char *argv[], char *fmt, ...)
                 ATTRIBUTE_PRINTF(2, 3);

/*
 * res must be char xxx[LEN_TM]
 */
bool mutil_getdatetime(char *res, int len, const char *fmt, time_t second);
/*
 * res must be char xxx[LEN_TM_GMT]
 */
bool mutil_getdatetime_gmt(char *res, int len, const char *fmt, time_t second);
time_t mutil_get_abssec(char *fmt, char *time);

void mutil_utc_time(struct timespec *ts);

int  mutil_compare_int(const void *a, const void *b);
int  mutil_compare_inta(const void *a, const void *b);

/*
 * get 'outlen' numbers into array 'out',
 * random seed is array 'in' with 'inlen', can't repeat, so, inlen MUST >= outlen
 * usually used interally
 *
 * ATTENTION: make sure 'out' have 'outlen' length
 */
void mutil_rand_numbers_byarray(int in[], int inlen, int out[], int outlen);
/*
 * get 'outlen' unrepeat numbers into array 'out',
 * the maxim value in out[] is 'max', so, max MUST >= outlen
 * if you need value zero, please minus 1 for every out[]
 * don't pass a HUGEEEE max, > 10000 0000, or, will destroy stack memory
 *
 * ATTENTION: make sure 'out' have 'outlen' length
 */
void mutil_rand_numbers(int max, int out[], int outlen);

/*
 * safe atoi
 */
int satoi(const char *nptr);

__END_DECLS
#endif    /* __MUTIL_H__ */
