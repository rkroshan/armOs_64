#include "generic_timer.h"
#include "utils/utils.h"

static uint32_t timer_interval = 0;
static uint64_t ticks = 0;

void init_timer(void)
{
    timer_interval = read_timer_freq() / 100;
    enable_timer();
    outw(CNTP_EL0, (1 << 1));       //enable Core 0 generic timer
}

void timer_interrupt_handler(void)
{
    uint32_t status = read_timer_status();
    if (status & (1 << 2)) {        //check if status bit in control reg is set, then it is an irq
        ticks++;
        if (ticks % 100 == 0) {
            printk("timer irq: %d \r\n", ticks);
        }

        set_timer_interval(timer_interval); //then we reset the timer interval, since TVAL reg dec as CNTP increments so need to reset
    }
}