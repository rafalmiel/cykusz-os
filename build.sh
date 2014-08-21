#!/bin/bash

#nasm -felf ./arch/x86/boot.S -o boot.o
i686-elf-as arch/x86/boot.S -o boot.o
i686-elf-as arch/x86/gdt.S -o gdt.o
i686-elf-as arch/x86/interrupt.S -o interrupt.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/io.c -o io.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/descriptor_tables.c -o descriptor_tables.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/isr.c -o isr.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/timer.c -o timer.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/kheap.c -o kheap.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c arch/x86/paging.c -o paging.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -c core/common.c -o common.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I.
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o gdt.o kheap.o paging.o kernel.o io.o common.o descriptor_tables.o interrupt.o isr.o timer.o -lgcc
qemu-system-i386 -kernel myos.bin

