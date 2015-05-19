#include "mheads.h"
//#include "packet.h"

HDF *g_cfg = NULL;

int main(int argc, char **argv, char **envp)
{
    unsigned char buf[2048];
    int blen = 2048, len;
    char *s;
    HDF *hdf, *ohdf = NULL;

    hdf = calloc(10, sizeof(HDF));
    memset(hdf, 0xee, 10*sizeof(HDF));

    printf("%d %d %d %d %d %d\n", sizeof(HDF), sizeof(HDF*), sizeof(void*),
	sizeof(uint32_t*), sizeof(uint16_t*), sizeof(char*));

    void *p = hdf;

    //*((HDF*)p) = 0x0;
    //memset(p, 0x0, sizeof(HDF));
    * (char*) p = &ohdf;
    printf("%d %d\n", p, (int)*(int*)p);
    p += sizeof(HDF);
    printf("%d %d\n", p, (int)*(int*)p);
    
    //mconfig_parse_file("/tpl/oms.hdf", &g_cfg);

#if 0
    mtimer_start();
    for (int i = 0; i < 100000; i++) {
        memset(buf, 2048, 0x0);
        len = pack_hdf(g_cfg, buf, blen);
        unpack_hdf(buf, len, &hdf);
        s = hdf_get_value(hdf, "manual.Layout", NULL);
        hdf_destroy(&hdf);
    }
    mtimer_stop(NULL);
#endif

    return 0;
}
