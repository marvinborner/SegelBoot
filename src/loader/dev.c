// MIT License, Copyright (c) 2021 Marvin Borner
// Device manager

#include <dev.h>
#include <lib.h>
#include <pnc.h>

static struct dev devices[32] = { 0 };

static const char *dev_resolve_type(enum dev_type type)
{
	switch (type) {
	case DEV_DISK:
		return "Disk";
	case DEV_FB:
		return "Framebuffer";
	default:
		return "Unknown";
	}
}

struct dev *dev_get(u8 id)
{
	assert(id < COUNT(devices));
	return &devices[id];
}

void dev_foreach(enum dev_type type, u8 (*cb)(struct dev *))
{
	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (dev->type == type)
			if (cb(dev))
				break;
	}
}

u8 dev_register(enum dev_type type, char *name, u32 data,
		s32 (*read)(void *, u32, u32, struct dev *),
		s32 (*write)(const void *, u32, u32, struct dev *))
{
	static u8 id = 0;
	assert(++id < 0xff);

	struct dev *dev = &devices[id];
	dev->id = id;
	dev->type = type;
	dev->read = read;
	dev->write = write;
	dev->data = data;

	assert(strlen(name) < sizeof(dev->name));
	memcpy(dev->name, name, sizeof(dev->name));

	if (type == DEV_DISK)
		dsk_detect(dev);

	return id;
}

void dev_print(void)
{
	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (!dev->id)
			continue;
		log("[DEV] %d: %s device: %s\n", dev->id, dev_resolve_type(dev->type), dev->name);
	}
}
