#!/bin/bash

BIN_DIR="./bin"
BIN_TESTS_DIR="$BIN_DIR/tests"

INCLUDE_FLAGS="-Ilibc/include -Idrivers/$ARCH/include -Iarch/$ARCH/include -Iarch/common/include"
CFLAGS="-std=c99 -ffreestanding $INCLUDE_FLAGS"

set -e
mkdir -p "$BIN_TESTS_DIR/libc"
echo "Compiling vsnprintf..."

file="libc/vsnprintf.c"
gcc $CFLAGS -c "$file" -o "$BIN_TESTS_DIR/$(dirname "$file")/$(basename "$file" .c).o"

echo "Compiling the tests..."
gcc -std=c99 -c ./tests/libc/vsnprintf_tests.c -o "$BIN_TESTS_DIR/vsnprintf_tests.o" \
    -I./tests/libc/external/ \
    -fno-builtin-sprintf
    

echo "Linking..."
gcc "$BIN_TESTS_DIR/$(dirname "$file")/$(basename "$file" .c).o" "$BIN_TESTS_DIR/vsnprintf_tests.o" -o "$BIN_TESTS_DIR/vsnprintf_tests"

echo "Running printf tests..."
./"$BIN_TESTS_DIR/vsnprintf_tests"
