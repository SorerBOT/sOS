BIN="./bin/"
STAGE_1="bootloader_stage_1"
STAGE_2="bootloader_stage_2"

rm -rf $BIN
mkdir $BIN

nasm -f bin "$STAGE_1.asm" -o "$BIN$STAGE_1.bin"
nasm -f bin "$STAGE_2.asm" -o "$BIN$STAGE_2.bin"
