#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "vector_t/exception_handler.h"
#include "fs/fs.h"

/*process structure*/
struct Process {
    int pid; /*hold the process id*/
	int state; /*holds process current state*/
	uint64_t page_map; /*holds addr to process pgd*/
	uint64_t stack; /*holds the process stack addr*/
	struct TrapFrame *tf; /*holds the process context regs*/
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

/*initialize the process*/
void init_process(void);

#endif