#include <stdint.h>
#include "utils/print.h"

void exception_handler(uint64_t numid, uint64_t esr, uint64_t elr)
{
    switch (numid) {
        case 1:
            printk("sync error at %x: %x\r\n", elr, esr);
            while (1) { }
            
        default:
            printk("unknown exception\r\n");
            while (1) { }
    }
}