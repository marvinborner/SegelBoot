// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef ASSERT_H
#define ASSERT_H

#include <log.h>
#define assert(exp)                                                                                \
	{                                                                                          \
		if (!(exp)) {                                                                      \
			log("%s:%d: %s: Kernel assertion '%s' failed\n", __FILE__, __LINE__,       \
			    __func__, #exp);                                                       \
			while (1)                                                                  \
				__asm__ volatile("cli\nhlt");                                      \
		}                                                                                  \
	}

#endif
