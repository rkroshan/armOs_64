#include "uart.h"
#include "utils/utils.h"

void write_char(unsigned char c)
{
    while (inw(UART0_FR) & (1 << 5)) { } /*loop till TX FIFO is Full*/
    outw(UART0_DR, c); /*Write a byte in Data Register*/
}

unsigned char read_char(void)
{
    while (inw(UART0_FR) & (1 << 4)) { } /*loop till RX FIFO is empty*/
    return inw(UART0_DR); /*Read a byte from Data Register*/
}

void write_string(const char *string)
{
    /*No Explaination need write char multiple times*/
    for (int i = 0; string[i] != '\0'; i++) {
        write_char(string[i]);
    }
}

void init_uart(void)
{
    outw(UART0_CR, 0); /*disabling Uart*/
    outw(UART0_IBRD, 26); /*setting baud rate as 115200,  BAUDDIV = 48mhz/(16*115200) = 26 I and 0 F*/
    outw(UART0_FBRD, 0);
    outw(UART0_LCRH, (1 << 5) | (1 << 6)); /*8 bit data transmit*/
    outw(UART0_IMSC, (1 << 4)); /*Enable Receive interrupt*/
    outw(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9)); /*Enable UART, Transmit Enable Recievce Enable*/
}

void uart_interrupt_handler(void)
{
    uint32_t status = inw(UART0_MIS);

    if (status & (1 << 4)) { /*checking the RXMIS reg value*/
        char ch = read_char();

        if (ch == '\r') {
            write_string("\r\n");
        }
        else {
            write_char(ch);
        }
        
        outw(UART0_ICR, (1 << 4)); /*clear RXMIS reg since all are level triggered interrupt*/
    }
}
