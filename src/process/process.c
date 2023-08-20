#include "process.h"
#include "utils/assert.h"
#include "utils/memory.h"
#include "utils/utils.h"
#include "fs/fs.h"
#include <stddef.h>

static struct Process process_table[NUM_PROC];
static int pid_num = 1; /*pid_num starts from 1 excluding idle process*/
static struct ProcessControl pc;
void pstart(struct TrapFrame *tf);
uint64_t read_pgd();

struct ProcessControl* get_pc(void)
{
    return &pc;
}

static struct Process* find_unused_process(void)
{
    struct Process *process = NULL;

    for (int i = 0; i < NUM_PROC; i++) {
        if (process_table[i].state == PROC_UNUSED) { /*looks for unused pid index and gives that*/
            process = &process_table[i];
            break;
        }
    }

    return process;
}


static void init_idle_process(void)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process = find_unused_process();
    ASSERT(process == &process_table[0]); /*it should be the first process*/

    process->state = PROC_RUNNING; /*set state as runnnig*/
    process->pid = 0;
    process->page_map = P2V(read_pgd());

    process_control = get_pc();
    process_control->current_process = process; 
}

static struct Process* alloc_new_process(void)
{
    struct Process *process;

    process = find_unused_process(); /*find unused pid process*/

    process->stack = (uint64_t)kalloc(); /*allocate a stack memory for this process, return VA*/
    ASSERT(process->stack != 0);

    memset((void*)process->stack, 0, PAGE_SIZE); /*clear the mem*/

    process->state = PROC_INIT; /*set process as init*/
    process->pid = pid_num++; /*increment the pid*/

    process->context = process->stack + PAGE_SIZE - sizeof(struct TrapFrame) - 12*8; /*space for 12 calle saved registers*/
    *(uint64_t*)(process->context + 11*8) = (uint64_t)trap_return; /*setting x30 to trap_return function so that on ret pc is set to trapreturn*/
    process->tf = (struct TrapFrame*)(process->stack + PAGE_SIZE - sizeof(struct TrapFrame)); /*set pointer to trapframe at top of stack, so pointer to stack will point to trapframe as well*/
    process->tf->elr = 0x400000; /*this is a VA where every user process will be mapped to return to el0*/
    process->tf->sp0 = 0x400000 + PAGE_SIZE; /*and the stack pointer will starting from one page above*/
    process->tf->spsr = 0; /*switch to el0, with interrupts enabled*/

    process->page_map = (uint64_t)kalloc(); /*actual VA addr where this process will be mapped*/
    ASSERT(process->page_map != 0);

    memset((void*)process->page_map, 0, PAGE_SIZE); /*clear the memory allocated, TODO: create KZALLOC*/

    return process;
}

static void init_user_process(void)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process = alloc_new_process();
    ASSERT(process != NULL);

    ASSERT(setup_uvm((uint64_t)process->page_map, "INIT.BIN")); /*setup the user process at the process map addr*/

    process_control = get_pc(); /*get the process control*/
    list = &process_control->ready_list; /*put the process in ready list*/

    process->state = PROC_READY; /*proc should be ready state*/
    append_list_tail(list, (struct List*)process); /*add the process in the process control ready list*/
}

static void switch_process(struct Process *prev, struct Process *current)
{
    switch_vm(current->page_map); /*load the ttb0_el1 with current process pgd*/
    swap(&prev->context, current->context); /*swap the sp*/
}

static void schedule(void)
{
    struct Process *prev_proc;
    struct Process *current_proc;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc(); /*get the current process control*/
    list = &process_control->ready_list;
    prev_proc = process_control->current_process;

    if (is_list_empty(list)) { //we should not come here via yield 
        current_proc = &process_table[0]; //set the current process to idle process
    }
    else {
        current_proc = (struct Process*)remove_list_head(list); //set the current procee as process on list head
    }

    current_proc->state = PROC_RUNNING; //set state as running
    process_control->current_process = current_proc;

    switch_process(prev_proc, current_proc); //switch the process
}

void yield(void)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc(); /*get the current process control*/
    list = &process_control->ready_list;

    if (is_list_empty(list)) { /*if list is empty then return so as to continue running the current ptocess*/
        return;
    }

    process = process_control->current_process; /*take the current process and put it into ready state*/
    process->state = PROC_READY;

    if (process->pid != 0) {
        append_list_tail(list, (struct List*)process); /*add the current process into ready list*/
    }

    schedule(); /*schedule to switch to another process*/
}

void init_process(void)
{
    init_idle_process(); /*initalize the idle process*/
    /*generally idle process only required ti be spawned but for example sake, it can a idle process as well*/
    init_user_process(); /*initialize the user process*/
}
/*TODO: there is a better way to go to sleep where ticks actually set to future tick when they should wake up, so that wake up just check for wait time exceed current and add the process into list rather than continuously waking and waiting and runinng list jagron*/
void sleep(int wait)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process_control = get_pc();
    process = process_control->current_process;
    process->state = PROC_SLEEP;
    process->wait = wait;
    /*put the process in wait list*/
    append_list_tail(&process_control->wait_list, (struct List*)process);
    schedule();
}

void wake_up(int wait)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *ready_list;
    struct HeadList *wait_list;

    process_control = get_pc();
    ready_list = &process_control->ready_list;
    wait_list = &process_control->wait_list;

    process = (struct Process*)remove_list(wait_list, wait); /*remove the process in wait==wait*/

    while (process != NULL) { /*do it for all processes if any other is waiting for same wait as well*/
        process->state = PROC_READY;
        append_list_tail(ready_list, (struct List*)process);
        process = (struct Process*)remove_list(wait_list, wait);
    }
}

void exit(void)
{
    struct Process *process;
    struct ProcessControl *process_control;

    process_control = get_pc();
    process = process_control->current_process; /*get the current process*/
    process->state = PROC_KILLED; /*set state to killed*/
    process->wait = process->pid; /*wait = pid, for simplying the search*/
    /*add the process in killed list*/
    append_list_tail(&process_control->kill_list, (struct List*)process);

    wake_up(-3); /*wake up any process waiting till current ticks, -3 is dummy*/
    schedule();
}

void wait(int pid)
{
    struct Process *process;
    struct ProcessControl *process_control;
    struct HeadList *list;

    process_control = get_pc();
    list = &process_control->kill_list;

    while (1) {
        if (!is_list_empty(list)) { /*if killed list is not empty*/
            process = (struct Process*)remove_list(list, pid); /*look out for process with pid mentioned in wait */
            if (process != NULL) {
                ASSERT(process->state == PROC_KILLED);
                kfree(process->stack);  /*free up stack*/
                free_vm(process->page_map); /*free up transaltion tables*/
                memset(process, 0, sizeof(struct Process)); /*clean the process struct*/
                break;
            }
        }

        sleep(-3); /*sleep the current process for -3 ticks, means wake it up on wake_up call*/
    }
}