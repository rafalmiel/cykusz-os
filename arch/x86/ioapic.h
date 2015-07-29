#ifndef APIC_H
#define APIC_H

#include <core/common.h>

void ioapic_set_base(u32 *base);

void init_ioapic(void);
void ioapic_set_int(u32 i, u32 idt_idx);

u32 ioapic_get_id(void);
u32 ioapic_get_max_red_entries(void);
u32 ioapic_get_version(void);
u32 ioapic_get_identification(void);

#endif // APIC_H

