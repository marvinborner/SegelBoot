// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef DEV_H
#define DEV_H

#include <def.h>
#include <dsk.h>

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

	union {
		struct {
			struct fs fs;
		} disk;
		// TODO: Other (framebuffer?)
	} p; // Prototype union

	u32 data; // Optional (device-specific) data/information
};

struct dev *dev_get_by_id(u8 id);
struct dev *dev_get_by_name(const char *name, u32 len);
void dev_foreach(enum dev_type type, u8 (*cb)(struct dev *)); // cb=1 => break
u8 dev_register(enum dev_type type, char *name, u32 data,
		s32 (*read)(void *, u32, u32, struct dev *),
		s32 (*write)(const void *, u32, u32, struct dev *));
void dev_print(void);

#endif
