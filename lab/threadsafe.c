#include "mheads.h"

HDF *g_cfg = NULL;

static pthread_rwlock_t m_lock = PTHREAD_RWLOCK_INITIALIZER;

static int m_number = 0;
static pthread_t *m_thread[5];
static int m_num[5] = {1, 2, 3, 4, 5};
static size_t m_thread_size = 5;

static void* log_routine(void *arg)
{
    int cnum = *(int*)arg;

    errno = cnum;

    while (1) {
        pthread_rwlock_rdlock(&m_lock);

        printf("read thread I'm %d thread number : %d\n", cnum, m_number);
        //mtc_dbg("errno from child %d %d%d%d%d %s ",
        //cnum, cnum, cnum, cnum, errno, strerror(errno));

        //mtc_dbg("%d%d%d%d %s", cnum, cnum,  cnum, cnum, mongo_errno(cnum));
        usleep(1000);

        pthread_rwlock_unlock(&m_lock);

        usleep(1000);
    }

    return NULL;
}

static void * write_routine(void *arg)
{
    int cnum = *(int*)arg;

    while (1) {
        pthread_rwlock_wrlock(&m_lock);

        m_number++;

        printf("write thread I'm %d thread number : %d\n", cnum, m_number);

        usleep(1000);

        pthread_rwlock_unlock(&m_lock);
        sleep(1);
    }
}

int main(int argc, char **argv, char **envp)
{
    mtc_init("multi_thread_log", 7);

    for (int i = 0; i < 4; i++) {
        m_thread[i] = calloc(1, sizeof(pthread_t));
        pthread_create(m_thread[i], NULL, log_routine, (void*)&m_num[i]);
    }

    m_thread[4] = calloc(1, sizeof(pthread_t));
    pthread_create(m_thread[4], NULL, write_routine, (void*)&m_num[4]);

    for (int i = 0; i < m_thread_size; i++) {
        pthread_join(*m_thread[i], NULL);
    }

    return 0;
}
