.section .text
.global writeu
.global sleepu
.global exitu
.global waitu
.global close_file
.global open_file
.global get_file_size
.global read_file
.global fork
.global getpid
.global exec
.global keyboard_read
.global read_root_directory

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

exitu:
    mov x8, #2          //SVC for exit 
    mov x0, #0          //no arguments

    svc #0              //svc instruction trap in lower_el1_aarch64_sync

    ret

waitu:
    sub sp, sp, #8      
    mov x8, #3          //SVC for wait

    str x0, [sp]        //store x0=pid
    mov x0, #1          //num of args on stack = 1
    mov x1, sp          //stack pointer addr in x1

    svc #0

    add sp, sp, #8
    ret

open_file:
    sub sp, sp, #8
    mov x8, #4

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #0

    add sp, sp, #8
    ret

close_file:
    sub sp, sp, #8
    mov x8, #5

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #0

    add sp, sp, #8
    ret

get_file_size:
    sub sp, sp, #8
    mov x8, #6

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #0

    add sp, sp, #8
    ret
    
read_file: //3 arguments
    sub sp, sp, #24
    mov x8, #7

    stp x0, x1, [sp]
    str x2, [sp, #16]

    mov x0, #3
    mov x1, sp

    svc #0

    add sp, sp, #24
    ret

fork:
    mov x8, #8
    mov x0, #0

    svc #0

    ret

getpid:
    mov x8, #9
    mov x0, #0

    svc #0

    ret

exec:
    sub sp, sp, #8
    mov x8, #10

    str x0, [sp]

    mov x0, #1
    mov x1, sp

    svc #0

    add sp, sp, #8
    ret

keyboard_read:
    mov x8, #11
    mov x0, #0

    svc #0

    ret

read_root_directory:
    sub sp, sp, #8
    mov x8, #12

    str x0, [sp]
    mov x0, #1
    mov x1, sp

    svc #0

    add sp, sp, #8
    ret