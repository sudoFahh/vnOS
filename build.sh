#!/bin/bash

set -e

nasm -f elf32 boot.asm -o boot.o
gcc -m32 -ffreestanding -c kernel.c -o kernel.o
ld -m elf_i386 -T linker.ld boot.o kernel.o -o kernel.bin

mkdir -p iso/boot/grub

cp kernel.bin iso/boot/kernel.bin
cp grub.cfg iso/boot/grub/grub.cfg

grub2-mkrescue -o myos.iso iso