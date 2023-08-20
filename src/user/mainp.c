#include "user/lib/lib.h"

int main(void)
{
    int fd;
    int size;
    char buffer[100] = { 0 };

    fd = open_file("TEXTFILE.TXT");
    if (fd == -1) {
        printf("open file failed\r\n");
    }
    else {
        size = get_file_size(fd);
        size = read_file(fd, buffer, size);
        printf("DATA READ: %s\r\n", buffer);
        printf("Read %dbytes in total \r\n", (int64_t)size);
    }

    close_file(fd);
    
    return 0;
}