CROSS_COMPILE ?= i686-elf
ASM = nasm

OUT = myos.bin

FLAGS = -std=gnu99 -ffreestanding -g -O0 -Wall -Wextra -I.
LD_FLAGS = -ffreestanding -O2 -g -nostdlib

BUILD = build/

PROJ_DIRS := . core arch/x86

C_SOURCES := $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.c")
ASM_SOURCES := $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.S")
C_OBJS := $(patsubst %.c,%.o,$(C_SOURCES))
ASM_OBJS := $(patsubst %.S,%.o,$(ASM_SOURCES))

C_DEPS := $(patsubst %.c,%.d,$(C_SOURCES))

LINKER = linker.ld

all: $(OUT)

$(OUT): $(ASM_OBJS) $(C_OBJS)
	$(CROSS_COMPILE)-gcc -T $(LINKER) -o $(OUT) $(LD_FLAGS) $(ASM_OBJS) $(C_OBJS) -lgcc

-include $(C_DEPS)

%.o: %.S
	$(ASM) -felf $< -o $@

%.o: %.c
	$(CROSS_COMPILE)-gcc $(FLAGS) -MMD -MP -c $< -o $@

clean:
	-rm -rf $(ASM_OBJS)
	-rm -rf $(C_OBJS)
	-rm -rf $(OUT)
	-rm -rf $(C_DEPS)
