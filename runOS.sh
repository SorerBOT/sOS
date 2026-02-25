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
STAGE_1="$BOOTLOADER_DIR/stage_1/bootloader_stage_1"
STAGE_2="$BOOTLOADER_DIR/stage_2/bootloader_stage_2"
PAGE_TABLE_SETUP="$BOOTLOADER_DIR/bootloader_stage_2_page_table_setup"
UPDATE_GDT="$BOOTLOADER_DIR/bootloader_stage_2_update_gdt"
BOOTLOADER_SIZE_MAX=0x8400 # 32.5KiB + 512 bytes = 33 KiB

STAGE_2_ORG=0x7E00
KERNEL_ORG=0x10000 # 0x7E00 + 32.5KiB

KERNEL_DIR="kernel"
KERNEL_START="$KERNEL_DIR/kernel_start"
KERNEL="$KERNEL_DIR/kernel"

INCLUDE_FLAGS="-Ibootloader/ -Idrivers/$ARCH/include -Iarch/$ARCH/include -Iarch/common/include"

MAKEFILE_STAGE_1="Makefile_bootloader_stage_1"
MAKEFILE_STAGE_2="Makefile_bootloader_stage_2"
MAKEFILE_KERNEL="Makefile_kernel"

set -e # quits on fail

echo "Cleaning up previous builds..."
make -f $MAKEFILE_STAGE_1 clean
make -f $MAKEFILE_STAGE_2 clean
make -f $MAKEFILE_KERNEL clean

make -f $MAKEFILE_STAGE_1 "$BIN_16_BIT_DIR/$STAGE_1.bin"
make -f $MAKEFILE_STAGE_2 "$BIN_32_BIT_DIR/$STAGE_2.bin"
make -f $MAKEFILE_KERNEL "$BIN_64_BIT_DIR/$KERNEL.bin"

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
