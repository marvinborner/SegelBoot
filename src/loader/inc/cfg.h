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

struct cfg_entry {
	u8 exists : 1;
	char name[64];
	char path[64];
	struct dev *dev;
};

struct cfg {
	u32 timeout;
	struct cfg_entry entry[16]; // Up to 16 different entries
};

void cfg_foreach(u8 (*cb)(struct cfg_entry *));
void cfg_exec(struct cfg_entry *entry);
void cfg_read(void);

#endif
