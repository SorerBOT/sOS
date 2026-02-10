#!/bin/bash

set -e # makes the script quit on fail

BIN="./bin/"
STAGE_1="bootloader_stage_1"
STAGE_2="bootloader_stage_2"
PAGE_TABLE_SETUP="bootloader_stage_2_page_table_setup"
PADDING="padding"
OS_IMG="os-img"
STAGE_2_ORG="0x7E00"
OS_IMAGE_SIZE_MAX=33280 # 32KiB + 512

LIB="lib/"
MATH_LIB="math"

rm -rf $BIN
mkdir $BIN
mkdir $BIN$

nasm -f bin "$STAGE_1.asm" -o "$BIN$STAGE_1.bin"
nasm -f elf32 "$STAGE_2.asm" -o "$BIN$STAGE_2.o"
x86_64-elf-gcc -ffreestanding -m32 -g -c "$PAGE_TABLE_SETUP.c" -o "$BIN$PAGE_TABLE_SETUP.o"
x86_64-elf-gcc -ffreestanding -m32 -g -c "$PAGE_TABLE_SETUP.c" -o "$BIN$PAGE_TABLE_SETUP.o"
x86_64-elf-gcc -ffreestanding -m32 -g -c "$LIB$MATH_LIB.c" -o "$BIN$MATH_LIB.o"

# -Ttext 0x7E00: [ORG 0x7E00]
# --oformat binary: output a raw flat file, not an executable
x86_64-elf-ld -m elf_i386 --oformat binary -Ttext $STAGE_2_ORG -o "$BIN$PAGE_TABLE_SETUP.bin" "$BIN$STAGE_2.o" "$BIN$PAGE_TABLE_SETUP.o" "$BIN$MATH_LIB.o"
dd if=/dev/zero of="$BIN$PADDING.bin" bs=1024 count=32

# now stage 2 and 3 are combined
cat "$BIN$STAGE_1.bin" "$BIN$PAGE_TABLE_SETUP.bin" > "$BIN$OS_IMG.bin"

# checking the file size, should not exceed 512 + 32KiB because this is all we're loading into RAM
OS_IMAGE_SIZE=$(wc -c < "$BIN$OS_IMG.bin")

echo "OS built successfully..."
if (( OS_IMAGE_SIZE > OS_IMAGE_SIZE_MAX )); then
    echo "OS size: $OS_IMAGE_SIZE, exceeds allowed size of 32KiB + 512 bytes. It's time to make the BIOS load more code in stage 1...."
    exit 1
else
    echo "OS size is OK. Running the OS"
fi

cat "$BIN$PADDING.bin" >> "$BIN$OS_IMG.bin"

qemu-system-x86_64 -drive format=raw,file="$BIN$OS_IMG.bin" -display cocoa
