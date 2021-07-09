// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef CFG_H
#define CFG_H

#include <def.h>
#include <impl.h>

extern u8 boot_disk;

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
	char full_path[64]; // With disk name
	const char *path; // Without disk name
	struct dev *dev;
	struct impl impl;
};

struct cfg {
	u32 timeout;
	struct cfg_entry entry[16]; // Up to 16 different entries
};

void cfg_foreach(u8 (*cb)(struct cfg_entry *));
void cfg_exec(struct cfg_entry *entry);
void cfg_read(void);

#endif
