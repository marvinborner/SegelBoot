// MIT License, Copyright (c) 2021 Marvin Borner
// Memory map generator

#include <log.h>
#include <memory.h>
#include <panic.h>
#include <real.h>

/**
 * Memory map using e820 BIOS call
 */

#define E820_MAX_ENTRIES 256 // Spec

enum e820_entry_type {
	E820_MEMORY_NONE,
	E820_MEMORY_USABLE,
	E820_MEMORY_RESERVED,
	E820_MEMORY_RECLAIMABLE,
	E820_MEMORY_ACPI_NVS,
	E820_MEMORY_UNUSABLE,
};

struct e820_entry {
	u32 base_low;
	u32 base_high;
	u32 length_low;
	u32 length_high;
	enum e820_entry_type type;
	u32 unused;
} PACKED;

u16 e820_count = 0;
struct e820_entry e820_map[E820_MAX_ENTRIES] = { 0 };

static u8 memory_e820(void)
{
	struct real_regs r = { 0 };

	for (u32 i = 0; i < E820_MAX_ENTRIES; i++) {
		struct e820_entry entry = { 0 };

		r.eax = 0xe820;
		r.ecx = 24;
		r.edx = 0x534d4150;
		r.edi = (u32)&entry;
		real_int(0x15, &r, &r);

		if (r.eflags & EFLAGS_CF) {
			e820_count = i;
			return 1;
		}

		e820_map[i] = entry;

		if (!r.ebx) {
			e820_count = i++;
			return 1;
		}
	}

	return 0;
}

/**
 * Configure memory map
 */

#define MAP_MAX_ENTRIES 256

static struct memory_entry mem[MAP_MAX_ENTRIES] = { 0 };
static struct memory_map map = { .entry = mem, .count = 0 };

static void memory_map_e820(void)
{
	u32 i;
	for (i = 0; i < COUNT(e820_map); i++) {
		struct e820_entry *e820_entry = &e820_map[i];
		struct memory_entry *map_entry = &mem[i];

		map_entry->base = e820_entry->base_low;
		map_entry->length = e820_entry->length_low;

		// Set type accordingly
		switch (e820_entry->type) {
		case E820_MEMORY_NONE:
			map_entry->type = MEMORY_USABLE;
			continue;
		case E820_MEMORY_USABLE:
			map_entry->type = MEMORY_USABLE;
			break;
		case E820_MEMORY_RESERVED:
			map_entry->type = MEMORY_RESERVED;
			break;
		case E820_MEMORY_RECLAIMABLE:
			map_entry->type = MEMORY_RECLAIMABLE;
			break;
		case E820_MEMORY_ACPI_NVS:
			map_entry->type = MEMORY_ACPI_NVS;
			break;
		case E820_MEMORY_UNUSABLE:
			map_entry->type = MEMORY_UNUSABLE;
			break;
		default:
			panic("Unknown e820 type\n");
		}
	}

	map.count = i;
}

struct memory_map *memory_map_get(void)
{
	return &map;
}

void memory_map(void)
{
	if (memory_e820()) {
		memory_map_e820();
		return;
	}

	panic("Couldn't find memory map\n");
}
