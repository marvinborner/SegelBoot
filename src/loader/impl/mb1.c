// MIT License, Copyright (c) 2021 Marvin Borner
// Everything according to spec

#include <impl/mb1.h>
#include <pnc.h>

#define MB1_MAGIC 0x1badb002

struct multiboot_entry {
	u32 magic;
	u32 flags;
	u32 checksum; // Everything after that is optional
	u32 header_addr;
	u32 load_addr;
	u32 load_end_addr;
	u32 bss_end_addr;
	u32 entry_addr;
	u32 mode_type;
	u32 width;
	u32 height;
	u32 depth;
};

// The (really simple) multiboot checksum algorithm
static u32 mb1_checksum(struct multiboot_entry *entry)
{
	return -(entry->magic + entry->flags);
}

// Detect and verify mb1
u8 mb1_detect(struct dev *dev, const char *path)
{
	u8 header[8192] = { 0 };

	s32 ret = dev->p.disk.fs.read(path, header, 0, sizeof(header), dev);
	if (ret < 12)
		return 0;

	// Find start of multiboot entry by searching for magic
	struct multiboot_entry *entry = 0;
	for (u32 i = 0; i < sizeof(header); i++) {
		u32 *p = (u32 *)&header[i];
		if (*p == MB1_MAGIC) {
			entry = (void *)p;
			break;
		}
	}

	if (!entry)
		return 0;

	u32 checksum = mb1_checksum(entry);
	if (checksum != entry->checksum)
		return 0;

	dev->p.disk.impl.type = IMPL_MB1;
	dev->p.disk.impl.start = entry;

	return 1;
}
