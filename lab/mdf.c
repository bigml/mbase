#include "mheads.h"

HDF *g_cfg = NULL;

void hdf_copy_simulator(int number_of_nodes, int repeattime)
{
    char *name, *value;
    char *key, *val;
    int lenv, lenk;

    name = "url";
    value = "http://image.res.hunantv.com//";

    for (int k = 0; k < repeattime; k++) {
        for (int i = 0; i < number_of_nodes; i++) {

            lenk = strlen(name);
            lenv = strlen(value);

            key = malloc(lenk);
            val = malloc(lenv);

            memcpy(key, name, lenk);
            memcpy(val, value, lenv);
        }
    }
}

void mdf_copy_simulator(int number_of_nodes, int repeattime)
{
    /*
     * numof hdf node, hdf attribute, hash table, hash node
     * memory map:
     *
     * /-----------------\    \
     * | hdf node 1      |    |
     * | hdf node 2      |    |
     * | ...             |    |
     * | hdf node x      |    |
     * |-----------------|    |
     * | hdf attribute 1 |    |
     * | hdf attribute 2 |    |
     * | ...             |     > fix length for each unit
     * | hdf attribute x |    |
     * |-----------------|    |
     * | hash table 1    |    |
     * | hash table 2    |    |
     * | ...             |    |
     * | hash table x    |    |
     * |-----------------|    |
     * | hash node 1     |    |
     * | hash node 2     |    |
     * | ...             |    |
     * | hash node x     |    |
     * |-----------------|    /
     * |                 |
     * |-----------------|    \
     * | char string 1   |    |
     * | char string 2   |     > variable length for each unit
     * | ...             |    |
     * | char string x   |    |
     * \-----------------/    /
     */

    size_t len = 10000;
    unsigned char *oribuf = "xxx";
    unsigned char *buf = calloc(1, len);
    unsigned char *pos = buf;

    for (int k = 0; k < repeattime; k++) {
        //memcopy(buf, '', len);
        memset(buf, 0x0, len);

        pos = buf;
        /* hdf node */
        for (int i = 0; i < number_of_nodes; i++) {
            pos = pos + 2*sizeof(int);
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos = pos + sizeof(int);
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
        }

        pos = buf;
        /* hdf attribute */
        for (int i = 0; i < number_of_nodes; i++) {
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
        }

        pos = buf;
        /* hash table */
        for (int i = 0; i < number_of_nodes; i++) {
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
        }

        pos = buf;
        /* hash node */
        for (int i = 0; i < number_of_nodes; i++) {
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
            *pos = *pos + &buf - &oribuf;
            pos++;
        }
    }
}

int main(int argc, char **argv, char **envp)
{
    mtc_init("mdf", 7);

    mtimer_start();
    hdf_copy_simulator(1000, 100000);
    mtimer_stop("hdf copy simulator");

    mtimer_start();
    mdf_copy_simulator(1000, 100000);
    mtimer_stop("mdf copy simulator");

    return 0;
}
