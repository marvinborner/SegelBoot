// MIT License, Copyright (c) 2021 Marvin Borner

#include <def.h>
#include <ide.h>
#include <log.h>

/**
 * Entry
 */

int start(void);
int start(void)
{
	vga_clear();
	serial_install();

	log("Log initiated\n");

	ata_install();

	while (1)
		;
	return 0;
}
