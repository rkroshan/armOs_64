#include "utils/memory.h"
#include "utils/assert.h"
#include "utils/print.h"
#include "utils/utils.h"
#include <stddef.h>

/*static struct to hold free page linked list*/
static struct Page free_memory;
extern char end; /*linker symbol to represent the end addr for kernel code, it can be less than or greater than 2MB*/

void load_pgd(uint64_t map); /*load ttbr0_el1 with pointer to pgd*/

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

static uint64_t* find_pgd_entry(uint64_t map, uint64_t v, int alloc, uint64_t attribute)
{
    uint64_t *ptr = (uint64_t*)map; /*ttbr value = &pgd[0]*/
    void *addr = NULL;
    unsigned int index = (v >> 39) & 0x1ff; /*get the pgd index*/

    if (ptr[index] & ENTRY_V) { /*check if index is valid*/
        addr = (void*)P2V(PDE_ADDR(ptr[index])); /*return the VA and clearing the attributes*/
    }
    else if (alloc == 1) { /*if entyr is not valid and asked to allocate entry*/
        addr = kalloc(); /*allocate one page = PAGE_size = 4096 bytes = 1 PGD  = 4096 PUD entries*/
        if (addr != NULL) {
            memset(addr, 0, PAGE_SIZE); /*clear the mem data*/
            ptr[index] = (V2P(addr) | attribute | TABLE_ENTRY); /*set PA with attr and mark it as a nested table entry*/
        }
    }

    return addr;
}

static uint64_t* find_pud_entry(uint64_t map, uint64_t v, int alloc, uint64_t attribute)
{
    uint64_t *ptr = NULL;
    void *addr = NULL;
    unsigned int index = (v >> 30) & 0x1ff; /*get the PUD index from VA*/

    ptr = find_pgd_entry(map, v, alloc, attribute); /* get the PGD entry from ttbr*/
    if (ptr == NULL) {
        return NULL;
    }

    if (ptr[index] & ENTRY_V) { /*if PUD entry is valid return the addr*/
        addr = (void*)P2V(PDE_ADDR(ptr[index])); /* since value the PUD index contains is physical addr, so need to convert to VA, also clearing the attributes and then return addr*/
    }
    else if (alloc == 1) { /*if entyr is not valid and asked to allocate entry*/
        addr = kalloc(); /*allocate one page = PAGE_size = 4096 bytes = 1 PUD  = 4096 PMD entries*/
        if (addr != NULL) { 
            memset(addr, 0, PAGE_SIZE); /*clear the mem data*/
            ptr[index] = (V2P(addr) | attribute | TABLE_ENTRY); /*set PA with attr and mark it as a nested table entry*/
        }
    }

    return addr;
}

bool map_page(uint64_t map, uint64_t v, uint64_t pa, uint64_t attribute)
{
    uint64_t vstart = PA_DOWN(v); /*get the base page addr for the virtual addr*/
    uint64_t *ptr = NULL;

    ASSERT(vstart + PAGE_SIZE < HEAP_MEMORY_END); /*it should not exceed HEAP memory*/
    ASSERT(pa % PAGE_SIZE == 0);   /*pa should be Page align addr*/
    ASSERT(pa + PAGE_SIZE <= V2P(HEAP_MEMORY_END)); /*it should exceed HEAP memory*/

    ptr = find_pud_entry(map, vstart, 1, attribute); /*find the PUD entry*/
    if (ptr == NULL) {
        return false; 
    }

    unsigned int index = (vstart >> 21) & 0x1ff; /*get the pmd index from the VA*/
    ASSERT((ptr[index] & ENTRY_V) == 0); /*make sure the entry is not valid*/ 

    ptr[index] = (pa | attribute | BLOCK_ENTRY); /*set the value for this addr as PA with attr provided and set it as Block entry*/

    return true;
}

bool setup_uvm(void)
{
    bool status = false;

    return status;
}

void switch_vm(uint64_t map)
{
    load_pgd(V2P(map));
}

void init_memory(void)
{
    free_region((uint64_t)&end, HEAP_MEMORY_END);
    // checkmm();
}