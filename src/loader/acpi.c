// MIT License, Copyright (c) 2021 Marvin Borner

#include <acpi.h>
#include <library.h>
#include <panic.h>

/**
 * General SDP
 */

static u8 acpi_sdp_verify(struct sdp_header *header)
{
	u8 sum = 0;

	for (u32 i = 0; i < sizeof(struct rsdp); i++)
		sum += (u8)(((u8 *)header)[i]);

	return sum == 0;
}

/**
 * General SDT
 */

static u8 acpi_sdt_verify(struct sdt_header *header)
{
	u8 sum = 0;

	for (u32 i = 0; i < header->length; i++)
		sum += (u8)(((u8 *)header)[i]);

	return sum == 0;
}

static void *acpi_sdt_find(struct rsdt *rsdt, const char *signature)
{
	u32 entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

	for (u32 i = 0; i < entries; i++) {
		struct sdt_header *header = (struct sdt_header *)rsdt->sdt_pointer[i];
		if (memcmp(header->signature, signature, 4) == 0) {
			if (acpi_sdt_verify(header))
				return header;
			else
				break;
		}
	}

	return NULL;
}

/**
 * RSDP - points to RSDT
 */

static struct rsdp *acpi_rsdp_find(void)
{
	// Main BIOS area
	for (u32 i = 0xe0000; i < 0xfffff; i++)
		if (memcmp((u32 *)i, RSDP_MAGIC, 8) == 0)
			return (struct rsdp *)i;

	// Or first KB of EBDA?
	u8 *ebda = (void *)(*((u16 *)0x40e) << 4);
	for (u16 i = 0; i < 1024; i += 16)
		if (memcmp(ebda + i, RSDP_MAGIC, 8) == 0)
			return (struct rsdp *)(ebda + i);

	return NULL;
}

/**
 * Probe
 */

void acpi_probe(void)
{
	struct rsdp *rsdp = acpi_rsdp_find();
	assert(rsdp && rsdp->header.revision == 0 && acpi_sdp_verify(&rsdp->header));
	struct rsdt *rsdt = rsdp->rsdt;
	assert(rsdt && memcmp(rsdt->header.signature, RSDT_MAGIC, 4) == 0 &&
	       acpi_sdt_verify(&rsdt->header));

	struct madt *madt = acpi_sdt_find(rsdt, MADT_MAGIC);
	struct fadt *fadt = acpi_sdt_find(rsdt, FADT_MAGIC);
	struct hpet *hpet = acpi_sdt_find(rsdt, HPET_MAGIC);

	// TODO!
	UNUSED(madt);
	UNUSED(fadt);
	UNUSED(hpet);
}
