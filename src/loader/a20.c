// MIT License, Copyright (c) 2021 Marvin Borner
// https://wiki.osdev.org/A20_Line

#include <a20.h>
#include <cpu.h>
#include <def.h>
#include <pnc.h>
#include <rem.h>

static u8 a20_check(void)
{
	if (mminw(0x7dfe) != mminw(0x7dfe + 0x100000))
		return 1;

	mmoutw(0x7dfe, ~mminw(0x7dfe));

	if (mminw(0x7dfe) != mminw(0x7dfe + 0x100000))
		return 1;

	return 0;
}

void a20_enable(void)
{
	if (a20_check())
		return;

	// BIOS method
	struct rem_regs r = { 0 };
	r.eax = 0x2401;
	rem_int(0x15, &r, &r);

	if (a20_check())
		return;

	// Keyboard controller method
	while (inb(0x64) & 2)
		;
	outb(0x64, 0xad);
	while (inb(0x64) & 2)
		;
	outb(0x64, 0xd0);
	while (!(inb(0x64) & 1))
		;
	u8 b = inb(0x60);
	while (inb(0x64) & 2)
		;
	outb(0x64, 0xd1);
	while (inb(0x64) & 2)
		;
	outb(0x60, b | 2);
	while (inb(0x64) & 2)
		;
	outb(0x64, 0xae);
	while (inb(0x64) & 2)
		;

	assert(a20_check());
}
