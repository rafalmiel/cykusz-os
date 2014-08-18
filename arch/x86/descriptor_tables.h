#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include <core/common.h>

struct gdt_entry_struct
{
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access;
	u8 granularity;
	u8 base_high;
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
	u16 limit;
	u32 base;
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

void init_descriptor_tables();

#endif // DESCRIPTOR_TABLES_H
