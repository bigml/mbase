#include "mheads.h"

HDF *g_cfg = NULL;

static pthread_t *m_thread[5];
static int m_num[5] = {1, 2, 3, 4, 5};
static size_t m_thread_size = 1;

static int m_counter = 0;

static void* log_routine(void *arg)
{
    int cnum = *(int*)arg;
    int a, b, c, d;
    FILE *fp;
    char fname[56];

    a = b = c = d = 0;

    errno = cnum;

    snprintf(fname, sizeof(fname), "%d.log", cnum);
    fp = fopen(fname, "w");

    while (1) {
        for (int i = 0; i < 100000000; i++) {
            fprintf(fp, "%d%d%d%d hahahahahahahahahhahdsfkdfsa;dkfpowefkdjf;alksdjf;laksdjf\n", cnum, a+b,  b*c, a+b+c);
            fflush(fp);
        }
        break;
        //mtc_dbg("%d%d%d%d %s", cnum, cnum,  cnum, cnum, mongo_errno(cnum));
    }

    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    mtc_init("multi_thread_log", 7);

    mtimer_start();

    for (int i = 0; i < m_thread_size; i++) {
        m_thread[i] = calloc(1, sizeof(pthread_t));
        pthread_create(m_thread[i], NULL, log_routine, (void*)&m_num[i]);
    }

    for (int i = 0; i < m_thread_size; i++) {
        pthread_join(*m_thread[i], NULL);
    }

    elapsed = mtimer_stop("finish");

    printf("use %ld u seconds\n", elapsed);

    return 0;
}
