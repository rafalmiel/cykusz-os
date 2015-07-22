#ifndef ACPI_H
#define ACPI_H

#include <core/common.h>

void init_acpi(void);

typedef struct acpi_rsdp_descriptor {
	char signature[8];
	u8 checksum;
	char oemid[6];
	u8 revision;
	u32 rsdt_address;
} __attribute__ ((packed)) acpi_rsdp_descriptor_t;

#endif // ACPI_H

