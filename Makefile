# Makefile for compiling the kernel
ARCH=x86_64
DRIVERS_DIR=drivers
ARCH_DIR=arch
LIBC_DIR=libc
BIN_DIR=bin

INCLUDE_FLAGS = -Ilibc/include -Idrivers/$(ARCH)/include -Iarch/$(ARCH)/include -Iarch/common/include
CFLAGS = -std=c99 -ffreestanding -mno-red-zone -m64 -g $(INCLUDE_FLAGS)

BIN_64_BIT_DIR=$(BIN_DIR)/64-bit
BIN_64_BIT_ASM_DIR=$(BIN_64_BIT_DIR)/asm/

KERNEL_ORG = 0x10000 # 0x7E00 + 32.5KiB

KERNEL_DIR = kernel
KERNEL_START = $(KERNEL_DIR)/kernel_start
KERNEL = $(KERNEL_DIR)/kernel

KERNEL_SRCS := $(shell find $(KERNEL_DIR) -type f \( -name '*.c' -o -name '*.asm' \))
ARCH_SRCS := $(shell find $(ARCH_DIR)/$(ARCH) -type f \( -name '*.c' -o -name '*.asm' \))
DRIVERS_SRCS := $(shell find $(DRIVERS_DIR)/$(ARCH) -type f \( -name '*.c' -o -name '*.asm' \))
LIBC_SRCS := $(shell find $(LIBC_DIR) -type f \( -name '*.c' -o -name '*.asm' \))

ALL_SRCS = $(KERNEL_SRCS) $(KERNEL_START_SRC) $(ARCH_SRCS) $(DRIVERS_SRCS) $(LIBC_SRCS)

C_OBJS := $(patsubst %.c, $(BIN_64_BIT_DIR)/%.o, $(filter %.c, $(ALL_SRCS)))
ASM_OBJS := $(patsubst %.asm, $(BIN_64_BIT_ASM_DIR)/%.o, $(filter %.asm, $(ALL_SRCS)))
ALL_OBJS := $(ASM_OBJS) $(C_OBJS)
START_OBJ := $(BIN_64_BIT_ASM_DIR)/$(KERNEL_START).o
ALL_OBJS_EXCEPT_START := $(filter-out $(START_OBJ), $(ALL_OBJS))

$(BIN_64_BIT_DIR)/$(KERNEL).bin: $(ALL_OBJS)
	@echo "Linking the kernel..."
	x86_64-elf-ld -m elf_x86_64 --oformat binary -Ttext $(KERNEL_ORG) \
		-o $(BIN_64_BIT_DIR)/$(KERNEL).bin \
		$(START_OBJ) $(ALL_OBJS_EXCEPT_START)

$(BIN_64_BIT_DIR)/%.o: %.c
	@echo "Compiling C file $<"
	@mkdir -p $(dir $@)
	x86_64-elf-gcc $(CFLAGS) -c $< -o $@

$(BIN_64_BIT_ASM_DIR)/%.o: %.asm
	@echo "Compiling ASM file $<"
	@mkdir -p $(dir $@)
	nasm -f elf64 $< -o $@

clean:
	rm -rf $(BIN_64_BIT_DIR)
