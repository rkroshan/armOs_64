#ifndef __UART_H__
#define __UART_H__

#define IO_BASE_ADDR    0x3f200000
/*UART DATA Register*/
#define UART0_DR        IO_BASE_ADDR + 0x1000
/*UART Flag Register*/
#define UART0_FR        IO_BASE_ADDR + 0x1018
/*UART Control Register*/
#define UART0_CR        IO_BASE_ADDR + 0x1030
/*UART Line Control Register*/
#define UART0_LCRH      IO_BASE_ADDR + 0x102c
/*UART Fractional Baud Rate Register*/
#define UART0_FBRD      IO_BASE_ADDR + 0x1028
/*UART Integer Baud Rate Register*/
#define UART0_IBRD      IO_BASE_ADDR + 0x1024
/*UART Interrupt Mask Control Register*/
#define UART0_IMSC      IO_BASE_ADDR + 0x1038

/*read a character from uart*/
unsigned char read_char(void);
/*write a char to uart*/
void write_char(unsigned char c);
/*write a string data to uart*/
void write_string(const char *string);
/*initialize uart*/
void init_uart(void);

#endif