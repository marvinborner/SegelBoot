// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef GDT_H
#define GDT_H

#include <def.h>

struct gdtr {
	u16 limit;
	u32 ptr;
	u32 pad;
} PACKED;

struct gdt_desc {
	u16 limit;
	u16 base_low;
	u8 base_mid;
	u8 access;
	u8 granularity;
	u8 base_hi;
} PACKED;

extern struct gdtr gdt;

#endif
