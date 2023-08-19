.section .text
.global writeu
.global sleepu

writeu:
    sub sp, sp, #16     //push the SP 2 regs down
    mov x8, #0          //SVC num for printf

    stp x0, x1, [sp]    //store x0= buffer addr and x1 = buffer size
    mov x0, #2          //num of args on stack = 2
    mov x1, sp          //stack pointer addr in x1
    svc #0              //svc instruction trap in lower_el1_aarch64_sync

    add sp, sp, #16     //restore sp
    ret
    
sleepu:
    sub sp, sp, #8     //push the SP 1 regs down
    mov x8, #1          //SVC num for sleep

    str x0, [sp]        //store x0= sleep wait
    mov x0, #2          //num of args on stack = 1
    mov x1, sp          //stack pointer addr in x1
    svc #0              //svc instruction trap in lower_el1_aarch64_sync

    add sp, sp, #8     //restore sp
    ret
    