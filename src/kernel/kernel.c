#include "uart/uart.h"
#include "utils/utils.h"
#include "utils/assert.h"

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

void kernel_main()
{
    /*uart testing*/
    uart_test();
    /*assert testing*/
    ASSERT(0);
    while(1)
    {
        //loop infinite
    }
}