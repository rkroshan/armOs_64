#include "uart/uart.h"
#include "utils/utils.h"
#include "utils/assert.h"
#include "timer/generic_timer.h"
#include "irq/irq.h"
#include "utils/memory.h"
#include "fs/fs.h"
#include "syscall/syscall.h"
#include "process/process.h"
#include <stddef.h>

/*uart testing*/
void uart_test(void)
{
    init_uart();
    write_string("Welcome! Uart is working \nHola!!\n");
    printk("Using Printk Function !!\r\n");
    printk("Testing Hex val: %x\n",0x0123456789ABCDEF);
    printk("Testing unsigned val: %u\n",2);
    printk("Testing signed val: %d\n",-4); /*%d will only support int32*/
    printk("Testing string : %s\n","this is pretty cool\n");
}

/*Synchronous Error handling testing*/
void sync_error_test(void)
{
    char* p = (char*)0xffff000000000000;
    *p = 1; //not possible address is out of range
    printk("This message shouldn't be printed\r\n");
}

/*generic timer test*/
void generic_timer_test(void)
{
    init_timer();
}

/*Enable Interrupt Controller*/
void IC_init()
{
    init_interrupt_controller();
}

/*initialize free heap memory list*/
void HMem_init()
{
    init_memory();
}

/*fat16 file system bootup test*/
void fs16_init()
{
    init_fs();
}

void kernel_main()
{
    /*check current exception level*/
    printk("Current EL is EL%u\n",(uint64_t)get_el());

    /*uart testing*/
    uart_test();

    /*Heap Free Memory Init*/
    HMem_init();
    
    /*Enable Interrupt Controller*/
    IC_init();

    /*fat16 file system bootup test*/
    fs16_init();

    /*init_system_call();*/
    init_system_call();

    /*assert testing*/
    ASSERT(1);

    /*Sync error test*/
    // sync_error_test();

    /*generic timer test*/
    generic_timer_test();

    /*Enable irq*/
    enable_irq();

    /*init_process();*/
    init_process();
}