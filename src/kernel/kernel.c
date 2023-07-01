#include "uart/uart.h"

/*uart testing*/
void uart_test(void)
{
    init_uart();
    write_string("Welcome! Uart is working \nHola!!\n");
}

void kernel_main()
{
    /*uart testing*/
    uart_test();
    while(1)
    {
        //loop infinite
    }
}