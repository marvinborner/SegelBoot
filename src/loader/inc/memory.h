// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef MEMORY_H
#define MEMORY_H

#include <def.h>

enum memory_entry_type {
	MEMORY_NONE,
	MEMORY_USABLE,
	MEMORY_RESERVED,
	MEMORY_RECLAIMABLE,
	MEMORY_ACPI_NVS,
	MEMORY_UNUSABLE,
};

// Generalised memory map entry struct
struct memory_entry {
	u32 base;
	u32 length;
	u32 type;
};

struct memory_map {
	struct memory_entry *entry;
	u32 count;
};

void memory_map(void);
struct memory_map *memory_map_get(void);

#endif
