// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef CFG_H
#define CFG_H

#include <def.h>

// Global config element index (can actually be anything but wth)
#define CFG_GLOBAL 0

enum cfg_key {
	CFG_NONE,
	CFG_NAME,
	CFG_TIMEOUT,
	CFG_PATH,
};

const void *cfg_get(u8 index, enum cfg_key key);
void cfg_exec(void);

#endif
