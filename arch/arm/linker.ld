/* link-arm-eabi.ld - linker script for arm eabi */

SECTIONS
{
	. = 0x8000;
	.init : {
		KEEP(*(.init))
		*(.isr_vector)
	}

	.text : {
		*(.rodata*)
		*(.text)
	}

	.data : {
		*(.data)
	}
}
