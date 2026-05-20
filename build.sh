#!/bin/bash

set -e

echo "[1/5] Creating directories..."
mkdir -p build
mkdir -p iso/boot/grub
mkdir -p .isos

echo "[2/5] Assembling bootloader..."
nasm -f elf32 boot.asm -o build/boot.o

echo "[3/5] Compiling kernel sources..."

SRC_FILES=$(find src -name "*.c")

for file in $SRC_FILES; do
    obj="build/$(basename "${file%.c}.o")"

    echo "Compiling $file -> $obj"

    gcc \
        -m32 \
        -ffreestanding \
        -fno-pie \
        -fno-stack-protector \
        -nostdlib \
        -Iinclude \
        -c "$file" \
        -o "$obj"
done

echo "[4/5] Linking kernel..."

ld -m elf_i386 \
   -T linker.ld \
   build/*.o \
   -o kernel.bin

echo "[5/5] Building ISO..."

cp kernel.bin iso/boot/kernel.bin
cp grub.cfg iso/boot/grub/grub.cfg

grub2-mkrescue -o .isos/myos.iso iso

echo ""
echo "Done."
echo "ISO: .isos/myos.iso"