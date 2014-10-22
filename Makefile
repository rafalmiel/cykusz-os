CROSS_COMPILE	?= i686-elf
ARCH		?= x86

include Makefile.$(ARCH)

FLAGS		= -std=gnu99 -ffreestanding -g -O0 -Wall -Wextra -I. -Iinclude
LD_FLAGS	= -ffreestanding -O2 -g -nostdlib

PROJ_DIRS	:= . \
		   arch/$(ARCH)

ifeq ($(ARCH),x86)
	PROJ_DIRS += core
endif

C_SOURCES	:= $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.c")
ASM_SOURCES	:= $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.S")
C_OBJS		:= $(patsubst %.c,%.o,$(C_SOURCES))
ASM_OBJS	:= $(patsubst %.S,%.o,$(ASM_SOURCES))
C_DEPS		:= $(patsubst %.c,%.d,$(C_SOURCES))

OUT		= myos.bin
LINKER		= arch/$(ARCH)/linker.ld

all: $(OUT)

-include $(C_DEPS)

$(OUT): $(ASM_OBJS) $(C_OBJS) $(LINKER)
	@$(CROSS_COMPILE)-gcc -T $(LINKER) -o $(OUT) $(LD_FLAGS) $(ASM_OBJS) $(C_OBJS) -lgcc
	@echo -e "\e[1;35mLD $(OUT)\e[00m"

%.o: %.S Makefile
	@$(ASM) $(ASM_FLAGS) $< -o $@
	@echo -e "\e[1;31mAS $<\e[00m"

Makefile : Makefile.$(ARCH)

%.o: %.c Makefile
	@$(CROSS_COMPILE)-gcc $(FLAGS) -MMD -MP -c $< -o $@
	@echo -e "\e[1;33mCC $<\e[00m"

run: all
	qemu-system-i386 -kernel myos.bin -initrd LICENSE

clean:
	-rm -rf $(ASM_OBJS)
	-rm -rf $(C_OBJS)
	-rm -rf $(OUT)
	-rm -rf $(C_DEPS)
