#Absoulte Paths
export SRC_DIRECTORY=${PWD}/src
export BIN_DIRECTORY=${PWD}/bin
export BUILD_DIRECTORY=${PWD}/build

export CC = aarch64-linux-gnu-gcc
export LD = aarch64-linux-gnu-ld
export OBJCPY =  aarch64-linux-gnu-objcopy
export INCLUDES = -I$(SRC_DIRECTORY)
export CFLAGS = -g -std=c99 -ffreestanding -mgeneral-regs-only
export LDFLAGS = -nostdlib

export KERNEL_ELF = kernel.elf
export KERNEL_BIN = kernel.bin
export LINKER_SCRIPT = linker.lds
export FS16_IMG = fs16.img
export OS_BIN = os.bin

.PHONY: all clean build run 

all:clean build make_os

build:
	$(MAKE) -C $(SRC_DIRECTORY)

clean:
	$(MAKE) -C $(SRC_DIRECTORY) clean

make_os:
	dd if=$(BIN_DIRECTORY)/$(KERNEL_BIN) > $(BIN_DIRECTORY)/$(OS_BIN)
#50MB fat16 file system, we will place it on top of kernel.img and then map it to virtual memory, 
#since currently disk read write functionality is not available we will read write from ram by booting the complete File system on to ram during bootup
	dd if=/dev/zero bs=512 count=102400 > $(BIN_DIRECTORY)/$(FS16_IMG)
#format it as fat16 and place it in os binary
	mkfs.fat $(BIN_DIRECTORY)/$(FS16_IMG) -F 16
	dd if=$(BIN_DIRECTORY)/$(FS16_IMG) >> $(BIN_DIRECTORY)/$(OS_BIN)
	

#run the emulator
run: run_qemu

run_qemu:
	qemu-system-aarch64 -M raspi3b -serial stdio -kernel $(BIN_DIRECTORY)/$(OS_BIN)