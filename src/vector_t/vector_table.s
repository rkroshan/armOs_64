.macro kernel_entry
    //store the context
    sub sp, sp, #(36 * 8)       //for user space processes it is actually top of stack == trapframe
    stp x0, x1, [sp]
    stp x2, x3, [sp, #(16 * 1)]
    stp x4, x5, [sp, #(16 * 2)]
    stp	x6, x7, [sp, #(16 * 3)]
	stp	x8, x9, [sp, #(16 * 4)]
	stp	x10, x11, [sp, #(16 * 5)]
	stp	x12, x13, [sp, #(16 * 6)]
	stp	x14, x15, [sp, #(16 * 7)]
	stp	x16, x17, [sp, #(16 * 8)]
	stp	x18, x19, [sp, #(16 * 9)]
	stp	x20, x21, [sp, #(16 * 10)]
	stp	x22, x23, [sp, #(16 * 11)]
	stp	x24, x25, [sp, #(16 * 12)]
	stp	x26, x27, [sp, #(16 * 13)]
	stp	x28, x29, [sp, #(16 * 14)]
    mrs x0, sp_el0
    stp x30, x0,  [sp, #(16 * 15)]
.endm

.macro kernel_exit
    //restore the context
    ldp x30, x0, [sp, #(16 * 15)]
    msr sp_el0, x0

    ldp x0, x1, [sp]
    ldp x2, x3, [sp, #(16 * 1)]
    ldp x4, x5, [sp, #(16 * 2)]
    ldp	x6, x7, [sp, #(16 * 3)]
	ldp	x8, x9, [sp, #(16 * 4)]
	ldp	x10, x11, [sp, #(16 * 5)]
	ldp	x12, x13, [sp, #(16 * 6)]
	ldp	x14, x15, [sp, #(16 * 7)]
	ldp	x16, x17, [sp, #(16 * 8)]
	ldp	x18, x19, [sp, #(16 * 9)]
	ldp	x20, x21, [sp, #(16 * 10)]
	ldp	x22, x23, [sp, #(16 * 11)]
	ldp	x24, x25, [sp, #(16 * 12)]
	ldp	x26, x27, [sp, #(16 * 13)]
	ldp	x28, x29, [sp, #(16 * 14)]

    add sp, sp, #(36 * 8)
    eret
.endm

.macro handler_entry
    mrs x1, esr_el1
    stp x0, x1, [sp, #(16 * 16)]        //x0 contains the trap number

    mrs x0, elr_el1
    mrs x1, spsr_el1
    stp x0, x1, [sp, #(16 * 17)]

    mov x0, sp                          //for user process this will points to top of stack == pointer to trapframe struct
    bl exception_handler
    b trap_return
.endm

.section .text
.global vector_table_el1
.global pstart

.balign 0x800
vector_table_el1:
current_el1_sp0_sync:
    b error

.balign 0x80
current_el1_sp0_irq:
    b error

.balign 0x80
current_el1_sp0_fiq:
    b error

.balign 0x80
current_el1_sp0_serror:
    b error

.balign 0x80
current_el1_spn_sync:
    b sync_handler

.balign 0x80
current_el1_spn_irq:
    b irq_handler

.balign 0x80
current_el1_spn_fiq:
    b error

.balign 0x80
current_el1_spn_serror:
    b error

.balign 0x80
lower_el1_aarch64_sync:
    b sync_handler

.balign 0x80
lower_el1_aarch64_irq:
    b irq_handler

.balign 0x80
lower_el1_aarch64_fiq:
    b error

.balign 0x80
lower_el1_aarch64_serror:
    b error

.balign 0x80
lower_el1_aarch32_sync:
    b error

.balign 0x80
lower_el1_aarch32_irq:
    b error

.balign 0x80
lower_el1_aarch32_fiq:
    b error

.balign 0x80
lower_el1_aarch32_serror:
    b error

pstart:
    mov sp, x0      /*move trapframe pointer to sp*/
    
trap_return:
    ldp x0, x1, [sp, #(16 * 17)]    /*restore elr and spsr */
    msr elr_el1, x0
    msr spsr_el1, x1
    kernel_exit                     /*restore the context from the trapframe, beautiful!*/

sync_handler:
    kernel_entry
    mrs x0, esr_el1
    lsr x1, x0, #26
    cmp x1, #0b010101               /*check the exception class to make sure it is SVC64 bit or not */
    mov x2, #1
    mov x3, #3
    csel x0, x2, x3, ne             /*if EC=SVC64 then xo=x3, else x0=x2 */
    handler_entry

irq_handler:
    kernel_entry
    mov x0, #2
    handler_entry

error:
    kernel_entry
    mov x0, #0
    handler_entry
    