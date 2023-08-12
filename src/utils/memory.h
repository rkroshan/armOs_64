#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

/*
Memory Division:
0-2MB : kernel memory
2MB-0x30000000 : Free Memory for HEAP
0x30000000 - 0x34000000: Filesystem
0x34000000 - 1GB : Peripheral Mem
#0x40000000 - #0x41000000 : Peripheral Mem
*/

#define KERNEL_BASE 0xffff000000000000

#define P2V(p) ((uint64_t)(p) + KERNEL_BASE)
#define V2P(v) ((uint64_t)(v) - KERNEL_BASE)

/*Heap memory page struct, idea is to store free pages in a linked list fashion*/
struct Page {
    struct Page* next;
};

#define HEAP_MEMORY_END     P2V(0x30000000)
#define PAGE_SIZE           (2*1024*1024)

/*Page align up PA_UP can also be done as (((uint64_t)v + PAGE_SIZE - 1) & 0x1FFFFF)*/
#define PA_UP(v)    ((((uint64_t)v + PAGE_SIZE - 1) >> 21) << 21)
/*Page align down PA_DOWN can also be done as ((uint64_t)v & 0x1FFFFF)*/
#define PA_DOWN(v)  (((uint64_t)v >> 21) << 21)

/*allocate and return pointer to in Page of size PAGE_size else NULL*/
void* kalloc(void);
/*put the page back into free up page linked list*/
void kfree(uint64_t v);
/*functional call to setup free up memory linked list and check amount of free mem we have as heap*/
void init_memory(void);

#endif