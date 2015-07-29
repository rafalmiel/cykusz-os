#include "ioapic.h"

#define IOAPIC_REG_ID		0x00
#define IOAPIC_REG_VER		0x01
#define IOAPIC_REG_ARB		0x02
#define IOAPIC_REG_REDTBL_LOW(num)	(0x10 + (2*(num)))
#define IOAPIC_REG_REDTBL_HIGH(num)	(0x11 + (2*(num)))

static u8 *s_ioapic_base = (u8*)0xFEC00000;

typedef union ioapic_reg_id {
	volatile u32 raw;
	struct {
		volatile u32 _reserved		: 24;
		volatile u32 id			: 4;
		volatile u32 _reserved2		: 4;
	} v;
} ioapic_reg_id_t;

typedef union ioapic_reg_ver {
	volatile u32 raw;
	struct {
		volatile u32 apic_version	: 8;
		volatile u32 _reserved		: 8;
		volatile u32 max_red_entry	: 8;
		volatile u32 _reserved2		: 8;
	} v;
} ioapic_reg_ver_t;

typedef union ioapic_reg_arb {
	volatile u32 raw;
	struct {
		volatile u32 _reserved		: 24;
		volatile u32 id			: 4;
		volatile u32 _reserved2		: 4;
	} v;
} ioapic_reg_arb_t;

typedef union ioapic_reg_redtbl_low {
	volatile u32 raw;
	struct {
		volatile u32 intvec		: 8;
		volatile u32 delivery_mode	: 3;
		volatile u32 dest_mode		: 1;
		volatile u32 delivery_status	: 1;
		volatile u32 int_pin_polarity	: 1;
		volatile u32 remote_irr		: 1;
		volatile u32 trigger_mode	: 1;
		volatile u32 int_mask		: 1;
		volatile u32 _reserved		: 15;
	} v;
} ioapic_reg_redtbl_low_t;

typedef union ioapic_reg_redtbl_high {
	volatile u32 raw;
	struct {
		volatile u32 _reserver		: 24;
		volatile u32 destination	: 8;
	} v;
} ioapic_reg_redtbl_high_t;

static inline u32 read_data(u32 reg)
{
	volatile u32 *write = (u32*)s_ioapic_base;
	u32 *read = (u32*)(s_ioapic_base + 0x10);

	*write = reg;

	return *read;
}

static inline void write_data(u32 reg, u32 data)
{
	volatile u32 *write = (u32*)s_ioapic_base;
	volatile u32 *read = (u32*)(s_ioapic_base + 0x10);

	*write = reg;

	*read = data;
}

static void mask_interrupt(u32 i, u32 masked)
{
	ioapic_reg_redtbl_low_t rl;
	ioapic_reg_redtbl_high_t rh;
	rl.raw = 0;
	rl.v.int_mask = masked;
	rh.raw = 0;

	write_data(IOAPIC_REG_REDTBL_LOW(i), rl.raw);
	write_data(IOAPIC_REG_REDTBL_HIGH(i), rh.raw);
}

void ioapic_set_base(u32 *base)
{
	s_ioapic_base = (u8*)base;
}

void init_ioapic()
{
	u32 cnt = ioapic_get_max_red_entries();

	for (u32 i = 0; i < cnt; ++i) {
		mask_interrupt(i, 1);
	}
}

u32 ioapic_get_id(void)
{
	ioapic_reg_id_t id;
	id.raw = read_data(IOAPIC_REG_ID);

	return id.v.id;
}

u32 ioapic_get_max_red_entries(void)
{
	ioapic_reg_ver_t id;
	id.raw = read_data(IOAPIC_REG_VER);

	return id.v.max_red_entry;
}


u32 ioapic_get_version(void)
{
	ioapic_reg_ver_t id;
	id.raw = read_data(IOAPIC_REG_VER);

	return id.v.apic_version;
}


u32 ioapic_get_identification(void)
{
	ioapic_reg_arb_t id;
	id.raw = read_data(IOAPIC_REG_ARB);

	return id.v.id;
}


void ioapic_set_int(u32 i, u32 idt_idx)
{
	ioapic_reg_redtbl_low_t rl;
	ioapic_reg_redtbl_high_t rh;
	rl.raw = 0;
	rl.v.intvec = idt_idx;
	rl.v.int_mask = 0;
	rh.raw = 0;
	rh.v.destination = 0;

	write_data(IOAPIC_REG_REDTBL_LOW(i), rl.raw);
	write_data(IOAPIC_REG_REDTBL_HIGH(i), rh.raw);
}
