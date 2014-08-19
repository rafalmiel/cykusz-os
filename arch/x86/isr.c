#include <core/common.h>

#include "isr.h"
#include "io.h"

void isr_handler(registers_t *reg)
{
	vga_writestring("Received interrupt: ");
	vga_writeint(reg->int_no);
	vga_writestring(" err code: ");
	vga_writeint(reg->err_code);
	vga_writestring("\n");

}
