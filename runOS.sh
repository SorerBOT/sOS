#!/bin/bash

set -e # makes the script quit on fail

BIN="./bin/"
STAGE_1="bootloader/bootloader_stage_1"
STAGE_2="bootloader/bootloader_stage_2"
PAGE_TABLE_SETUP="bootloader/bootloader_stage_2_page_table_setup"
UPDATE_GDT="bootloader/bootloader_stage_2_update_gdt"
KERNEL="kernel/kernel"
KERNEL_START="kernel/kernel_start"
OS_IMG="os-img"
STAGE_2_ORG=0x7E00
KERNEL_ORG=0xBE00 # 0x7E00 + 16KiB
BOOTLOADER_SIZE_MAX=16896 # 16KiB + 512
OS_IMAGE_SIZE_MAX=33280 # 32KiB + 512


LIB="libc/"
MATH_LIB="math"
MEMSET_LIB="memset"

rm -rf $BIN
mkdir $BIN
mkdir "$BIN/bootloader"
mkdir "$BIN/kernel"

# Compiling Stage 1
echo "Compiling stage 1."
nasm -f bin "$STAGE_1.asm" -o "$BIN$STAGE_1.bin"

# Compiling Stage 2
echo "Compiling stage 2."
CFLAGS_32_BIT="-ffreestanding -m32 -g -I./include -I./drivers"
nasm -f elf32 "$STAGE_2.asm" -o "$BIN$STAGE_2.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$PAGE_TABLE_SETUP.c" -o "$BIN$PAGE_TABLE_SETUP.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$UPDATE_GDT.c" -o "$BIN$UPDATE_GDT.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$LIB$MATH_LIB.c" -o "$BIN$MATH_LIB.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$LIB$MEMSET_LIB.c" -o "$BIN$MEMSET_LIB.o"
# -Ttext 0x7E00: [ORG 0x7E00]
# --oformat binary: output a raw flat file, not an executable
x86_64-elf-ld -m elf_i386 --oformat binary -Ttext $STAGE_2_ORG -o "$BIN$STAGE_2.bin" "$BIN$STAGE_2.o" "$BIN$PAGE_TABLE_SETUP.o" "$BIN$UPDATE_GDT.o" "$BIN$MEMSET_LIB.o" "$BIN$MATH_LIB.o"

# Allocating the first 16KiB + 512 bytes to the bootloader
cat "$BIN$STAGE_1.bin" "$BIN$STAGE_2.bin" > "$BIN$OS_IMG.bin"

BOOTLOADER_SIZE=$(wc -c < "$BIN$OS_IMG.bin")
echo "Bootloader built successfully..."
if (( BOOTLOADER_SIZE > BOOTLOADER_SIZE_MAX )); then
    echo "Bootloader size: $OS_IMAGE_SIZE, exceeds allowed size of 16KiB + 512 bytes. It's time to make the BIOS load more code in stage 1...."
    exit 1
else
    echo "Bootloader size: $OS_IMAGE_SIZE is OK."
fi

truncate -s $BOOTLOADER_SIZE_MAX "$BIN$OS_IMG.bin"

# Compiling the kernel
echo "Compiling the kernel."
CFLAGS_64_BIT="-ffreestanding -mno-red-zone -m64 -g -I./include -I./drivers"
nasm -f elf64 "$KERNEL_START.asm" -o "$BIN$KERNEL_START.o"
x86_64-elf-gcc $CFLAGS_64_BIT -c "$KERNEL.c" -o "$BIN/$KERNEL.o"
x86_64-elf-gcc $CFLAGS_64_BIT -c "$LIB$MATH_LIB.c" -o "$BIN$MATH_LIB.o"
x86_64-elf-gcc $CFLAGS_64_BIT -c "$LIB$MEMSET_LIB.c" -o "$BIN$MEMSET_LIB.o"
x86_64-elf-ld -m elf_x86_64 --oformat binary -Ttext $KERNEL_ORG -o "$BIN/$KERNEL.bin" "$BIN/$KERNEL_START.o" "$BIN/$KERNEL.o" "$BIN$MEMSET_LIB.o" "$BIN$MATH_LIB.o"
cat "$BIN/$KERNEL.bin" >> "$BIN$OS_IMG.bin"

# checking the file size, should not exceed 512 + 32KiB because this is all we're loading into RAM
OS_IMAGE_SIZE=$(wc -c < "$BIN$OS_IMG.bin")
echo "OS built successfully..."
if (( OS_IMAGE_SIZE > OS_IMAGE_SIZE_MAX )); then
    echo "OS size: $OS_IMAGE_SIZE, exceeds allowed size of 32KiB + 512 bytes. It's time to make the BIOS load more code in stage 1...."
    exit 1
else
    echo "OS size: $OS_IMAGE_SIZE is OK. Running the OS"
fi

truncate -s $OS_IMAGE_SIZE_MAX "$BIN$OS_IMG.bin"

qemu-system-x86_64 -drive format=raw,file="$BIN$OS_IMG.bin" -display cocoa
