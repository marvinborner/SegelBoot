// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef DEVICE_H
#define DEVICE_H

#include <def.h>
#include <disk.h>

enum device_type {
	DEVICE_NONE,
	DEVICE_DISK,
	DEVICE_FB,
};

struct dev {
	u8 id;
	enum device_type type;
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

struct dev *device_get_by_id(u8 id);
struct dev *device_get_by_name(const char *name, u32 len);
void device_foreach(enum device_type type, u8 (*cb)(struct dev *)); // cb=1 => break
u8 device_register(enum device_type type, char *name, u32 data,
		s32 (*read)(void *, u32, u32, struct dev *),
		s32 (*write)(const void *, u32, u32, struct dev *));
void device_print(void);

#endif
