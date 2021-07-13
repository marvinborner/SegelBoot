// MIT License, Copyright (c) 2021 Marvin Borner
// Device manager

#include <device.h>
#include <library.h>
#include <panic.h>

static struct dev devices[32] = { 0 };

static const char *device_resolve_type(enum device_type type)
{
	switch (type) {
	case DEVICE_DISK:
		return "Disk";
	case DEVICE_FB:
		return "Framebuffer";
	case DEVICE_NONE:
	default:
		return "Unknown";
	}
}

struct dev *device_get_by_id(u8 id)
{
	assert(id < COUNT(devices));
	return &devices[id];
}

struct dev *device_get_by_name(const char *name, u32 len)
{
	if (!name || !name[0])
		return NULL;

	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (strncmp(dev->name, name, MIN(sizeof(dev->name), len)) == 0)
			return dev;
	}

	return NULL;
}

void device_foreach(enum device_type type, u8 (*cb)(struct dev *))
{
	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (dev->type == type)
			if (cb(dev))
				break;
	}
}

u8 device_register(enum device_type type, char *name, u32 data,
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

	if (type == DEVICE_DISK)
		disk_detect(dev);

	return id;
}

void device_print(void)
{
	for (u8 i = 0; i < COUNT(devices); i++) {
		struct dev *dev = &devices[i];
		if (!dev->id)
			continue;
		log("[DEV] %d: %s device: %s\n", dev->id, device_resolve_type(dev->type),
		    dev->name);
	}
}
