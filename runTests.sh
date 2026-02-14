#!/bin/bash

BIN_DIR="./bin"
BIN_TESTS_DIR="$BIN_DIR/tests"

INCLUDE_FLAGS="-Ilibc/include -Idrivers/$ARCH/include -Iarch/$ARCH/include -Iarch/common/include"
CFLAGS="-std=c99 -ffreestanding -Wall -Wextra $INCLUDE_FLAGS" 

set -e
mkdir -p "$BIN_TESTS_DIR/libc"

OBJS=""

echo "Compiling sources..."
files=("libc/vsnprintf.c" "libc/snprintf.c")
for file in "${files[@]}"; do
    OBJ_OUT="$BIN_TESTS_DIR/$(dirname "$file")/$(basename "$file" .c).o"
    gcc $CFLAGS -c "$file" -o "$OBJ_OUT" -Dvsnprintf=custom_vsnprintf -Dsnprintf=custom_snprintf
    
    OBJS="$OBJS $OBJ_OUT"
done

echo "Compiling the tests..."
gcc -std=c99 -c ./tests/libc/vsnprintf_tests.c -o "$BIN_TESTS_DIR/vsnprintf_tests.o" \
    -I./tests/libc/external/ \
    -fno-builtin-sprintf \
    -Wall -Wextra

echo "Linking..."
gcc $OBJS "$BIN_TESTS_DIR/vsnprintf_tests.o" -o "$BIN_TESTS_DIR/vsnprintf_tests"

echo "Running printf tests..."
./"$BIN_TESTS_DIR/vsnprintf_tests"
