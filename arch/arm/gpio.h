#ifndef GPIO_H
#define GPIO_H

#include <core/common.h>

#define GPIO_BASE 0x20200000
#define GPIO_ON_BASE (GPIO_BASE + 28)
#define GPIO_OFF_BASE (GPIO_BASE + 40)

#define GPIO_PIN_PWRLED 35
#define GPIO_PIN_ACTLED 47

void gpio_actled_select();
void gpio_actled_on();
void gpio_actled_off();

void gpio_pwrled_select();
void gpio_pwrled_on();
void gpio_pwrled_off();

static inline void gpio_select(u8 pin)
{
	u32 addr = GPIO_BASE + (pin / 10)*4;
	u32 bit = (1 << (pin % 10)*3);

	volatile u32 *a = (u32*)addr;
	*a |= bit;
}

static inline void gpio_set(u32 pin)
{
	u32 addr = GPIO_ON_BASE + (pin / 32)*4;
	u32 bit = (1 << (pin % 32));

	volatile u32 *a = (u32*)addr;
	*a |= bit;
}

static inline void gpio_clear(u32 pin)
{
	u32 addr = GPIO_OFF_BASE + (pin / 32)*4;
	u32 bit = (1 << (pin % 32));

	volatile u32 *a = (u32*)addr;
	*a |= bit;
}

#endif // GPIO_H
