/* link-arm-eabi.ld - linker script for arm eabi */
SECTIONS
{
	.init 0x8000 : {
		KEEP(*(.init))
	}

	.text : {
		*(.rodata)
		*(.text)
	}

	.data : {
		*(.data)
	}

	/DISCARD/ : {
		*(*)
	}
}
