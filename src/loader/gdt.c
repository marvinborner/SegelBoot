// MIT License, Copyright (c) 2021 Marvin Borner

#include <gdt.h>

static struct gdt_desc gdt_descs[] = {
	{ 0 },

	{ .limit = 0xffff,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x9a,
	  .granularity = 0x00,
	  .base_hi = 0x00 },

	{ .limit = 0xffff,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x92,
	  .granularity = 0x00,
	  .base_hi = 0x00 },

	{ .limit = 0xffff,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x9a,
	  .granularity = 0xcf,
	  .base_hi = 0x00 },

	{ .limit = 0xffff,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x92,
	  .granularity = 0xcf,
	  .base_hi = 0x00 },

	{ .limit = 0x0000,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x9a,
	  .granularity = 0x20,
	  .base_hi = 0x00 },

	{ .limit = 0x0000,
	  .base_low = 0x0000,
	  .base_mid = 0x00,
	  .access = 0x92,
	  .granularity = 0x00,
	  .base_hi = 0x00 },
};

REAL struct gdtr gdt = {
	sizeof(gdt_descs) - 1,
	(u32)gdt_descs,
	0,
};
