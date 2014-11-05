#include <stdint.h>

#include <core/io.h>
#include <core/timer.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "isr.h"
#include "mmu.h"

extern void kernel_main();
extern u32 __kernel_init_start;
extern u32 __kernel_text_start;
extern u32 __kernel_data_start;
extern u32 __kernel_bss_start;
extern u32 __kernel_bss_end;

static u32 *s_kernel_table = (u32 *const)0xC0003C00;

void arm_kernel_main(void)
{
	u32 init_start = (u32)&__kernel_init_start;
	u32 text_start = (u32)&__kernel_text_start;
	u32 data_start = (u32)&__kernel_data_start;
	u32 bss_start = (u32)&__kernel_bss_start;
	u32 bss_end = (u32)&__kernel_bss_end;

	/* Remove identity mapping of 1st MB of ram */
	remove_pt_mapping(0x00000000);

	init_interrupts();

	gpio_select(GPIO_PIN_ACTLED);
	gpio_select(GPIO_PIN_PWRLED);

	kernel_main();
	usb_on();

	kprint("Init start: "); kprint_hexnl(init_start);
	kprint("Text start: "); kprint_hexnl(text_start);
	kprint("Data start: "); kprint_hexnl(data_start);
	kprint("Bss  start: "); kprint_hexnl(bss_start);
	kprint("Bss    end: "); kprint_hexnl(bss_end);

	for (u32 x = 0; x < 16; ++x) {
		kprint_int(x);
		kprint(" ");
		kprint_hexnl(s_kernel_table[x]);
	}

	while(1) {}
}
