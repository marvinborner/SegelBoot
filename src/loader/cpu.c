// MIT License, Copyright (c) 2021 Marvin Borner

#include <cpu.h>

/**
 * CPU serial I/O
 */

u8 inb(u16 port)
{
	u8 value;
	__asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

u16 inw(u16 port)
{
	u16 value;
	__asm__ volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

u32 inl(u16 port)
{
	u32 value;
	__asm__ volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

void outb(u16 port, u8 data)
{
	__asm__ volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

void outw(u16 port, u16 data)
{
	__asm__ volatile("outw %0, %1" ::"a"(data), "Nd"(port));
}

void outl(u16 port, u32 data)
{
	__asm__ volatile("outl %0, %1" ::"a"(data), "Nd"(port));
}

/**
 * Move memory
 */

#define WORD_PTR(PTR) (*((u16 *)(PTR)))
u16 mminw(u32 addr)
{
	u16 ret;
	__asm__ volatile("movw %1, %0" : "=r"(ret) : "m"(WORD_PTR(addr)) : "memory");
	return ret;
}

void mmoutw(u32 addr, u16 value)
{
	__asm__ volatile("movw %1, %0" : "=m"(WORD_PTR(addr)) : "ir"(value) : "memory");
}
