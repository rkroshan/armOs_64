.section .text
.global vector_table_el1

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
    b error

.balign 0x80
current_el1_spn_fiq:
    b error

.balign 0x80
current_el1_spn_serror:
    b error

.balign 0x80
lower_el1_aarch64_sync:
    b error

.balign 0x80
lower_el1_aarch64_irq:
    b error

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


sync_handler:
    mov x0, #1          //synchronous error numid
    mrs x1, esr_el1     //exception syndrome reg val
    mrs x2, elr_el1     //exception addr
    bl exception_handler
    eret

error:
    mov x0, #0
    bl exception_handler
    eret
    