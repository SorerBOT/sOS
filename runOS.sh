#!/bin/bash

OS_IMG="os-img"
OS_IMAGE_SIZE_MAX=66560 # 33KiB + 32KiB

ARCH="i386"
DRIVERS_DIR="drivers"
ARCH_DIR="arch"
LIBC_DIR="bootloader/libc_partials/"

BIN_DIR="./bin"
BIN_16_BIT_DIR="$BIN_DIR/16-bit"
BIN_32_BIT_DIR="$BIN_DIR/32-bit"
BIN_64_BIT_DIR="$BIN_DIR/64-bit"

BOOTLOADER_DIR="bootloader"
STAGE_1="$BOOTLOADER_DIR/bootloader_stage_1"
STAGE_2="$BOOTLOADER_DIR/bootloader_stage_2"
PAGE_TABLE_SETUP="$BOOTLOADER_DIR/bootloader_stage_2_page_table_setup"
UPDATE_GDT="$BOOTLOADER_DIR/bootloader_stage_2_update_gdt"
BOOTLOADER_SIZE_MAX=0x8400 # 32.5KiB + 512 bytes = 33 KiB

STAGE_2_ORG=0x7E00
KERNEL_ORG=0x10000 # 0x7E00 + 32.5KiB

KERNEL_DIR="kernel"
KERNEL_START="$KERNEL_DIR/kernel_start"
KERNEL="$KERNEL_DIR/kernel"


INCLUDE_FLAGS="-Ibootloader/ -Idrivers/$ARCH/include -Iarch/$ARCH/include -Iarch/common/include"

set -e # quits on fail

echo "Cleaning up previous builds..."
rm -rf $BIN_DIR

echo "Creating 16-bit BIN directories..."
mkdir -p "$BIN_16_BIT_DIR/$BOOTLOADER_DIR"

echo "Creating 32-bit BIN directories..."
mkdir -p "$BIN_32_BIT_DIR/$BOOTLOADER_DIR"
mkdir "$BIN_32_BIT_DIR/$LIBC_DIR"
mkdir -p "$BIN_32_BIT_DIR/$DRIVERS_DIR/$ARCH"
mkdir -p "$BIN_32_BIT_DIR/$ARCH_DIR/$ARCH"

# echo "Creating 64-bit BIN directories..."
# mkdir -p "$BIN_64_BIT_DIR/$KERNEL_DIR"
# mkdir "$BIN_64_BIT_DIR/$LIBC_DIR"
# mkdir -p "$BIN_64_BIT_DIR/$DRIVERS_DIR/$ARCH"
# mkdir -p "$BIN_64_BIT_DIR/$ARCH_DIR/$ARCH"

echo "Compiling stage 1..."
nasm -f bin "$STAGE_1.asm" -o "$BIN_16_BIT_DIR/$STAGE_1.bin"

echo "Compiling stage 2..."
CFLAGS_32_BIT="-std=c99 -ffreestanding -m32 -g $INCLUDE_FLAGS"
nasm -f elf32 "$STAGE_2.asm" -o "$BIN_32_BIT_DIR/$STAGE_2.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$PAGE_TABLE_SETUP.c" -o "$BIN_32_BIT_DIR/$PAGE_TABLE_SETUP.o"
x86_64-elf-gcc $CFLAGS_32_BIT -c "$UPDATE_GDT.c" -o "$BIN_32_BIT_DIR/$UPDATE_GDT.o"

echo "Compiling 32-bit drivers..."
for file in "$DRIVERS_DIR/$ARCH"/*.c; do
    [ -e "$file" ] || continue
    x86_64-elf-gcc $CFLAGS_32_BIT -c "$file" -o "$BIN_32_BIT_DIR/$(dirname "$file")/$(basename "$file" .c).o"
done
echo "Compiling 32-bit architecture specific code..."
for file in "$ARCH_DIR/$ARCH"/*.c; do
    [ -e "$file" ] || continue
    x86_64-elf-gcc $CFLAGS_32_BIT -c "$file" -o "$BIN_32_BIT_DIR/$(dirname "$file")/$(basename "$file" .c).o"
done
echo "Compiling 32-bit libc..."
for file in "$LIBC_DIR"/*.c; do
    [ -e "$file" ] || continue
    x86_64-elf-gcc $CFLAGS_32_BIT -c "$file" -o "$BIN_32_BIT_DIR/$(dirname "$file")/$(basename "$file" .c).o"
done

echo "Linking Stage 2..."
OBJ_FILES_EXCEPT_ENTRY=$(find $BIN_32_BIT_DIR -type f -name "*.o" ! -name "$(basename "$BIN_32_BIT_DIR/$STAGE_2.o")")
x86_64-elf-ld -m elf_i386 --oformat binary -Ttext $STAGE_2_ORG \
    -o "$BIN_32_BIT_DIR/$STAGE_2.bin" \
    "$BIN_32_BIT_DIR/$STAGE_2.o" $OBJ_FILES_EXCEPT_ENTRY
echo "Successfully linked stage 2..."


cat "$BIN_16_BIT_DIR/$STAGE_1.bin" "$BIN_32_BIT_DIR/$STAGE_2.bin" > "$BIN_DIR/$OS_IMG.bin"

BOOTLOADER_SIZE=$(wc -c < "$BIN_DIR/$OS_IMG.bin")
echo "Bootloader built successfully..."
if (( BOOTLOADER_SIZE > BOOTLOADER_SIZE_MAX )); then
    echo "Bootloader size: $BOOTLOADER_SIZE, exceeds allowed size of 33KiB bytes. It's time to make the BIOS load more code in stage 1..."
    exit 1
else
    echo "Bootloader size: $BOOTLOADER_SIZE is OK."
fi

echo "Padding bootloader binary (entire OS image)..."
truncate -s $BOOTLOADER_SIZE_MAX "$BIN_DIR/$OS_IMG.bin"

# Compiling and linking the kernel via a makefile
make -f Makefile-kernel "$BIN_64_BIT_DIR/$KERNEL.bin"


echo "Appending kernel to OS image..."
cat "$BIN_64_BIT_DIR/$KERNEL.bin" >> "$BIN_DIR/$OS_IMG.bin"

OS_IMAGE_SIZE=$(wc -c < "$BIN_DIR/$OS_IMG.bin")
echo "OS built successfully..."
if (( OS_IMAGE_SIZE > OS_IMAGE_SIZE_MAX )); then
    echo "OS size: $OS_IMAGE_SIZE, exceeds allowed size of 65KiB. It's time to make the BIOS load more code in stage 1...."
    exit 1
else
    echo "OS size: $OS_IMAGE_SIZE is OK. Running the OS"
fi

echo "Padding kernel binary (entire OS image)..."
truncate -s $OS_IMAGE_SIZE_MAX "$BIN_DIR/$OS_IMG.bin"

echo "Build complete. Running the OS!"
qemu-system-x86_64 -drive format=raw,file="$BIN_DIR/$OS_IMG.bin" -display cocoa \
    -d int,cpu_reset \
    -D qemu.log \
    -no-reboot \
    -no-shutdown
