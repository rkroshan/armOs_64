#include "user/lib/lib.h"

int main(void)
{
    int pid;

    pid = fork();
    if (pid == 0) {
        printf("This is the new process pid:%d\r\n", getpid());
    }
    else {
        printf("This is the current process pid: %d\r\n", getpid());
        waitu(pid); /*since it is the parent process it will get pid as child process id*/
    }
    
    return 0;
}