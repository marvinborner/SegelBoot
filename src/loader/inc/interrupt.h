// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <def.h>

#define INTERRUPT_GATE 0x8e
#define INTERRUPT_TRAP 0xef
#define INTERRUPT_USER 0x60
#define IDT_ENTRY(offset, selector, type)                                                          \
	(struct idt_entry)                                                                         \
	{                                                                                          \
		.base_low = (u16)((offset)&0xffff), .sel = (selector), .zero = 0, .flags = (type), \
		.base_high = (u16)(((offset) >> 16) & 0xffff),                                     \
	}

struct interrupt_frame {
	u32 gs, fs, es, ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 interrupt_no, err_code;
	u32 eip, cs, eflags;
} PACKED;

struct interrupt_frame_user {
	u32 gs, fs, es, ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 interrupt_no, err_code;
	u32 eip, cs, eflags;
	u32 useresp, ss;
} PACKED;

struct idt_entry {
	u16 base_low;
	u16 sel; // Kernel segment
	u8 zero; // Always 0
	u8 flags;
	u16 base_high;
} PACKED;

struct idt_ptr {
	u16 size;
	void *base;
} PACKED;

void idt_install(void);
void interrupt_event_handler_add(u32 interrupt_no, void (*handler)(void));

#endif
