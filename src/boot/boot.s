.section .text_boot
.global start

.equ FS_BASE, 0xffff000030000000    //virtual addr for FS starting addr
.equ FS_SIZE, 512*18432             //9MB

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
check_if_el2:
    mrs x0, currentel           //currentel[3:2] bit define at which el we are
    lsr x0, x0, #2
    cmp x0, #2                  //if el is El2, which is the case with Raspi3b
    bne end                     //should not be possible
switch_to_el1:
    msr sctlr_el1, xzr          //zero-out sctrl_el1 register
    mov x0, #(1 << 31)
    msr hcr_el2, x0             //makes execution state of el1 as Aarch64
    
    mov x0, #0b1111000101       //spsr_el2 Aarch64 bit configuration: mode set to el1, diabled all interrupts, this will be copied to PSTATE on ret
    msr spsr_el2, x0

    adr x0, el1_entry           //get addr of el1_entry label first instruction
    msr elr_el2, x0             //set the return addr to el1_entry, this is copied to PC register on ret
    eret

el1_entry:
    mov sp, #0x80000            //move the stack pointer to 0x80000, above which boot code will lie
setup_mmu:
    bl setup_vm
    bl enable_mmu
setup_fs:
    ldr x0, =FS_BASE            //load the FS_Base VA
    ldr x1, =bss_start          //bss_start will be the starting point where the FS is loaded in os.bin, since actual data is upto .data section after which everything should be 0, where the FS should be in os.bin
    ldr x2, =FS_SIZE            
    bl memcpy                   //memcpy the FS staring from FS_BASE upto FS_SIZE
init_bss_section:
    ldr x0, =bss_start          //get bss_start in x0
    ldr x1, =bss_end            //get bss_end in x1
    sub x2, x1, x0              //x2 hold size of bss section x2 = x1-x0
    mov x1, #0                  //x1 hold value to set x1 = 0
    bl memset                   //memset(x0,x1,x2)

setup_el1_vbar:
    ldr x0, =vector_table_el1
    msr vbar_el1, x0            //setup el1 aarch64 vector table

update_sp_mmu_enable:
    mov x0, #0xffff000000000000
    add sp, sp, x0              //kernel is at 0xffff00000080000, so below it can be sp
jmp_to_kernel_main:
    ldr x0, =kernel_main        //need to get virtual address of kernel_main
    blr x0                      //jmp to kernel main C function

halt:
    wfi
    b end