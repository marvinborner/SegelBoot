// MIT License, Copyright (c) 2021 Marvin Borner

#include <dsk.h>
#include <fs/ext2.h>
#include <log.h>
#include <mbr.h>

void dsk_detect(struct dev *dev)
{
	if (mbr_detect(dev))
		return;

	if (ext2_detect(dev))
		return;

	log("[DSK] Couldn't detect type of disk %s\n", dev->name);
}
