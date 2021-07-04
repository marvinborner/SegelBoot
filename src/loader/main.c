// MIT License, Copyright (c) 2021 Marvin Borner

#include <def.h>
#include <log.h>

/**
 * Entry
 */

int start(void);
int start(void)
{
	serial_install();
	log("Hello %d\n", 42);

	while (1)
		;
	return 0;
}
