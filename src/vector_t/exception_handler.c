#include "exception_handler.h"
#include "utils/utils.h"
#include "timer/generic_timer.h"
#include "irq/irq.h"
#include "uart/uart.h"
#include "process/process.h"
#include "syscall/syscall.h"

void exception_handler(struct TrapFrame *tf)
{
    uint32_t irq;
    uint8_t schedule=0;

    switch (tf->trapno) {
        case 1:
            printk("sync error at elr:%x esr:%x\r\n", tf->elr, tf->esr);
            while (1) { }
            break;
        case 2:
            irq = inw(CNTP_STATUS_EL0); //check the interrupt source enabled for this core
            if (irq & (1 << 1)) {           //check if NSIRQ is enabled for this core
                timer_interrupt_handler();
                schedule = 1;
            }
            else if ( get_irq_number() & (1 << 19))
            {
                //it is Uart Interrupt
                uart_interrupt_handler();
            }
            else {
                printk("unknown irq \r\n");
                while (1) { }
            }
            break;
        case 3:
            system_call(tf);
            break;    
        default:
            printk("unknown exception\r\n");
            while (1) { }
    }

    if (schedule == 1) {
        yield();
    }
}