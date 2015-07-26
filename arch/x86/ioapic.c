#include "ioapic.h"

#define IOAPIC_REG_ID		0x00
#define IOAPIC_REG_VER		0x01
#define IOAPIC_REG_ARB		0x02
#define IOAPIC_REG_REDTBL_LOW(num)	(0x10 + (2*(num)))
#define IOAPIC_REG_REDTBL_HIGH(num)	(0x11 + (2*(num)))

typedef union ioapic_reg_id {
	u32 raw;
	struct {
		u32 _reserved		: 24;
		u32 id			: 4;
		u32 _reserved2		: 4;
	} v;
} ioapic_reg_id_t;

typedef union ioapic_reg_ver {
	u32 raw;
	struct {
		u32 apic_version	: 8;
		u32 _reserved		: 8;
		u32 max_red_entry	: 8;
		u32 _reserved2		: 8;
	} v;
} ioapic_reg_ver_t;

typedef union ioapic_reg_arb {
	u32 raw;
	struct {
		u32 _reserved		: 24;
		u32 id			: 4;
		u32 _reserved2		: 4;
	} v;
} ioapic_reg_arb_t;

typedef union ioapic_reg_redtbl_low {
	u32 raw;
	struct {
		u32 intvec		: 8;
		u32 delivery_mode	: 3;
		u32 dest_mode		: 1;
		u32 delivery_status	: 1;
		u32 int_pin_polarity	: 1;
		u32 remote_irr		: 1;
		u32 trigger_mode	: 1;
		u32 int_mask		: 1;
	} v;
} ioapic_reg_redtbl_low_t;

typedef union ioapic_reg_redtbl_high {
	u32 raw;
	struct {
		u32 destination		: 8;
		u32 _reserver		: 24;
	} v;
} ioapic_reg_redtbl_high_t;
