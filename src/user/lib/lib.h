#ifndef _LIB_H_
#define _LIB_H_

#include <stdint.h>

int printf(const char *format, ...);
void sleepu(uint64_t ticks);
void exit(void);
void waitu(int64_t pid);
int open_file(char *name);
void close_file(int fd);
int get_file_size(int fd);
int read_file(int fd, void *buffer, uint32_t size);
int fork(void);
int getpid(void);
int exec(char *file_name);
#endif