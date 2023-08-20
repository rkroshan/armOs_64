#include "syscall.h"
#include "utils/print.h"
#include "timer/generic_timer.h"
#include "process/process.h"
#include "keyboard/keyboard.h"

static SYSTEMCALL system_calls[20]; /*currently 10 syscalls*/

static int sys_write(int64_t *argptr)
{
    write_console((char*)argptr[0], (int)argptr[1]); /*since each entry on stack in uint64 so, 0 points to the buffer and 1 points to the buffer_size*/
    return (int)argptr[1]; /*return the buffer size printed*/
}

static int sys_sleep(int64_t *argptr)
{
    uint64_t ticks;
    uint64_t old_ticks;
    uint64_t sleep_ticks = argptr[0];

    ticks = get_ticks();
    old_ticks = ticks;

    while (ticks - old_ticks < sleep_ticks) { /*check if sleep ticks exceeds, this loop is necessary so that wakeup should not put this process into ready list before ticks exceeds*/
        sleep(-1); /*put the process in waiting list*/
        ticks = get_ticks();
    }

    return 0;
}

static int sys_exit(int64_t *argptr)
{
    exit();
    return 0;
}

static int sys_wait(int64_t *argptr)
{
    wait(argptr[0]);
    return 0;
}

static int sys_open_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return open_file(pc->current_process, (char*)argptr[0]);
}

static int sys_close_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    close_file(pc->current_process, argptr[0]);

    return 0;
}

static int sys_get_file_size(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();  
    return get_file_size(pc->current_process, argptr[0]);
}

static int sys_read_file(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return read_file(pc->current_process, argptr[0], (void*)argptr[1], argptr[2]);
}

static int sys_fork(int64_t *argptr)
{
    return fork();
}

static int sys_getpid(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    return pc->current_process->pid;
}

static int sys_exec(int64_t *argptr)
{
    struct ProcessControl *pc = get_pc();
    struct Process *process = pc->current_process; 

    return exec(process, (char*)argptr[0]);
}

static int sys_keyboard_read(int64_t *argptr)
{
    return read_key_buffer();
}

void system_call(struct TrapFrame *tf)
{
    int64_t i = tf->x8; /*SVC number*/
    int64_t param_count = tf->x0;   /*parameter count on stack*/
    int64_t *argptr = (int64_t*)tf->x1; /*pointer to stack where boths arguments are present*/

    if (param_count < 0 || i < 0 || i > 11) {
        printk("INVALID SVC\n");
        tf->x0 = -1;    /*if param count is neg then problem*/
        return;
    }

    tf->x0 = system_calls[i](argptr); /*execute the SVC*/
}

void init_system_call(void)
{
    /*initialize all syscalls in syscall array*/
    system_calls[0] = sys_write; /*to handle printf */
    system_calls[1] = sys_sleep; /*to handle sleep*/
    system_calls[2] = sys_exit; /*to handle exit*/
    system_calls[3] = sys_wait; /*to handle wait svc*/
    system_calls[4] = sys_open_file; /*to handle open file svc*/
    system_calls[5] = sys_close_file; /*close file svc*/
    system_calls[6] = sys_get_file_size; /*get file size svc*/
    system_calls[7] = sys_read_file; /*read file svc*/
    system_calls[8] = sys_fork; /*fork process svc*/
    system_calls[9] = sys_getpid; /*get current process pid svc*/
    system_calls[10] = sys_exec; /*exec svc, completely replaces the current process map and load new data onto same process to execute to return*/
    system_calls[11] = sys_keyboard_read; /*keyboard press svc*/
}
