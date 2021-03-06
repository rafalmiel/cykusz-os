#include <core/io.h>
#include "descriptor_tables.h"
#include "isr.h"
#include "common.h"
#include "acpi.h"
#include "localapic.h"
#include "ioapic.h"
#include <core/timer.h>

extern void gdt_flush(u32);
extern void idt_flush(u32);

static void init_gdt(void);
static void init_idt(void);

static void gdt_set_gate(s32, u32, u32, u8, u8);
static void idt_set_gate(u8, u32, u16, u8);

static gdt_entry_t gdt_entries[5];
static gdt_ptr_t gdt_ptr;

static idt_entry_t idt_entries[256];
static idt_ptr_t idt_ptr;

extern isr_t interrupt_handlers[];

static void init_pic(void);

void init_descriptor_tables()
{
	init_gdt();
	init_idt();
	init_pic();
	init_lapic();
	init_ioapic();

	__asm__ volatile("sti");
}

static void init_gdt(void)
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

static void configure_pic(void)
{
	/** ICW1 */
	outb(PIC_MASTER_PORT_A,	0x11);
	outb(PIC_SLAVE_PORT_A,	0x11);

	/** ICW2 */
	outb(PIC_MASTER_PORT_B,	0x20); /* Master offset of 0x20 in the IDT */
	outb(PIC_SLAVE_PORT_B,	0x28); /* Master offset of 0x28 in the IDT */

	/** ICW3 */
	outb(PIC_MASTER_PORT_B, 0x04); /* Slaves attached to IR line 2 */
	outb(PIC_SLAVE_PORT_B,	0x02); /* This slave in IR line 2 of master */

	/** ICW4 */
	outb(PIC_MASTER_PORT_B, 0x01); /* Set as master */
	outb(PIC_SLAVE_PORT_B,	0x01); /* Set as slave */
}

static void disable_pic(void)
{
	outb(PIC_MASTER_PORT_B,	0xFF);
	outb(PIC_SLAVE_PORT_B,	0xFF);
}

static void init_idt(void)
{
	idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base = (u32)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t)*256);
	memset(interrupt_handlers, 0, sizeof(isr_t)*256);

	idt_set_gate(0, (u32)isr0, 0x08, 0x8E); //DPL = priv lvl = 0
	idt_set_gate(1, (u32)isr1, 0x08, 0x8E);
	idt_set_gate(2, (u32)isr2, 0x08, 0x8E);
	idt_set_gate(3, (u32)isr3, 0x08, 0x8E);
	idt_set_gate(4, (u32)isr4, 0x08, 0x8E);
	idt_set_gate(5, (u32)isr5, 0x08, 0x8E);
	idt_set_gate(6, (u32)isr6, 0x08, 0x8E);
	idt_set_gate(7, (u32)isr7, 0x08, 0x8E);
	idt_set_gate(8, (u32)isr8, 0x08, 0x8E);
	idt_set_gate(9, (u32)isr9, 0x08, 0x8E);
	idt_set_gate(10, (u32)isr10, 0x08, 0x8E);
	idt_set_gate(11, (u32)isr11, 0x08, 0x8E);
	idt_set_gate(12, (u32)isr12, 0x08, 0x8E);
	idt_set_gate(13, (u32)isr13, 0x08, 0x8E);
	idt_set_gate(14, (u32)isr14, 0x08, 0x8E);
	idt_set_gate(15, (u32)isr15, 0x08, 0x8E);
	idt_set_gate(16, (u32)isr16, 0x08, 0x8E);
	idt_set_gate(17, (u32)isr17, 0x08, 0x8E);
	idt_set_gate(18, (u32)isr18, 0x08, 0x8E);
	idt_set_gate(19, (u32)isr19, 0x08, 0x8E);
	idt_set_gate(20, (u32)isr20, 0x08, 0x8E);
	idt_set_gate(21, (u32)isr21, 0x08, 0x8E);
	idt_set_gate(22, (u32)isr22, 0x08, 0x8E);
	idt_set_gate(23, (u32)isr23, 0x08, 0x8E);
	idt_set_gate(24, (u32)isr24, 0x08, 0x8E);
	idt_set_gate(25, (u32)isr25, 0x08, 0x8E);
	idt_set_gate(26, (u32)isr26, 0x08, 0x8E);
	idt_set_gate(27, (u32)isr27, 0x08, 0x8E);
	idt_set_gate(28, (u32)isr28, 0x08, 0x8E);
	idt_set_gate(29, (u32)isr29, 0x08, 0x8E);
	idt_set_gate(30, (u32)isr30, 0x08, 0x8E);
	idt_set_gate(31, (u32)isr31, 0x08, 0x8E);

	idt_set_gate(32, (u32)irq0, 0x08, 0x8E);
	idt_set_gate(33, (u32)irq1, 0x08, 0x8E);
	idt_set_gate(34, (u32)irq2, 0x08, 0x8E);
	idt_set_gate(35, (u32)irq3, 0x08, 0x8E);
	idt_set_gate(36, (u32)irq4, 0x08, 0x8E);
	idt_set_gate(37, (u32)irq5, 0x08, 0x8E);
	idt_set_gate(38, (u32)irq6, 0x08, 0x8E);
	idt_set_gate(39, (u32)irq7, 0x08, 0x8E);
	idt_set_gate(40, (u32)irq8, 0x08, 0x8E);
	idt_set_gate(41, (u32)irq9, 0x08, 0x8E);
	idt_set_gate(42, (u32)irq10, 0x08, 0x8E);
	idt_set_gate(43, (u32)irq11, 0x08, 0x8E);
	idt_set_gate(44, (u32)irq12, 0x08, 0x8E);
	idt_set_gate(45, (u32)irq13, 0x08, 0x8E);
	idt_set_gate(46, (u32)irq14, 0x08, 0x8E);
	idt_set_gate(47, (u32)irq15, 0x08, 0x8E);
	idt_set_gate(0xFF, (u32)irq_spurious, 0x08, 0x8E);

	idt_flush((u32)&idt_ptr);
}

static void init_pic(void)
{
	configure_pic();
	disable_pic();
}

static void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags)
{
	idt_entries[num].base_lo	= base & 0xFFFF;
	idt_entries[num].base_hi	= (base >> 16) & 0xFFFF;

	idt_entries[num].sel		= sel;
	idt_entries[num].always0	= 0;

	idt_entries[num].flags		= flags;
}
