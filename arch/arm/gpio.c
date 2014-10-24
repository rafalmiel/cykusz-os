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
