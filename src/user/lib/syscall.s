.section .text
.global writeu

writeu:
    sub sp, sp, #16     //push the SP 2 regs down
    mov x8, #0          //SVC num for printf

    stp x0, x1, [sp]    //store x0= buffer addr and x1 = buffer size
    mov x0, #2          //num of args on stack = 2
    mov x1, sp          //stack pointer addr in x1
    svc #0              //svc instruction trap in lower_el1_aarch64_sync

    add sp, sp, #16     //restore sp
    ret
    