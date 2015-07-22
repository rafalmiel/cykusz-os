#include <core/io.h>

#include "acpi.h"
#include "paging.h"


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

static acpi_rsdp_descriptor_t *check_rsdp_ptr(void *addr)
{
	if (memcmp(addr, "RSD PTR ", 8) == 0) {
		if (checksum(addr, sizeof(acpi_rsdp_descriptor_t)) == 0) {
			return addr;
		}
	}

	return 0;
}

static void *find_rsdt_address(void)
{
	acpi_rsdp_descriptor_t *rsdp = 0;
	u32 rsdt_addr = 0;
	u32 *s = (u32*)0xC00E0000;

	for (s = (u32*)0xC00E0000; s < 0xC0100000; s++) {
		rsdp = check_rsdp_ptr(s);

		if (rsdp) {
			rsdt_addr = rsdp->rsdt_address;
			paging_identity_map(rsdt_addr);
			return (void*)(phys_to_virt(rsdt_addr));
		}
	}

	// Look in Extended Bios Data Area if above fails
	u32 ebda = *((u16 *) phys_to_virt(0x40E));	// get pointer
	ebda = phys_to_virt(ebda*0x10 & 0x000FFFFF);	// transform segment into linear address

	for (s = (u32*)ebda; s < ebda + 1024; s++) {
		rsdp = check_rsdp_ptr(s);

		if (rsdp) {
			rsdt_addr = rsdp->rsdt_address;
			paging_identity_map(rsdt_addr);
			return (void*)(phys_to_virt(rsdt_addr));
		}
	}


	return (void *) (0);
}

void init_acpi(void)
{
	u8 *rsdt_addr = find_rsdt_address();

	if (memcmp(rsdt_addr, "RSDT", 4) == 0) {
		kprint("Found RSDT header!\n");
	}
}
