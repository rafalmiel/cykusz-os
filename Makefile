CROSS_COMPILE	?= i686-elf
ARCH		?= x86

include Makefile.$(ARCH)

FLAGS		= -std=gnu11 -ffreestanding -O3 -nostdlib -Wall -Wextra  \
			-fshort-wchar -I. -Iinclude -Iarch/$(ARCH)/include \
			-D__arch_$(ARCH)
LD_FLAGS	= -ffreestanding -O3 -nostdlib -Wl,--build-id=none

PROJ_DIRS	:= . \
		   core \
		   arch/$(ARCH) \
		   drivers/usb/hcd/dwc

C_SOURCES	:= $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.c")
ASM_SOURCES	:= $(shell find $(PROJ_DIRS) -maxdepth 1 -type f -name "*.S")
C_OBJS		:= $(patsubst %.c,%.c.o,$(C_SOURCES))
ASM_OBJS	:= $(patsubst %.S,%.o,$(ASM_SOURCES))
C_DEPS		:= $(patsubst %.c,%.d,$(C_SOURCES))

OUTELF		= myos.elf
OUT		= myos.img
LINKER		:= arch/$(ARCH)/linker.ld

all: $(OUT)

-include $(C_DEPS)

$(OUT): $(OUTELF)
	@$(CROSS_COMPILE)-objcopy $(OUTELF) -O binary $(OUT)
	@echo -e "\e[1;35mOBJCOPY $(OUT)\e[00m"

$(OUTELF): $(ASM_OBJS) $(C_OBJS) $(LINKER)
	@$(CROSS_COMPILE)-gcc -T $(LINKER) -o $(OUTELF) $(LD_FLAGS) $(ASM_OBJS) $(C_OBJS) -lgcc
	@echo -e "\e[1;35mLD $(OUTELF) $(ASM_OBJS) $(C_OBJS)\e[00m"

%.o: %.S Makefile $(LINKER)
	@$(ASM) $(ASM_FLAGS) $< -o $@
	@echo -e "\e[1;31mAS $<\e[00m"

Makefile : Makefile.$(ARCH)

%.c.o: %.c Makefile $(LINKER)
	@$(CROSS_COMPILE)-gcc $(FLAGS) -MMD -MP -c $< -o $@
	@echo -e "\e[1;33mCC $(FLAGS) $<\e[00m"


run: all
	qemu-system-i386 -kernel myos.elf

clean:
	-rm -rf $(ASM_OBJS)
	-rm -rf $(C_OBJS)
	-rm -rf $(OUT)
	-rm -rf $(C_DEPS)
