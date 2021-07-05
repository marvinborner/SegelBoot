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

u32 dev_register(enum dev_type type, char *name, u32 data,
		 s32 (*read)(void *, u32, u32, struct dev *),
		 s32 (*write)(const void *, u32, u32, struct dev *))
{
	static u8 id = 0;

	struct dev *dev = &devices[id];
	dev->id = id;
	dev->type = type;
	dev->read = read;
	dev->write = write;
	dev->data = data;
	dev->exists = 1;

	assert(strlen(name) < sizeof(dev->name));
	memcpy(dev->name, name, sizeof(dev->name));

	return id++;
}

void dev_print(void)
{
	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (!dev->exists)
			continue;
		log("%d: %s device: %s\n", dev->id, dev_resolve_type(dev->type), dev->name);
	}
}
