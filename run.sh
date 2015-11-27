#!/bin/bash

./build.sh

mkdir -p build/isofiles/boot/grub
cp build/kernel.bin build/isofiles/boot/kernel.bin
cp grub.cfg build/isofiles/boot/grub
grub-mkrescue -d /usr/lib/grub/i386-pc/ -o build/kernel.iso build/isofiles 2> /dev/null
rm -r build/isofiles

qemu-system-x86_64 -hda build/kernel.iso
