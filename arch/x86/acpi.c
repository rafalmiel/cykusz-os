#include <core/io.h>

#include "acpi.h"
#include "paging.h"
#include "localapic.h"
#include "ioapic.h"

static u8 *s_madt;

#define MATD_ENTRY_PROC_LOCAL_APIC	0x0
#define MATD_ENTRY_PROC_IOAPIC		0x1
#define MATD_ENTRY_INT_SRC_OVERRIDE	0x2

static u8 checksum(void *addr, u32 size)
{
	u32 i, s = 0;
	u8* b = addr;
	for (i = 0; i < size; ++i) {
		s += *b;
		b++;
	}

	return s;
}

static s32 check_header(void *addr, char *sig, u32 len)
{
	u8 *p = addr;
	u32 l = *(u32*)(p+4);


	if (memcmp(sig, p, len) == 0) {
		if (checksum(p, l) == 0) {
			return 0;
		}
	}

	return -1;
}

static acpi_rsdp_desc_t *check_rsdp_ptr(void *addr)
{
	if (memcmp(addr, "RSD PTR ", 8) == 0) {
		if (checksum(addr, sizeof(acpi_rsdp_desc_t)) == 0) {
			return addr;
		}
	}

	return 0;
}

static void *find_rsdt_address(void)
{
	acpi_rsdp_desc_t *rsdp = 0;
	u32 rsdt_addr = 0;
	u32 *s = (u32*)0xC00E0000;

	for (s = (u32*)0xC00E0000; s < (u32*)0xC0100000; s++) {
		rsdp = check_rsdp_ptr(s);

		if (rsdp) {
			rsdt_addr = rsdp->rsdt_address;
			paging_identity_map(rsdt_addr);
			return (void*)((rsdt_addr));
		}
	}

	// Look in Extended Bios Data Area if above fails
	u32 ebda = *((u16 *) phys_to_virt(0x40E));	// get pointer
	ebda = phys_to_virt(ebda * 0x10 & (0x000FFFFF));	// transform segment into linear address

	for (s = (u32*)ebda; s < (u32*)ebda + 1024; s++) {
		rsdp = check_rsdp_ptr(s);

		if (rsdp) {
			rsdt_addr = rsdp->rsdt_address;
			paging_identity_map(rsdt_addr);
			return (void*)((rsdt_addr));
		}
	}


	return (void *) (0);
}

static inline void log(const char *txt, u32 value)
{
	//kprint(txt);
	//kprint(": ");
	//kprint_hexnl(value);
}

static void parse_matd(void *addr)
{
	u8 *a = addr;
	u8 *limit = a;
	acpi_matd_t *matd = (acpi_matd_t*)a;

	s_madt = (u8*)addr;

	kprint("----------\n");
	kprint("APIC: Local controller address: ");
	kprint_hexnl(matd->localcontrolleraddress);
	kprint("APIC: Flags: ");
	kprint_hexnl(matd->flags);

	paging_identity_map(matd->localcontrolleraddress);
	lapic_set_base((u32*)matd->localcontrolleraddress);

	a += sizeof *matd;

	//kprint_hexnl(e->lenght);
	//kprint_hexnl(e->type);

	limit += matd->lenght;


	while (a < limit) {
		acpi_matd_entry_t *e = (acpi_matd_entry_t *)a;

		//kprint("APIC ENTRY TYPE: ");
		//kprint_hex(e->type);
		//kprint(" ");
		//kprint_hexnl(e->lenght);

		a += 2;

		switch (e->type) {
		case MATD_ENTRY_PROC_LOCAL_APIC:
		{
			acpi_matd_entry_local_apic_t *i =
					(acpi_matd_entry_local_apic_t *)a;
			log("        apic processor id", i->procid);
			log("        apic id", i->apicid);
			log("        flags", i->flags);

			break;
		}
		case MATD_ENTRY_PROC_IOAPIC:
		{
			acpi_matd_entry_io_apic_t *i =
					(acpi_matd_entry_io_apic_t *)a;
			//log("        io apic id", i->ioapicid);
			//log("        io apic address", i->ioapicaddress);
			//log("        global sys int base", i->globalintbase);
			//
			paging_identity_map(i->ioapicaddress);

			//TODO: Make it aware of multiple cpus
			ioapic_set_base((u32*)i->ioapicaddress);
			//log("        ioapic id", ioapic_get_id());
			//log("        ioapic ident", ioapic_get_identification());
			log("        ioapic max ent", ioapic_get_max_red_entries());
			//log("        ioapic ver", ioapic_get_version());

			break;
		}
		case MATD_ENTRY_INT_SRC_OVERRIDE:
		{
			acpi_matd_entry_intsrc_t *i =
					(acpi_matd_entry_intsrc_t *)a;
			//log("        bus source", i->bussrc);
			log("        irq source", i->irqsrc);
			log("        global sys int", i->globalsysint);
			log("        flags", i->flags);

			//for (u32 c = 0; c < 0xF000000; c++) {}
			break;
		}
		default:
			break;
		}

		a += (e->lenght - 2);
	}
}

void init_acpi(void)
{
	u32* rsdt_addr = find_rsdt_address();

	if (memcmp(rsdt_addr, "RSDT", 4) == 0) {
		s32 entries = (*(rsdt_addr + 1) - 36) / 4;

		kprint("Found RSDT header! number of entries: ");
		kprint_intnl(entries);

		rsdt_addr += 36/4;

		while (0 < entries--) {
			paging_identity_map(*rsdt_addr);

			u8 *p = (u8*)(*rsdt_addr);

			if (check_header(p, "APIC", 4) == 0) {
				parse_matd(p);
			}

			for (u32 i = 0; i < 4; ++i) {
				kprint_char(*p++);
			}

			kprint("\n");

			rsdt_addr++;
		}
	}
}


u32 acpi_remap_irq(u32 irq)
{
	u8 *a = s_madt;
	u8 *limit = a;
	acpi_matd_t *matd = (acpi_matd_t*)a;

	a += sizeof *matd;

	limit += matd->lenght;

	while (a < limit) {
		acpi_matd_entry_t *e = (acpi_matd_entry_t *)a;

		a += 2;

		if (e->type == MATD_ENTRY_INT_SRC_OVERRIDE) {
			acpi_matd_entry_intsrc_t *i =
				(acpi_matd_entry_intsrc_t *)a;

			if (i->irqsrc == irq) {
				return i->globalsysint;
			}
		}

		a += (e->lenght - 2);
	}

	return irq;
}
