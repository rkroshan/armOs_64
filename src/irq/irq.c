#include "irq.h"
#include "utils/utils.h"

void init_interrupt_controller(void)
{
    /*disable all IRQs*/
    outw(DISABLE_BASIC_IRQS, 0xffffffff);
    outw(DISABLE_IRQS_1, 0xffffffff);
    outw(DISABLE_IRQS_2, 0xffffffff);
    /*Enable uart interrupt, refer page 113 doc bcm2835 arm peripherals*/
    outw(ENABLE_IRQS_2, (1 << 25));
}

uint32_t get_irq_number(void)
{
    return inw(IRQ_BASIC_PENDING);
}