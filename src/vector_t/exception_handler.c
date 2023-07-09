#include <stdint.h>
#include "utils/utils.h"
#include "timer/generic_timer.h"
#include "irq/irq.h"
#include "uart/uart.h"

void exception_handler(uint64_t numid, uint64_t esr, uint64_t elr)
{
    uint32_t irq;
    switch (numid) {
        case 1:
            printk("sync error at %x: %x\r\n", elr, esr);
            while (1) { }
            break;
        case 2:
            #ifdef RASPI3B
            irq = inw(CNTP_STATUS_EL0); //check the interrupt source enabled for this core
            if (irq & (1 << 1)) {           //check if NSIRQ is enabled for this core
                timer_interrupt_handler();
            }
            else if (get_irq_number() & (1 << 19))
            {
                //it is Uart Interrupt
                uart_interrupt_handler();
            }
            else {
                printk("unknown irq \r\n");
                while (1) { }
            }
            #elif RASPI4
            irq = get_irq_number();
            if (irq == 64) {           //check the ICC_ACK reg for int number
                timer_interrupt_handler();
            }
            else if ( irq == (96 + 57)) //uart interrupt
            {
                //it is Uart Interrupt
                uart_interrupt_handler();
            }
            else {
                printk("unknown irq \r\n");
                while (1) { }
            }

            outw(ICC_EOI, irq); //Set EOI for the irq to deasset it
            #endif
            break;    
        default:
            printk("unknown exception\r\n");
            while (1) { }
    }
}