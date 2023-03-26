#Directories to Build
SUBDIR = $(shell ls -F | grep "\/" )

#define phony
.PHONY: all clean build $(SUBDIR)

all: clean build 

$(SUBDIR):
	$(MAKE) -C $@ 

build: $(SUBDIR)
	echo "kernel building"
	$(LD) $(LDFLAGS) -T $(SRC_DIRECTORY)/$(LINKER_SCRIPT) -L $(BUILD_DIRECTORY) -o $(BIN_DIRECTORY)/$(KERNEL_ELF) $(BUILD_DIRECTORY)/*.o
	$(OBJCPY) -O binary $(BIN_DIRECTORY)/$(KERNEL_ELF) $(BIN_DIRECTORY)/$(KERNEL_BIN)

clean:
	rm -rf $(BUILD_DIRECTORY)/*.o
	rm -rf $(BIN_DIRECTORY)/$(KERNEL_ELF)
	rm -rf $(BIN_DIRECTORY)/$(KERNEL_BIN)

#disassemble the bootloader binary
disassemble_bin: