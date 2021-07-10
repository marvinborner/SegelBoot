// MIT License, Copyright (c) 2021 Marvin Borner

#include <a20.h>
#include <acpi.h>
#include <cfg.h>
#include <def.h>
#include <dev.h>
#include <ide.h>
#include <int.h>
#include <log.h>
#include <mem.h>
#include <pci.h>
#include <pic.h>
#include <sel.h>

/**
 * Entry
 */

u8 boot_disk = 0;

int start(u8 disk);
int start(u8 disk)
{
	boot_disk = disk;
	mem_map();
	a20_enable();

	vga_clear();
	serial_install();

	pic_install();
	idt_install();

	// acpi_probe(); // TODO: Fix slow ACPI probing
	pci_probe();
	dev_print();

	cfg_read();
	sel_draw();

	// Sleep and wait for interrupts
	while (1)
		__asm__ volatile("hlt");

	return 0;
}
