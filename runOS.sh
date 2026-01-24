#!/bin/bash

BIN="./bin/"
STAGE_1="bootloader_stage_1"
STAGE_2="bootloader_stage_2"
STAGE_3="bootloader_stage_3"
PADDING="padding"
OS_IMG="os-img"
STAGE_2_ORG="0x7E00"

rm -rf $BIN
mkdir $BIN

nasm -f bin "$STAGE_1.asm" -o "$BIN$STAGE_1.bin"
nasm -f elf32 "$STAGE_2.asm" -o "$BIN$STAGE_2.o"
x86_64-elf-gcc -ffreestanding -m32 -g -c "$STAGE_3.c" -o "$BIN$STAGE_3.o"

# -Ttext 0x7E00: [ORG 0x7E00]
# --oformat binary: output a raw flat file, not an executable
x86_64-elf-ld -m elf_i386 -o "$BIN$STAGE_3.bin" -Ttext $STAGE_2_ORG "$BIN$STAGE_2.o" "$BIN$STAGE_3.o" --oformat binary
dd if=/dev/zero of="$BIN$PADDING.bin" bs=1024 count=32

# now stage 2 and 3 are combined
cat "$BIN$STAGE_1.bin" "$BIN$STAGE_3.bin" "$BIN$PADDING.bin" > "$BIN$OS_IMG.bin"

qemu-system-x86_64 -drive format=raw,file="$BIN$OS_IMG.bin" -display cocoa
