SECTIONS {
	. = 0xC0008000;

	.init : AT(ADDR(.init) - 0xC0000000)
	{
		*(.init)
	}

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
