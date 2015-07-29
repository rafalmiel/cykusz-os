#include <core/common.h>

#include "localapic.h"

#define LAPIC_REG_ID		0x20
#define LAPIC_REG_VER		0x20
#define LAPIC_REG_TPR		0x80
#define LAPIC_REG_APR		0x90
#define LAPIC_REG_PPR		0xA0
#define LAPIC_REG_EOI		0xB0
#define LAPIC_REG_RRD		0xC0
#define LAPIC_REG_LCR		0xD0
#define LAPIC_REG_DFR		0xE0
#define LAPIC_REG_SIVR		0xF0
#define LAPIC_REG_ISR(n)	(0x100 + ((n)*0x10))
#define LAPIC_REG_TMR(n)	(0x180 + ((n)*0x10))
#define LAPIC_REG_IRR(n)	(0x200 + ((n)*0x10))
#define LAPIC_REG_ESR		0x280
#define LAPIC_REG_LVTCMCI	0x2F0
#define LAPIC_REG_ICR		0x300
#define LAPIC_REG_ICR2		0x310
#define LAPIC_REG_LVT_TIMER	0x320
#define LAPIC_REG_LVT_THERMAL	0x330
#define LAPIC_REG_LVT_PERFMON	0x340
#define LAPIC_REG_LVT_LINT0	0x350
#define LAPIC_REG_LVT_LINT1	0x360
#define LAPIC_REG_LVT_LINTERR	0x370
#define LAPIC_REG_ICNT		0x380
#define LAPIC_REG_CCNT		0x390
#define LAPIC_REG_DCNF		0x3E0

static volatile u8 *s_lapic_base = (u8*)0xFEE00000;

void lapic_set_base(u32 *base)
{
	s_lapic_base = (u8*)base;
}

static void lapic_write(u32 reg, u32 value)
{
	*(volatile u32*)(s_lapic_base + reg) = value;
}

static u32 lapic_read(u32 reg)
{
	return *(volatile u32*)(s_lapic_base + reg);
}

void init_lapic(void)
{
	// Clear task priority to enable all interrupts
	lapic_write(LAPIC_REG_TPR, 0);

	// Logical Destination Mode
	lapic_write(LAPIC_REG_DFR, 0xffffffff);	// Flat mode
	lapic_write(LAPIC_REG_LCR, 0x01000000);	// All cpus use logical id 1

	// Configure Spurious Interrupt Vector Register
	lapic_write(LAPIC_REG_SIVR, 0x100 | 0xff);
}


void lapic_end_of_interrupt()
{
	lapic_write(LAPIC_REG_EOI, 0);
}
