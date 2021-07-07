// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef IMPL_MB2_H
#define IMPL_MB2_H

#include <cfg.h>
#include <def.h>
#include <dev.h>

#define MB2_MAGIC 0xe85250d6 // Identifies kernel
#define MB2_LOAD_MAGIC 0x36d76289 // Passed to kernel

struct mb2_header {
	u32 total_size;
	u32 unknown;
} PACKED;

struct mb2_tag {
	u32 type;
	u32 size;
} PACKED;

struct mb2_memory {
	u32 addr_low;
	u32 addr_high;
	u32 size_low;
	u32 size_high;
	u32 type;
	u32 reserved;
} PACKED;

struct mb2_module {
	u32 start;
	u32 end;
	char string[1];
} PACKED;

struct mb2_fb {
	u32 addr_low;
	u32 addr_high;
	u32 pitch;
	u32 width;
	u32 height;
	u8 bpp;
	u8 type;
} PACKED;

enum mb2_tags {
	MB2_TAG_END = 0,
	MB2_TAG_CMDLINE = 1,
	MB2_TAG_MODULE = 3,
	MB2_TAG_MEMORY = 6,
	MB2_TAG_FB = 8,
	MB2_TAG_ACPI_1 = 14,
	MB2_TAG_ACPI_2 = 15,
};

u8 mb2_detect(struct cfg_entry *cfg);
void mb2_exec(struct cfg_entry *cfg);

#endif
