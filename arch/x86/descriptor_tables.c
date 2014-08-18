#include "descriptor_tables.h"

extern void gdt_flush(u32);

static void init_gdt();
static void gdt_set_gate(s32, u32, u32, u8, u8);


gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

void init_descriptor_tables()
{
	init_gdt();
}

static void init_gdt()
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base = (u32)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);			//NULL segnemt
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	//code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	//data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	//user mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	//user mode data segment

	gdt_flush((u32)&gdt_ptr);
}

static void gdt_set_gate(s32 num, u32 base, u32 limit, u8 access, u8 gran)
{
	gdt_entries[num].base_low	= (base & 0xFFFF);
	gdt_entries[num].base_middle	= (base >> 16) & 0xFF;
	gdt_entries[num].base_high	= (base >> 24) & 0xFF;

	gdt_entries[num].limit_low	= (limit & 0xFFFF);
	gdt_entries[num].granularity	= (limit >> 16) & 0x0F;

	gdt_entries[num].granularity	|= gran & 0xF0;
	gdt_entries[num].access		= access;
}
