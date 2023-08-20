#include "user/lib/lib.h"

static int read_cmd(char *buffer)
{
    char ch[4] = { 0 };
    int buffer_size = 0;

    while (1) {
        ch[0] = keyboard_read(); /*svc for keyboard read*/
        if (ch[0] == '\r' || buffer_size > 80) { /*if key is enter or buffer size exceeds 80*/
            if (ch[0] == '\r') { /*set the enter chars and print it to get new line*/
                ch[0] = '\r';
                ch[1] = '\n';
            }

            printf("%s", ch);
            ch[1] = 0;
            break;
        }
        else if (ch[0] == 127) { /*it means it is a backspace*/
            if (buffer_size > 0) { /*if buffer exist*/
                buffer[--buffer_size] = 0; /*clear the old char*/
                ch[0] = '\b'; /*clear the old data from screen via backspace*/
                ch[1] = ' ';
                ch[2] = '\b';
                printf("%s", ch); /*print to clear*/
                ch[1] = 0;
                ch[2] = 0;
            }
        }
        else {
            buffer[buffer_size++] = ch[0]; /*else get the char and print it*/
            printf("%s", ch);
        }
    }

    return buffer_size;
}

int main(void)
{
    char buffer[100] = { 0 };
    int buffer_size = 0;

    while (1) {
        printf("shell# ");
        memset(buffer, 0, sizeof(buffer));
        buffer_size = read_cmd(buffer);
        if (buffer_size == 0) { /*if nothing in buffer loop again and print the ususal shell prompt*/
            continue;
        }

        int fd = open_file(buffer); /*we expect use needs to run the file input via keyboard*/
        if (fd == -1) {
            printf("No Such file\r\n"); /*if fd doesn't exist print error*/
        }
        else {
            close_file(fd); /*else close the file*/
            int pid = fork(); /*fork a new process*/
            if (pid == 0) {
                exec(buffer); /*exec the file found*/
            }
            else {
                waitu(pid); /*free up the child process resource, since it is not smp configuration it will work*/
            }
        }
    }

    return 0;
}