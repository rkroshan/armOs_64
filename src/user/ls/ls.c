#include "user/lib/lib.h"

struct DirEntry buffer[1024];

int main(void)
{
    int count;
    char name[10] = { 0 };

    count = read_root_directory(buffer); /*get the root directory entires into buffer*/

    if (count != 0) {
        printf("\r\nNAME          DIR          FILE SIZE \r\n");
        printf("-------------------------------------------\r\n");
        for (int i = 0; i < count; i++) { /*if entry is available, or deleted or long format or label then continue*/
            if (buffer[i].name[0] == ENTRY_AVAILABLE || 
                buffer[i].name[0] == ENTRY_DELETED || 
                buffer[i].attributes == 0xf ||
                buffer[i].attributes == 8) {
                continue;
            }

            memcpy(name, buffer[i].name, 8); /*copy the entry name*/
            if ((buffer[i].attributes & 0x10) == 0x10) { /*if directory*/
                printf("%s      YES          %ub\r\n", name, (uint64_t)buffer[i].file_size);
            }
            else 
                printf("%s      NO           %ub\r\n", name, (uint64_t)buffer[i].file_size);
        }      
    }

    return 0;
}