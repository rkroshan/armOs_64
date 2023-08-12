#include "utils/memory.h"
#include "utils/assert.h"
#include "utils/print.h"
#include <stddef.h>

/*static struct to hold free page linked list*/
static struct Page free_memory;
extern char end; /*linker symbol to represent the end addr for kernel code, it can be less than or greater than 2MB*/

/*generic function to free pages from start pointer to end*/
static void free_region(uint64_t v, uint64_t e)
{
    /*align the start pointer to next page base*/
    for (uint64_t start = PA_UP(v); start + PAGE_SIZE <= e; start += PAGE_SIZE) {
        if (start + PAGE_SIZE <= HEAP_MEMORY_END) {
            /*making sure not to free region beyound HEAP_MEMORY_END*/
            kfree(start);
        }
    }
}

void kfree(uint64_t v)
{
    ASSERT(v % PAGE_SIZE == 0);
    ASSERT(v >= (uint64_t)&end);
    ASSERT(v + PAGE_SIZE <= HEAP_MEMORY_END);

    /*here free_memory will pint to latest page added in the list, similar to stack push*/
    struct Page *page_address = (struct Page*)v;
    page_address->next = free_memory.next;
    free_memory.next = page_address;
}

void* kalloc(void)
{
    struct Page *page_address = free_memory.next;

    if (page_address != NULL) {
        ASSERT((uint64_t)page_address % PAGE_SIZE == 0);
        ASSERT((uint64_t)page_address >= (uint64_t)&end);
        ASSERT((uint64_t)page_address + PAGE_SIZE <= HEAP_MEMORY_END);
        
        free_memory.next = page_address->next; /*similar to stack pop, free_memory struct will point to next free page in the list*/
    }

    return page_address;
}

/*utility function to print the amount of free memory we have in MBs based on number of free pages we have*/
void checkmm(void)
{
    struct Page *v = free_memory.next;
    uint64_t size = 0;
    uint64_t i = 0;

    while (v != NULL) {
        size += PAGE_SIZE;
        printk("%d base is %x \r\n", i++, v);
        v = v->next;
    }

    printk("memory size is %u \r\n", size/1024/1024);
}

void init_memory(void)
{
    free_region((uint64_t)&end, HEAP_MEMORY_END);
    checkmm();
}