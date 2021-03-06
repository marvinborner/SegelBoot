// MIT License, Copyright (c) 2021 Marvin Borner

#include <disk.h>
#include <fs/ext2.h>
#include <log.h>
#include <mbr.h>
#include <panic.h>

void disk_detect(struct dev *dev)
{
	assert(dev->type == DEVICE_DISK);

	if (mbr_detect(dev))
		return;

	if (ext2_detect(dev))
		return;

	log("[DSK] Couldn't detect type of disk %s\n", dev->name);
}
