#include <core/io.h>
#include "gpio.h"

void gpio_actled_select()
{
	gpio_select(GPIO_PIN_ACTLED);
}

void gpio_actled_on()
{
	gpio_set(GPIO_PIN_ACTLED);
}

void gpio_actled_off()
{
	gpio_clear(GPIO_PIN_ACTLED);
}

void gpio_pwrled_select()
{
	gpio_select(GPIO_PIN_PWRLED);
}

void gpio_pwrled_on()
{
	gpio_set(GPIO_PIN_PWRLED);
}

void gpio_pwrled_off()
{
	gpio_clear(GPIO_PIN_PWRLED);
}

static void sleep()
{
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
}

void leds_start_blinking()
{
	gpio_actled_select();
	gpio_pwrled_select();

	for (;;) {
		gpio_actled_on();
		gpio_pwrled_off();
		sleep();

		gpio_actled_off();
		gpio_pwrled_on();
		sleep();
	}
}


void usb_on()
{
	volatile u32* mailbox;
	u32 result;

	mailbox = (u32*)(IO_BASE + 0x0000B880);
	while (mailbox[6] & 0x80000000);
	mailbox[8] = 0x80;
	do {
		while (mailbox[6] & 0x40000000);
	} while (((result = mailbox[0]) & 0xf) != 0);
	(result == 0x80) ? kprint("USB OK\n") : kprint("USB ERROR\n");
}
