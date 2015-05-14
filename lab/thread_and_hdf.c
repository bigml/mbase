#include "mheads.h"

HDF *g_cfg = NULL;

static pthread_t *m_thread[5];
static int m_num[5] = {1, 2, 3, 4, 5};
static size_t m_thread_size = 1;

static int m_counter = 0;
static HDF *m_node = NULL;
static char *m_str = NULL;

static void* hdf_routine(void *arg)
{
    int cnum = *(int*)arg;
    int a, b, c, d;
    HDF *nodea, *nodeb;

    a = b = c = d = 0;
    nodea = nodeb = NULL;

    errno = cnum;

    for (int i = 0; i < 10000; i++) {
        hdf_init(&nodea);
        //mtimer_start();
        //hdf_read_string(nodea, m_str);
        hdf_copy(nodea, NULL, m_node);
        //mtimer_stop("read string");
        //hdf_dump(nodea, NULL);
        hdf_destroy(&nodea);
    }

    return NULL;
}

int main(int argc, char **argv, char **envp)
{
    NEOERR *err;

    mtc_init("multi_thread_log", 7);

    hdf_init(&m_node);

    err = hdf_read_file(m_node, argv[1]);
    RETURN_V_NOK(err, 1);

    err = hdf_write_string(m_node, &m_str);
    RETURN_V_NOK(err, 1);

    mtimer_start();

#if 0
    for (int i = 0; i < m_thread_size; i++) {
        m_thread[i] = calloc(1, sizeof(pthread_t));
        pthread_create(m_thread[i], NULL, hdf_routine, (void*)&m_num[i]);
    }

    for (int i = 0; i < m_thread_size; i++) {
        pthread_join(*m_thread[i], NULL);
    }
#endif

    hdf_routine(&m_num[1]);

    elapsed = mtimer_stop("finish");

    printf("use %ld u seconds\n", elapsed);

    hdf_destroy(&m_node);
    free(m_str);

    return 0;
}
