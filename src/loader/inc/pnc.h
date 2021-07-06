// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef PNC_H
#define PNC_H

#include <log.h>

#define panic(reason)                                                                              \
	{                                                                                          \
		log("%s:%d: %s: Panic: %s", __FILE__, __LINE__, __func__, (reason));               \
		while (1)                                                                          \
			__asm__ volatile("cli\nhlt");                                              \
	}

#define assert(exp)                                                                                \
	{                                                                                          \
		if (!(exp))                                                                        \
			panic("Assertion '" #exp "' failed\n");                                    \
	}

#endif
