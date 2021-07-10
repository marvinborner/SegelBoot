; MIT License, Copyright (c) 2021 Marvin Borner

extern bss_begin
extern bss_end
extern start
extern gdt

section .entry

global _start
_start:
	cld

	; Clear bss section
	xor al, al
	mov edi, bss_begin
	mov ecx, bss_end
	sub ecx, bss_begin
	rep stosb

	lgdt [gdt] ; Load GDT

	jmp 0x18:.reload_cs

.reload_cs:
	mov eax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp start
