#include "mailbox.h"

#define MBOX_BASE		0x2000B880

#define MBOX_IS_FULL		0x80000000
#define MBOX_IS_EMPTY		0x40000000

#define MBOX_OFFSET_READ	0x00
#define MBOX_OFFSET_PEAK	0x10
#define MBOX_OFFSET_WRITE	0x20
#define MBOX_OFFSET_STATUS	0x18
#define MBOX_OFFSET_SENDER	0x14
#define MBOX_OFFSET_CONFIG	0x1C

static volatile u32 *reg_status =	(u32*)(MBOX_BASE + MBOX_OFFSET_STATUS);
static volatile u32 *reg_write =	(u32*)(MBOX_BASE + MBOX_OFFSET_WRITE);
static volatile u32 *reg_read =		(u32*)(MBOX_BASE + MBOX_OFFSET_READ);

void mailbox_write(u8 channel, u32 data28)
{
	while ((*reg_status & MBOX_IS_FULL) != 0) {}

	*reg_write = ((data28 & 0xFFFFFFF0) | (channel & 0x0F));
}

u32 mailbox_read(u8 channel)
{
	while (1) {
		while ((*reg_status & MBOX_IS_EMPTY) != 0) {}

		u32 data = *reg_read;

		if ((data & 0xF) == channel) {
			return (data & ~0xF);
		}
	}
}
