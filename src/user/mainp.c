#include "user/lib/lib.h"

int main(void)
{
    while (1) {
        printf("User process\r\n");
        sleepu(100); /*sleep for 10ms*100 = 1s*/
    }

    return 0;
}