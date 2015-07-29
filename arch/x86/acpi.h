#ifndef ACPI_H
#define ACPI_H

#include <core/common.h>

void init_acpi(void);
u32 acpi_remap_irq(u32 irq);

typedef struct acpi_rsdp_desc {
	char signature[8];
	u8 checksum;
	char oemid[6];
	u8 revision;
	u32 rsdt_address;
} __attribute__ ((packed)) acpi_rsdp_desc_t;

typedef struct acpi_matd {
	char signature[4];
	u32 lenght;
	u8 revision;
	u8 checksum;
	char oemid[6];
	char oemtableid[8];
	u32 oemrevision;
	u32 creatorid;
	u32 creatorrevision;
	u32 localcontrolleraddress;
	u32 flags;
} __attribute__((packed)) acpi_matd_t;

typedef struct acpi_matd_entry {
	u8 type;
	u8 lenght;
} __attribute__((packed)) acpi_matd_entry_t;

typedef struct acpi_matd_entry_local_apic {
	u8 procid;
	u8 apicid;
	u32 flags;
} __attribute__((packed)) acpi_matd_entry_local_apic_t;

typedef struct acpi_matd_entry_io_apic {
	u8 ioapicid;
	u8 reserved;
	u32 ioapicaddress;
	u32 globalintbase;
} __attribute__((packed)) acpi_matd_entry_io_apic_t;

typedef struct acpi_matd_entry_intsrc {
	u8 bussrc;
	u8 irqsrc;
	u32 globalsysint;
	u16 flags;
} __attribute__((packed)) acpi_matd_entry_intsrc_t;

#endif // ACPI_H

