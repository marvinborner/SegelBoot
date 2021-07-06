// MIT License, Copyright (c) 2021 Marvin Borner

#include <acpi.h>
#include <cfg.h>
#include <def.h>
#include <dev.h>
#include <ide.h>
#include <int.h>
#include <log.h>
#include <pci.h>
#include <pic.h>

/**
 * Entry
 */

int start(void);
int start(void)
{
	vga_clear();
	serial_install();

	pic_install();
	idt_install();

	acpi_probe();
	pci_probe();
	dev_print();

	cfg_exec();

	// Sleep and wait for interrupts
	while (1)
		__asm__ volatile("hlt");

	return 0;
}
