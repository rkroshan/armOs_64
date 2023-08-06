.equ MAIR_ATTR, (0x44 << 8)
.equ TCR_T0SZ,  (16) 
.equ TCR_T1SZ,  (16 << 16)
.equ TCR_TG0,   (0 << 14)
.equ TCR_TG1,   (2 << 30)
.equ TCR_VALUE, (TCR_T0SZ | TCR_T1SZ | TCR_TG0 | TCR_TG1)
.equ PAGE_SIZE, (2*1024*1024)

.global enable_mmu
.global setup_vm

//enable mmu function
enable_mmu:
    adr x0, pgd_ttbr1   
    msr ttbr1_el1, x0   //put kernel pgd(page global directory) into ttbr1_el1, since kernel is decided to be mapped in higher mem region (0xFFFF s000000000000s)

    adr x0, pgd_ttbr0
    msr ttbr0_el1, x0   //put user pgd into ttbr0_el1 since in lower virtual mem region (0x0000 000000000000)

    ldr x0, =MAIR_ATTR
    msr mair_el1, x0    //setting up memory attribute indirection register, with index 0 = Device Memory and index 1 = Normal Memory

    ldr x0, =TCR_VALUE
    msr tcr_el1, x0     //setting translation control register with TTBR1/0 granule size 4KB , T0SZ/T1SZ = 16 so that translation bits will be 48 bits (64-16) and upper 16 bits will define where the translation will

    mrs x0, sctlr_el1
    orr x0, x0, #1
    msr sctlr_el1, x0   //enable the mmu for stage 1 translation
    
    ret


setup_vm:
setup_kvm:              //setup userspace memory map mapping upto 0x41000000
    adr x0, pgd_ttbr1
    adr x1, pud_ttbr1
    orr x1, x1, #3      //bit 0 = valid entry or not. bit 1 = whether it points to another translation table to not
    str x1, [x0]        //pgd[0] = &pud[0]

    adr x0, pud_ttbr1
    adr x1, pmd_ttbr1
    orr x1, x1, #3
    str x1, [x0]        //pud[0]=&pmd[0]

    mov x2, #0x34000000
    adr x1, pmd_ttbr1
    mov x0, #(1 << 10 | 1 << 2 | 1 << 0)    //staring to map from 0 to 0x34000000

loop1:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop1

    mov x2, #0x40000000
    mov x0, #0x3f000000

    adr x3, pmd_ttbr1                       //mapping peripheral memory from 0x3f000000 to 4GB
    lsr x1, x0, #(21 - 3)                   //trying to find pmd index; logic: right shit 21 bits for 2MB pages and left shift 3 bytes to get actual index
    add x1, x1, x3

    orr x0, x0, #1
    orr x0, x0, #(1 << 10)                  //mair idex = 0 ; device memory and valid bit set

loop2:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop2
above_4GB_mapping:                          //necessary for above 4GB mapping since some peripherals have system regs above 4 GB
    adr x0, pud_ttbr1                       
    add x0, x0, #(1 * 8)                    //2nd PUD entry to map next GB
    adr x1, pmd2_ttbr1
    orr x1, x1, #3
    str x1, [x0]                            //pud[1]=&pmd[1]

    mov x2, #0x41000000
    mov x0, #0x40000000

    adr x1, pmd2_ttbr1                      //will map upto 0x41000000
    orr x0, x0, #1
    orr x0, x0, #(1 << 10)

loop3:
    str x0, [x1], #8
    add x0, x0, #PAGE_SIZE
    cmp x0, x2
    blo loop3


setup_uvm:                  //setup userspace memory map
    adr x0, pgd_ttbr0
    adr x1, pud_ttbr0
    orr x1, x1, #3          //bit 0 = valid entry or not. bit 1 = whether it points to another translation table to not
    str x1, [x0]            //pgd[0] = &pud[0]

    adr x0, pud_ttbr0
    adr x1, pmd_ttbr0
    orr x1, x1, #3          //bit 0 = valid entry or not. bit 1 = whether it points to another translation table to not
    str x1, [x0]            //pud[0] = &pmd[0]

    adr x1, pmd_ttbr0
    mov x0, #(1 << 10 | 1 << 2 | 1 << 0)    //physical addr=0, 10bit = access flag, 2:4= mair index for normal memory, 0=valid bit 
    str x0, [x1]                            //pmd[0] = 0x0 with other bits set

    ret


.balign 4096
pgd_ttbr1:          //pgd for 2GB memory mapping, so only 1 pgd, 2 PUD and 2*4096 pmd entries to be mapped
    .space 4096     //1 pgd size = 4KB = 512 entries for 8 bytes, similary for pud and pmd
pud_ttbr1:
    .space 4096
pmd_ttbr1:
    .space 4096
pmd2_ttbr1:
    .space 4096

pgd_ttbr0:          //pgd for 2mb memory mapping so 1pgd, 1 PUD, 1 pmd entries to mapped
    .space 4096
pud_ttbr0:
    .space 4096
pmd_ttbr0:
    .space 4096



	