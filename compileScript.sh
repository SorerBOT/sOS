OUTPUT="./bin/bootloader.bin"

nasm -f bin bootloader.asm -o $OUTPUT
hexdump -C $OUTPUT

