#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdbool.h>
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

/*Page Global Directory entry addr*/
#define PDE_ADDR(p) ((uint64_t)p & 0xfffffffff000)
/*Physical Page entry addr*/
#define PTE_ADDR(p) ((uint64_t)p & 0xffffffe00000)

/*Memory Attributes*/
/*Valid Entry*/
#define ENTRY_V         (1 << 0)
/*Nested Table Entry*/
#define TABLE_ENTRY     (1 << 1)
/*Bloack Entry i.e pointing to physical page*/
#define BLOCK_ENTRY     (0 << 1)
/*is entry accessible ?*/
#define ENTRY_ACCESSED  (1 << 10)
/*Memory type as Normal memory*/
#define NORMAL_MEMORY   (1 << 2)
/*Memory type as Device Memory*/
#define DEVICE_MEMORY   (0 << 2)
/*Memory access as User*/
#define USER            (1 << 6)

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
/*similar to mmap, map virtual addr to physical addr along with attributes, map contains ttbr*/
bool map_page(uint64_t map, uint64_t v, uint64_t pa, uint64_t attribute);
/*setup ttbr0_el1*/
void switch_vm(uint64_t map);
/*map the file data onto VA 0x400000 getting pgd from map VA*/
bool setup_uvm(uint64_t map, char *file_name);
/*Free the Physical page mapped to the VA*/
void free_page(uint64_t map, uint64_t vstart);
/*Free the complete VM memory along with Page Tables*/
void free_vm(uint64_t map);

#endif