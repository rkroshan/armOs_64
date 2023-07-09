#include "irq.h"
#include "utils/utils.h"

void init_interrupt_controller(void)
{
#ifdef RASPI3B
    /*disable all IRQs*/
    outw(DISABLE_BASIC_IRQS, 0xffffffff);
    outw(DISABLE_IRQS_1, 0xffffffff);
    outw(DISABLE_IRQS_2, 0xffffffff);
    /*Enable uart interrupt, refer page 113 doc bcm2835 arm peripherals*/
    outw(ENABLE_IRQS_2, (1 << 25));
#endif

#ifdef RASPI4
    outw(DIST, 0);              /*Disable the Distributer*/
    outw(CPU_INTERFACE, 0);     /*Disable the CPU Interface*/

    outw(ICC_PR, 0xff);         /*Set the interrupt Priority as 255 as lowest priority to accept*/

    outw(ICD_PR + 64, 0);       /*Set Max priority = 0, Distributer priority register, 4 interrupts in one reg of 32 bits, for arm timer interrupt it will 64 int no so it will be in 16 reg so addr will be 16*4(size of each reg)*/
    outw(ICD_PR + 38*4, 0);       /*bcm28711 doc, similarly for UART interrupt it is connected to video core at 57 and VC start SPIID = 96 , so 96+57 = 153, sp 38 reg 2 int  = 38*4 and value will be 0 to set 3rd nibble*/
    outw(ICD_PTR + 64, 1);      /*Processor Target Reg, each reg defined processor affinity for the interrupt one reg contains information about 4 int, there for int 64 it will 16 register and since we need to trigger at processor 0, we write 1*/
    outw(ICD_PTR + 38*4, 0x100); /*Processor Target Reg, similar to ICD_PR 38 reg 3rd nibble so val 0x100 to target core 0*/
    outw(ICD_ICFGR + 16, 2);    /*interrupt configuration register, to define whether edge trigger or level, each reg conatin 16 int information in 2 bytes (high bit 0 = edge 1 = level), so for int 64 it will be reg 4 there addr = 4*4=16, and set as level triggered*/
    outw(ICD_ICFGR + 9*4, 0x80000);    /*similary, for 153 int = 9 reg and 20th bit for level trigger*/
    outw(ICD_ISENABLE + 8, 1);  /*interrupt enable register distributor, each reg contain 32 int information, so to enable int 64 it will be reg 2, addr 2*4=8*/
    outw(ICD_ISENABLE + 8, 1 << 25);  /*similary, for int 153, 4reg and 25th reg*/

    outw(DIST, 1);              /*Enable distributor*/
    outw(CPU_INTERFACE, 1);     /*enable cpu interface*/
#endif
}

uint32_t get_irq_number(void)
{
#ifdef RASPI3B
    return inw(IRQ_BASIC_PENDING);
#endif

#ifdef RASPI4
    return inw(ICC_ACK);
#endif
}