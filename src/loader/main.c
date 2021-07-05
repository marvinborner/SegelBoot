// MIT License, Copyright (c) 2021 Marvin Borner

#include <def.h>
#include <dev.h>
#include <ide.h>
#include <log.h>
#include <pci.h>

/**
 * Entry
 */
#include <pnc.h>

int start(void);
int start(void)
{
	vga_clear();
	serial_install();

	pci_probe();
	dev_print();

	while (1)
		;
	return 0;
}
