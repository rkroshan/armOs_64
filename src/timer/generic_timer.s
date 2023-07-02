.section .text

.global read_timer_freq
.global set_timer_interval
.global enable_timer
.global read_timer_status
.global enable_irq
.global disable_irq

read_timer_freq:
    mrs x0, CNTFRQ_EL0      //Current Frq Reg
    ret

set_timer_interval:
    msr CNTP_TVAL_El0, x0   //set EL1 Physical Timer Val
    ret

enable_timer:
    stp x29, x30, [sp, #-16]!   //save LR and FP regs on stack

    bl read_timer_freq          //read timer freq
    mov x1, #100                
    udiv x0, x0, x1             //div it by 100 to get 10ms time interval
    bl set_timer_interval       //set the Timer Val after which timer interrupt will trigger on enabling Timer

    mov x0, #1
    msr CNTP_CTL_EL0, x0        //enable the timer

    ldp x29, x30, [sp], #16     //restore LR and FP from stack
    ret

read_timer_status:
    mrs x0, CNTP_CTL_EL0        //read El1 Physical Timer Control Reg val
    ret

enable_irq:
    msr daifclr, #2             //enable IRQ byt clearing bit 2 for the current EL
    ret

disable_irq:
    msr daifset, #2             //disable IRQ byt clearing bit 2 for the current EL
    ret