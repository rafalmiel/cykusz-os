#!/bin/bash

make
qemu-system-i386 -kernel myos.bin -initrd LICENSE

