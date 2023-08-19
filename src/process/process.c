#include "process.h"
#include "utils/assert.h"
#include "utils/memory.h"
#include "utils/utils.h"
#include "fs/fs.h"
#include <stddef.h>

static struct Process process_table[NUM_PROC];
static int pid_num = 1; /*pid_num starts from 1 excluding idle process*/
void pstart(struct TrapFrame *tf);
uint64_t read_pgd();

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

    process = find_unused_process();
    ASSERT(process == &process_table[0]); /*it should be the first process*/

    process->state = PROC_RUNNING; /*set state as runnnig*/
    process->pid = 0;
    process->page_map = P2V(read_pgd()); 
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

    process = alloc_new_process();
    ASSERT(process != NULL);

    ASSERT(setup_uvm((uint64_t)process->page_map, "INIT.BIN")); /*setup the user process at the process map addr*/
}

/*launch user processes*/
void launch_user_process(void)
{
    /*load the user pgd into ttbr0_el1*/
    switch_vm(process_table[1].page_map);
    pstart(process_table[1].tf); /*this way it will eret to user mode el0 with return address as elr, as currently it is in el1*/
}

void init_process(void)
{
    init_idle_process(); /*initalize the idle process*/
    /*generally idle process only required ti be spawned but for example sake, it can a idle process as well*/
    init_user_process(); /*initialize the user process*/

    launch_user_process();
}