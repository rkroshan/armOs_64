#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "vector_t/exception_handler.h"

/*function pointer to various syscalls*/
typedef int (*SYSTEMCALL)(int64_t *argptr);
/*initialize all syscalls*/
void init_system_call(void);
/*syscall exception handler*/
void system_call(struct TrapFrame *tf);

#endif