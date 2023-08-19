#include "syscall.h"
#include "utils/print.h"

static SYSTEMCALL system_calls[10]; /*currently 10 syscalls*/

static int sys_write(int64_t *argptr)
{
    write_console((char*)argptr[0], (int)argptr[1]); /*since each entry on stack in uint64 so, 0 points to the buffer and 1 points to the buffer_size*/
    return (int)argptr[1]; /*return the buffer size printed*/
}

void system_call(struct TrapFrame *tf)
{
    int64_t i = tf->x8; /*SVC number*/
    int64_t param_count = tf->x0;   /*parameter count on stack*/
    int64_t *argptr = (int64_t*)tf->x1; /*pointer to stack where boths arguments are present*/

    if (param_count < 0 || i != 0) {
        tf->x0 = -1;    /*if param count is neg then problem*/
        return;
    }

    tf->x0 = system_calls[i](argptr); /*execute the SVC*/
}

void init_system_call(void)
{
    /*initialize all syscalls in syscall array*/
    system_calls[0] = sys_write; /*to handle printf */
}
