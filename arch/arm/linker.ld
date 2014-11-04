/* link-arm-eabi.ld - linker script for arm eabi */
ENTRY(_start)
SECTIONS
{
	. = 0x8000;
	.init : {
		KEEP(*(.init))
	}

	.text : {
		*(.rodata*)
		*(.text)
	}

	.data : {
		*(.data)
	}
}
