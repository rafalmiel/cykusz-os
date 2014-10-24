SECTIONS {

	.init 0x8000 :
	{
		*(.init)
	}

	.text :
	{
		*(.text)
		*(.rodata*)
	}

	.data :
	{
		*(.data)
	}

	.bss :
	{
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}
}
