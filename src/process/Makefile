.PHONY: all clean compile pre_install $(OBJ_FILES)

#current Directory
#$(shell ls $(PWD) -F | grep "\/") ; for reference
DIRECTORY = .
#Find all .c and .asm files in all the directories found
SRC_FILES = $(foreach d,$(DIRECTORY),$(shell find $(d) -name "*.c" -or -name "*.s"))
#create object file names by subsituting extensions with .o , using filter helps to substitute multiple files with different patterns
OBJ_FILES = $(filter %.o,$(patsubst %.s,%.s.o,$(SRC_FILES)) $(patsubst %.c,%.o,$(SRC_FILES)))
# #extract the file name from the absolute file paths
# FILES = $(foreach d,$(OBJ_FILES),$(shell basename $(d)))

all: compile pre_install clean

compile: $(OBJ_FILES)

# .s compilation
%.s.o: %.s
	$(CC) -c $^ -o $@
# .c compilation
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $^ -o $@

pre_install:
	for file in $(OBJ_FILES); do \
        cp -rfv $$file $(BUILD_DIRECTORY)/. ; \
    done

clean:
	rm -rf $(OBJ_FILES)