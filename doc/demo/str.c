#include "mheads.h"

HDF *g_cfg = NULL;

/*
 * const char *s = "xxx"; 传给repchr(char *s) 调用，编译时会报警
 * discards qualifiers from pointer target type

 * char s[100] = "xxx"; 传给 mstr_ulen(const char *s)调用，编译运行都正常
 */

int main(int argc, char **argv, char **envp)
{
    const char *s = "ha我爱你";
    //char s[100];

    //strcpy(s, "ha我爱你 ");

    //printf("%d\n", mstr_upos2len(s, 2));
    //printf("%s\n", mstr_repchr(s, 'a', 'b'));

    printf("%d\n", mstr_version_compare(NULL, NULL));
    printf("%d\n", mstr_version_compare(NULL, ""));
    printf("%d\n", mstr_version_compare("", NULL));
    printf("%d\n", mstr_version_compare("", ""));
    printf("%d\n", mstr_version_compare("1", "1"));
    printf("\n");

    printf("%d\n", mstr_version_compare("1", "2"));
    printf("%d\n", mstr_version_compare("1.1.3.27", "1.1.3.12"));
    printf("%d\n", mstr_version_compare("4.1.2", "4.2"));
    printf("%d\n", mstr_version_compare("4.2", "4.2.1"));
    printf("%d\n", mstr_version_compare("4.2.1", "4.2"));
    printf("%d\n", mstr_version_compare("1.1.3.27", "1.1.3.27"));

    printf("\n");
    printf("%d\n", mstr_version_compare("1..1.3.27", "1.1.3.27"));
    printf("%d\n", mstr_version_compare("1.1.3.27", ".1.1.3.27"));
    printf("%d\n", mstr_version_compare(".1.1.3.27", ".1.1.3.27"));

    return 0;
}
