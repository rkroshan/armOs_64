#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "vector_t/exception_handler.h"
#include "fs/fs.h"
#include "utils/list.h"

/*process structure*/
struct Process {
	struct List *next; //holds the addr to next process in list
    int pid; /*hold the process id*/
	int wait; /*holds the amount of time user process to wait*/
	int state; /*holds process current state*/
	uint64_t context; /*holds the context of callee saved registers x19-x30*/
	uint64_t page_map; /*holds addr to process pgd*/
	uint64_t stack; /*holds the process stack addr*/
	struct TrapFrame *tf; /*holds the process context regs*/
};

/*process control struct*/
struct ProcessControl {
	struct Process *current_process; /*holds pointer to current running process*/
	struct HeadList ready_list;	/*list of process in ready state to run*/
	struct HeadList wait_list; /*list of process in wait list*/
};

/*stack size per process*/
#define STACK_SIZE (2*1024*1024)
/*max number of processes*/
#define NUM_PROC 10
/*process states*/
#define PROC_UNUSED 0
#define PROC_INIT 1
#define PROC_RUNNING 2
#define PROC_READY 3
#define PROC_SLEEP 4

/*initialize the process*/
void init_process(void);
/*gives the current process control*/
struct ProcessControl* get_pc(void);
/*func to yield the cpu by current running process*/
void yield(void);
/*func to swap the sp of current running process*/
void swap(uint64_t *prev, uint64_t next);
/*func to restore elr and spsr*/
void trap_return(void);
/*func to put current runnnig process to sleep*/
void sleep(int wait);
/*func to wake up the sleeping process back into to ready list*/
void wake_up(int wait);

#endif