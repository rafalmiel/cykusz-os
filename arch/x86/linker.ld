ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
	. = 0xC0100000;

	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
		*(.rodata*)
	}

	.data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}

	__end = .; __phys_end = . - 0xC0000000;
}
