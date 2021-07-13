// MIT License, Copyright (c) 2021 Marvin Borner

#include <a20.h>
#include <acpi.h>
#include <config.h>
#include <def.h>
#include <device.h>
#include <gui.h>
#include <ide.h>
#include <interrupt.h>
#include <log.h>
#include <memory.h>
#include <pci.h>
#include <pic.h>

/**
 * Entry
 */

u8 boot_disk = 0;

int start(u8 disk);
int start(u8 disk)
{
	boot_disk = disk;
	memory_map();
	a20_enable();

	vga_clear();
	serial_install();

	pic_install();
	idt_install();

	pci_probe();
	device_print();

	config_read();
	gui_draw();

	// Sleep and wait for interrupts
	__asm__ volatile("sti");
	while (1)
		__asm__ volatile("hlt");

	return 0;
}
