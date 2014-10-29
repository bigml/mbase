#include "mheads.h"

HDF *g_cfg = NULL;

static pthread_t *m_thread[5];
static int m_num[5] = {1, 2, 3, 4, 5};
static size_t m_thread_size = 5;

static void* log_routine(void *arg)
{
    int cnum = *(int*)arg;

    errno = cnum;

    while (1) {
        mtc_dbg("errno from child %d %d%d%d%d %s ",
                cnum, cnum, cnum, cnum, errno, strerror(errno));

        //mtc_dbg("%d%d%d%d %s", cnum, cnum,  cnum, cnum, mongo_errno(cnum));
    }

    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    mtc_init("multi_thread_log", 7);

    for (int i = 0; i < m_thread_size; i++) {
        m_thread[i] = calloc(1, sizeof(pthread_t));
        pthread_create(m_thread[i], NULL, log_routine, (void*)&m_num[i]);
    }

    for (int i = 0; i < m_thread_size; i++) {
        pthread_join(*m_thread[i], NULL);
    }

    return 0;
}
