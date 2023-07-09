#include "generic_timer.h"
#include "utils/utils.h"

static uint32_t timer_interval = 0;
static uint64_t ticks = 0;

void init_timer(void)
{
#ifdef RASPI3B
    timer_interval = read_timer_freq() / 100;
    enable_timer();
    outw(CNTP_EL0, (1 << 1));       //enable Core 0 generic timer
#endif
#ifdef RASPI4
    outw(TIMER_PREDIV, 0x7d); /* TIMER_LOAD = (1/(250MhZ)=apb_clock)/((pre_divider=0x7d)+1)*/
    outw(TIMER_LOAD, 19841);
    outw(TIMER_CTL, 0b10100010); /*Enable Timer, Enable Interrupt, Enable 32 bit counter*/
#endif
}

void timer_interrupt_handler(void)
{
    uint32_t status;
    #ifdef RASPI3B
    status = read_timer_status();
    #elif RASPI4
    status = inw(TIMER_MSKIRQ);
    #endif
    if (status & (1 << 2)) {        //check if status bit in control reg is set, then it is an irq
        ticks++;
        if (ticks % 100 == 0) {
            printk("timer irq: %d \r\n", ticks);
        }
        #ifdef RASPI3B
        set_timer_interval(timer_interval); //then we reset the timer interval, since TVAL reg dec as CNTP increments so need to reset
        #elif RASPI4
        outw(TIMER_ACK, 1); //in Raspi4 it is repeated timer no need to reset it, just ack the timer it will reset
        #endif
    }
}