#!/bin/bash

nasm -felf ./arch/x86/boot.S -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/io.c -o io.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c core/common.c -o common.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o io.o common.o -lgcc
qemu-system-i386 -kernel myos.bin

