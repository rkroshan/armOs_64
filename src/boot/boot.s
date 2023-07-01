.section .text
.global start

start:
//currently we will work on cpu0 and put other cpus into sleep
//to do that we will read the cpu-number a.k.a cpu-affinity from last 2 bits of mpdir_el1 reg
//if it is cpu zero we will init kernel else put into infinite loop
    mrs x0, mpidr_el1           //read system register mpidr_el1 in r0
    and x0, x0, #3              //x0 = x0 & 0x11;
    cmp x0, #0                  //x0 == 0
    beq kernel_entry            //if yes, jmp to kernel_entry
end:
    b end                       //infinite loop 

kernel_entry:
    mov sp, #0x80000            //move the stack pointer to 0x80000, above which boot code will lie
init_bss_section:
    ldr x0, =bss_start          //get bss_start in x0
    ldr x1, =bss_end            //get bss_end in x1
    sub x2, x1, x0              //x2 hold size of bss section x2 = x1-x0
    mov x1, #0                  //x1 hold value to set x1 = 0
    bl memset                   //memset(x0,x1,x2)
jmp_to_kernel_main:
    bl kernel_main              //jmp to kernel main C function
    b end