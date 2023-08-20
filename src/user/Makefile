.PHONY: all clean compile pre_install $(OBJ_FILES)

#current Directory
#$(shell ls $(PWD) -F | grep "\/") ; for reference
DIRECTORY = .
#Find all .c and .asm files in all the directories found
SRC_FILES = $(foreach d,$(DIRECTORY),$(shell find $(d) -maxdepth 1 -name "*.c" -or -name "*.s"))
#create object file names by subsituting extensions with .o , using filter helps to substitute multiple files with different patterns
OBJ_FILES = $(filter %.o,$(patsubst %.s,%.s.o,$(SRC_FILES)) $(patsubst %.c,%.o,$(SRC_FILES)))
# #extract the file name from the absolute file paths
# FILES = $(foreach d,$(OBJ_FILES),$(shell basename $(d)))
export LIB_FILE = lib.a


all: build_lib build_test build_console build_ls compile pre_install build clean

build_lib:
	$(MAKE) -C $(DIRECTORY)/lib

build_test:
	$(MAKE) -C $(DIRECTORY)/test

build_console:
	$(MAKE) -C $(DIRECTORY)/console

build_ls:
	$(MAKE) -C $(DIRECTORY)/ls

compile: $(OBJ_FILES)

build:
	$(LD) $(LDFLAGS) -T $(DIRECTORY)/ulinker.ld -L $(BUILD_USER_DIRECTORY) -o $(BIN_DIRECTORY)/$(INIT_ELF) $(OBJ_FILES) $(BUILD_USER_DIRECTORY)/$(LIB_FILE)
	$(OBJCPY) -O binary $(BIN_DIRECTORY)/$(INIT_ELF) $(BIN_DIRECTORY)/$(INIT_BIN)

# .s compilation
%.s.o: %.s
	$(CC) -c $^ -o $@
# .c compilation
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $^ -o $@

pre_install:
	for file in $(OBJ_FILES); do \
        cp -rfv $$file $(BUILD_USER_DIRECTORY)/. ; \
    done

clean: clean_lib clean_test clean_console clean_ls
	rm -rf $(OBJ_FILES)

clean_lib:
	$(MAKE) -C $(DIRECTORY)/lib clean

clean_test:
	$(MAKE) -C $(DIRECTORY)/test clean

clean_console:
	$(MAKE) -C $(DIRECTORY)/console clean

clean_ls:
	$(MAKE) -C $(DIRECTORY)/ls clean