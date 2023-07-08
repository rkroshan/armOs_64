#ifndef __UART_H__
#define __UART_H__

#include "irq/irq.h"

/*For Raspi 3b*/
#define UART_BASE_ADDR (IO_BASE_ADDR + 0x200000)

/*UART DATA Register*/
#define UART0_DR        UART_BASE_ADDR + 0x1000
/*UART Flag Register*/
#define UART0_FR        UART_BASE_ADDR + 0x1018
/*UART Control Register*/
#define UART0_CR        UART_BASE_ADDR + 0x1030
/*UART Line Control Register*/
#define UART0_LCRH      UART_BASE_ADDR + 0x102c
/*UART Fractional Baud Rate Register*/
#define UART0_FBRD      UART_BASE_ADDR + 0x1028
/*UART Integer Baud Rate Register*/
#define UART0_IBRD      UART_BASE_ADDR + 0x1024
/*UART Interrupt Mask Control Register*/
#define UART0_IMSC      UART_BASE_ADDR + 0x1038
/*UART Interrupt Mask Interrupt Status Register*/
#define UART0_MIS       UART_BASE_ADDR + 0x1040
/*UART Interrupt Clear Register*/
#define UART0_ICR       UART_BASE_ADDR + 0x1044

/*read a character from uart*/
unsigned char read_char(void);
/*write a char to uart*/
void write_char(unsigned char c);
/*write a string data to uart*/
void write_string(const char *string);
/*initialize uart*/
void init_uart(void);
/*Uart Interrupt Handler*/
void uart_interrupt_handler(void);

#endif