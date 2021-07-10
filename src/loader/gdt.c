// MIT License, Copyright (c) 2021 Marvin Borner

#include <gdt.h>

#define GDT_MAX_LIMIT 0xffff // General max access limit without extra nibble
#define GDT_EXTRA_MAX_LIMIT (0x0f) // Extra limit nibble
#define GDT_PRESENT (1 << 7) // Makes the descriptor valid
#define GDT_DESCRIPTOR (1 << 4) // Should be set for code/data segments
#define GDT_EXECUTABLE (1 << 3) // Makes the segment executable (for code segments)
#define GDT_READWRITE (1 << 1) // Makes the segment read/writeable
#define GDT_SIZE (0x40) // Enable 32-Bit
#define GDT_GRANULARITY (0x80) // Enable 4K granularity for limit

static struct gdt_desc gdt_descs[] = {
	// NULL descriptor - Offset 0
	{ 0 },

	/**
	 * These 16-Bit access segments get used by 16-Bit real mode emulation
	 *
	 * Limit + extra limit gets multiplied by 1 (no granularity/size bit)
	 */

	// Standard code segment // Offset 1
	// Access from (0) to (0xffff * 1 = 64KiB)
	{
		.limit = GDT_MAX_LIMIT,
		.base_low = 0x0000,
		.base_mid = 0x00,
		.access = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE,
		.flags = 0x00,
		.base_hi = 0x00,
	},

	// Standard data segment // Offset 2
	// Access from (0) to (0xffff * 1 = 64KiB)
	{
		.limit = GDT_MAX_LIMIT,
		.base_low = 0x0000,
		.base_mid = 0x00,
		.access = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE,
		.flags = 0x00,
		.base_hi = 0x00,
	},

	/**
	 * These 32-Bit full access segments get uses by the bootloader to load
	 * ELF programs no matter whether they want to be loaded to 0-4GiB addr
	 *
	 * Limit + extra limit gets multiplied by 4096 (<< 12) with added 1s
	 */

	// Full access code segment // Offset 3
	// Access from (0) to ((0xfffff << 12) + 0xfff = 4GiB)
	{
		.limit = GDT_MAX_LIMIT,
		.base_low = 0x0000,
		.base_mid = 0x00,
		.access = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE,
		.flags = GDT_GRANULARITY | GDT_SIZE | GDT_EXTRA_MAX_LIMIT,
		.base_hi = 0x00,
	},

	// Full access data segment // Offset 4
	// Access from (0) to ((0xfffff << 12) + 0xfff = 4GiB)
	{
		.limit = GDT_MAX_LIMIT,
		.base_low = 0x0000,
		.base_mid = 0x00,
		.access = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE,
		.flags = GDT_GRANULARITY | GDT_SIZE | GDT_EXTRA_MAX_LIMIT,
		.base_hi = 0x00,
	},
};

REAL struct gdtr gdt = {
	sizeof(gdt_descs) - 1,
	(u32)gdt_descs,
	0,
};
