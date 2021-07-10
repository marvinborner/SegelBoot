// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef MEM_H
#define MEM_H

#include <def.h>

enum mem_entry_type {
	MEM_NONE,
	MEM_USABLE,
	MEM_RESERVED,
	MEM_RECLAIMABLE,
	MEM_ACPI_NVS,
	MEM_UNUSABLE,
};

// Generalised memory map entry struct
struct mem_entry {
	u32 base;
	u32 length;
	u32 type;
};

struct mem_map {
	struct mem_entry *entry;
	u32 count;
};

void mem_map(void);
struct mem_map *mem_map_get(void);

#endif
