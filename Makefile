#Absoulte Paths
export SRC_DIRECTORY=${PWD}/src
export BIN_DIRECTORY=${PWD}/bin
export BUILD_DIRECTORY=${PWD}/build

export CC = aarch64-linux-gnu-gcc
export LD = aarch64-linux-gnu-ld
export OBJCPY =  aarch64-linux-gnu-objcopy
export INCLUDES = -I$(SRC_DIRECTORY)
export CFLAGS = -g -std=c99 -ffreestanding -mgeneral-regs-only -DRASPI3B=1
export LDFLAGS = -nostdlib

export KERNEL_ELF = kernel.elf
export KERNEL_BIN = kernel.bin
export LINKER_SCRIPT = linker.lds

.PHONY: all clean build run 

all:clean build

build:
	$(MAKE) -C $(SRC_DIRECTORY)

clean:
	$(MAKE) -C $(SRC_DIRECTORY) clean

#run the emulator
run: run_qemu

run_qemu:
	qemu-system-aarch64 -M raspi3b -serial stdio -kernel $(BIN_DIRECTORY)/$(KERNEL_BIN)