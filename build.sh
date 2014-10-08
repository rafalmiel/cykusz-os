#!/bin/bash

nasm -felf arch/x86/boot.S -o boot.o
nasm -felf arch/x86/gdt.S -o gdt.o
nasm -felf arch/x86/interrupt.S -o interrupt.o
flags="-std=gnu99 -ffreestanding -g -O0 -Wall -Wextra -I."
i686-elf-gcc -c kernel.c -o kernel.o $flags
i686-elf-gcc -c arch/x86/io.c -o io.o $flags
i686-elf-gcc -c arch/x86/descriptor_tables.c -o descriptor_tables.o $flags
i686-elf-gcc -c arch/x86/isr.c -o isr.o $flags
i686-elf-gcc -c arch/x86/timer.c -o timer.o $flags
i686-elf-gcc -c arch/x86/kheap.c -o kheap.o $flags
i686-elf-gcc -c arch/x86/paging.c -o paging.o $flags
i686-elf-gcc -c arch/x86/frame.c -o frame.o $flags
i686-elf-gcc -c core/common.c -o common.o $flags
i686-elf-gcc -c core/ordered_array.c -o ordered_array.o $flags
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -g -nostdlib boot.o kernel.o io.o common.o ordered_array.o gdt.o descriptor_tables.o interrupt.o isr.o timer.o kheap.o paging.o frame.o -lgcc
qemu-system-i386 -kernel myos.bin -initrd LICENSE

