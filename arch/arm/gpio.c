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
