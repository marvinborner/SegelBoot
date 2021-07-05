// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef DEV_H
#define DEV_H

#include <def.h>
#include <fs.h>

enum dev_type {
	DEV_DISK,
	DEV_FB,
};

struct dev {
	u8 id;
	enum dev_type type;
	char name[16];

	s32 (*read)(void *, u32, u32, struct dev *);
	s32 (*write)(const void *, u32, u32, struct dev *);

	struct fs fs;

	u32 data; // Optional (device-specific) data/information
	u8 exists : 1;
};

struct dev *dev_get(u8 id);
u8 dev_register(enum dev_type type, char *name, u32 data,
		s32 (*read)(void *, u32, u32, struct dev *),
		s32 (*write)(const void *, u32, u32, struct dev *));
void dev_print(void);

#endif
