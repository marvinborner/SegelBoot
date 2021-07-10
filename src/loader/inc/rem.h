// MIT License, Copyright (c) 2021 Marvin Borner
// Real mode emulation - Implementation by Napalm (see rem.asm)

#ifndef REM_H
#define REM_H

#include <def.h>

#define EFLAGS_CF (1 << 0) // Carry flag
#define EFLAGS_ZF (1 << 6) // Zero flag

struct rem_regs {
	u16 gs;
	u16 fs;
	u16 es;
	u16 ds;
	u32 eflags;
	u32 ebp;
	u32 edi;
	u32 esi;
	u32 edx;
	u32 ecx;
	u32 ebx;
	u32 eax;
} PACKED;

void rem_int(u8 int_num, struct rem_regs *out_regs, struct rem_regs *in_regs);

#endif
