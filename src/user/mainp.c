#include "user/lib/lib.h"

int main(void)
{
    char *p = (char*)0xffff000000001000; 
    *p = 'a';/*this is kernel addr space region, it will generate translation/permission fault a.k.a data abort*/

    printf("User process\r\n");
    sleepu(100);

    return 0;
}