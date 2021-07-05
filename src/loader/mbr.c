// MIT License, Copyright (c) 2021 Marvin Borner

#include <lib.h>
#include <log.h>
#include <mbr.h>
#include <pnc.h>

static struct mbr_entry entries[16] = { 0 };

static s32 mbr_read(void *buf, u32 lba, u32 sector_count, struct dev *part)
{
	u8 dev_id = (part->data & 0xff00) >> 8;
	struct dev *dev = dev_get(dev_id);
	assert(dev && dev->type == DEV_DISK && dev->read);

	u8 mbr_id = part->data & 0xff;
	assert(mbr_id < COUNT(entries));
	struct mbr_entry *entry = &entries[mbr_id];

	return dev->read(buf, entry->start + lba, sector_count, dev);
}

static u8 mbr_add_entry(struct mbr_entry *entry)
{
	static u8 i = 0;
	assert(i + 1 < (u8)COUNT(entries));
	entries[i] = *entry;
	return i++;
}

u8 mbr_detect(struct dev *dev)
{
	assert(dev->type == DEV_DISK);

	struct mbr mbr = { 0 };
	dev->read(&mbr, 0, 1, dev); // Read first sector (MBR)

	if (mbr.magic != 0xaa55)
		return 0;

	for (u8 i = 0; i < 4; i++) {
		struct mbr_entry *entry = &mbr.entries[i];
		if (!entry->type || !entry->size)
			continue;

		char name[5] = { 0 };
		strlcpy(name, dev->name, sizeof(name));
		name[3] = '0' + i;

		log("[MBR] Found part %s\n", name);

		// Saving space and everything
		u16 data = mbr_add_entry(entry) | (dev->id << 8);

		dev_register(DEV_DISK, name, data, mbr_read, NULL);
	}

	return 1;
}
