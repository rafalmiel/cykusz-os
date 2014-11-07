/* link-arm-eabi.ld - linker script for arm eabi */
ENTRY(_start)
SECTIONS
{
	. = 0x8000;
	__kernel_init_start = .;
	.init :
	{
		KEEP(*(.init))
		arch/arm/boot.c.o(.text* .rodata* .data* .bss*)
	}

	. = ALIGN(4k);
	__kernel_text_start = .;

	.text (0xC0000000 + __kernel_text_start) : AT(__kernel_text_start)
	{
		*(.rodata*)
		*(.text)
	}

	. = ALIGN(4k) - 0xC0000000;
	__kernel_table_start = .;

	.kernel_table (0xC0000000 + __kernel_table_start) : AT(__kernel_table_start)
	{
		arch/arm/boot.c.o(.kernel_table)
	}

	. = . - 0xC0000000;
	__kernel_data_start = .;

	.data (0xC0000000 + __kernel_data_start) : AT(__kernel_data_start)
	{
		*(.data)
	}

	. = . - 0xC0000000;
	__kernel_bss_start = .;

	.bss (0xC0000000 + __kernel_bss_start) : AT(__kernel_bss_start)
	{
		*(.bss)
		*(COMMON)
	}

	__kernel_bss_end = . - 0xC0000000;
}
