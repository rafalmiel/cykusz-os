#!/bin/bash

for f in $(ls -1 *.asm)
do
	nasm -felf64 $f -o build/$f.o
done

ld -n --gc-sections -T linker.ld -o build/kernel.bin $(ls build/*.o)
