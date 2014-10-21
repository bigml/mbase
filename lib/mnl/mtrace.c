#include "mtrace.h"

/* global file name for trace info write to */
static char m_fn[_POSIX_PATH_MAX] = "";
static FILE *m_fp = NULL;
static int m_dftlv = TC_DEFAULT_LEVEL;
static char linebuf[2096];
static char *m_trace_level[TC_LEVELS] = {"DIE", "MESSAGE", "ERROR", "WARNING", "INFO", "DEBUG", "NOISE"};

static void trace_shift_file()
{
    struct stat fs;
    if (stat(m_fn, &fs) == -1)
        return;
    if (fs.st_size < TC_MAX_SIZE)
        return;

    int i;
    char ofn[_POSIX_PATH_MAX], nfn[_POSIX_PATH_MAX];

    if (m_fp != NULL)
        fclose(m_fp);

    for (i = TC_MAX_NUM-1; i > 1; i--) {
        sprintf(ofn, "%s.%d", m_fn, i-1);
        sprintf(nfn, "%s.%d", m_fn, i);
        rename(ofn, nfn);
    }
    if (TC_MAX_NUM > 1) {
        strcpy(ofn, m_fn);
        sprintf(nfn, "%s.1", m_fn);
        rename(ofn, nfn);
    }

    m_fp = fopen(m_fn, "a+");
}

bool mtc_init(const char *fn, int level)
{
    if (level >= 0) m_dftlv = level;

    strncpy(m_fn, fn, sizeof(m_fn)-4);
    strcat(m_fn, ".log");
    if (m_fp != NULL)
        fclose(m_fp);
    m_fp = fopen(m_fn, "a+");
    if (m_fp != NULL) setvbuf(m_fp, linebuf, _IOLBF, 2096);
    else return false;
    atexit(mtc_leave);
    return true;
}
void mtc_leave()
{
    if (m_fp != NULL)
        fclose(m_fp);
    m_fp = NULL;
    memset(m_fn, 0x0, sizeof(m_fn));
}

bool mtc_msg(const char *func, const char *file, long line,
             int level, const char *format, ...)
{
    //int dftlv = hdf_get_int_value(m_cfg, PRE_CONFIG".trace_level", TC_DEFAULT_LEVEL);
    if (level > m_dftlv) return false;

    if (m_fp == NULL) return false;

    va_list ap;
    char tm[25] = {0};
    double usec = ne_timef();
    time_t sec = (time_t)usec;
    //mutil_getdatetime(tm, sizeof(tm), "%Y-%m-%d %H:%M:%S", time(NULL));
    struct tm *stm = localtime(&sec);
    strftime(tm, 25, "%Y-%m-%d %H:%M:%S", stm);
    tm[24] = '\0';

    fprintf(m_fp, "[%s %f]", tm, usec);
    fprintf(m_fp, "[%s]", m_trace_level[level]);
    fprintf(m_fp, "[%s:%li %s] ", file, line, func);

    va_start(ap, (void*)format);
    vfprintf(m_fp, format, ap);
    va_end(ap);

    fprintf(m_fp, "\n");

    trace_shift_file();
    return true;
}
