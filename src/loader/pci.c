// MIT License, Copyright (c) 2021 Marvin Borner

#include <cpu.h>
#include <ide.h>
#include <log.h>
#include <pci.h>

// Generate device ID
#define PCI_DEVICE(bus, slot, func) ((u32)((bus << 16) | (slot << 8) | func))

// Extract data from device ID
#define PCI_BUS(device) ((u8)(device >> 16))
#define PCI_SLOT(device) ((u8)(device >> 8))
#define PCI_FUNC(device) ((u8)(device))

// Calculate address
#define PCI_ADDR(device, field)                                                                    \
	(0x80000000 | (u32)(PCI_BUS(device) << 16) | (u32)(PCI_SLOT(device) << 11) |               \
	 (u32)(PCI_FUNC(device) << 8) | ((field)&0xfc))

// Read pci device type
#define PCI_TYPE(device)                                                                           \
	((u32)((pci_read(device, PCI_CLASS, 1) << 16) | (pci_read(device, PCI_SUBCLASS, 1) << 8) | \
	       pci_read(device, PCI_PROG_IF, 1)))

static u32 pci_read(u32 device, u8 field, u32 size)
{
	outl(PCI_ADDR_PORT, PCI_ADDR(device, field));

	if (size == 4) {
		u32 t = inl(PCI_VALUE_PORT);
		return t;
	} else if (size == 2) {
		u16 t = inw((u16)(PCI_VALUE_PORT + (field & 2)));
		return t;
	} else if (size == 1) {
		u8 t = inb((u16)(PCI_VALUE_PORT + (field & 3)));
		return t;
	}

	return PCI_NONE;
}

/**
 * PCI scanning
 */

static void pci_print(u16 vendor_id, u16 device_id, u32 type_id)
{
	const char *vendor = "unknown";
	const char *device = "unknown";
	const char *class = "unknown";

	for (u16 i = 0; i < COUNT(pci_vendors); i++)
		if (pci_vendors[i].id == vendor_id)
			vendor = pci_vendors[i].name;

	for (u16 i = 0; i < COUNT(pci_devices); i++)
		if (pci_devices[i].vendor_id == vendor_id && pci_devices[i].device_id == device_id)
			device = pci_devices[i].name;

	for (u16 i = 0; i < COUNT(pci_types); i++)
		if (pci_types[i].id == type_id)
			class = pci_types[i].name;

	log("[PCI] %s: %s - %s\n", vendor, device, class);
}

static void pci_probe_bus(u8 bus);
static void pci_probe_func(u8 bus, u8 slot, u8 func)
{
	u32 device = PCI_DEVICE(bus, slot, func);

	u16 vendor_id = pci_read(device, PCI_VENDOR_ID, 2);
	u16 device_id = pci_read(device, PCI_DEVICE_ID, 2);
	u32 type_id = PCI_TYPE(device);
	pci_print(vendor_id, device_id, type_id);

	if (PCI_TYPE(device) == PCI_TYPE_BRIDGE)
		pci_probe_bus(pci_read(device, PCI_SECONDARY_BUS, 1));

	if (PCI_IDE(vendor_id, device_id))
		ata_install();
}

static void pci_probe_slot(u8 bus, u8 slot)
{
	u32 device = PCI_DEVICE(bus, slot, 0);
	if (pci_read(device, PCI_VENDOR_ID, 2) == PCI_NONE)
		return;

	// Scan first func
	pci_probe_func(bus, slot, 0);

	if (!pci_read(device, PCI_HEADER_TYPE, 1))
		return;

	// Scan rest
	for (u8 func = 1; func < 8; func++) {
		device = PCI_DEVICE(bus, slot, func);
		if (pci_read(device, PCI_VENDOR_ID, 2) != PCI_NONE)
			pci_probe_func(bus, slot, func);
	}
}

static void pci_probe_bus(u8 bus)
{
	for (u8 slot = 0; slot < 32; slot++)
		pci_probe_slot(bus, slot);
}

void pci_probe(void)
{
	for (u8 func = 0; func < 8; func++) {
		u32 device = PCI_DEVICE(0, 0, func);
		if (pci_read(device, PCI_VENDOR_ID, 2) != PCI_NONE)
			pci_probe_bus(func);
	}
}
