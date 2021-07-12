// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef CONFIG_H
#define CONFIG_H

#include <def.h>
#include <protocol.h>

extern u8 boot_disk;

// Global config element index (can actually be anything but wth)
#define CONFIG_GLOBAL 0

enum config_key {
	CONFIG_NONE,
	CONFIG_NAME,
	CONFIG_TIMEOUT,
	CONFIG_PATH,
};

struct config_entry {
	u8 exists : 1;
	char name[64];
	char full_path[64]; // With disk name
	const char *path; // Without disk name
	struct dev *dev;
	struct impl impl;
};

struct cfg {
	u32 timeout;
	struct config_entry entry[16]; // Up to 16 different entries
};

void config_foreach(u8 (*cb)(struct config_entry *));
void config_exec(struct config_entry *entry);
void config_read(void);

#endif
