; MIT License, Copyright (c) 2021 Marvin Borner

bits 32

MAGIC equ 0xe85250d6
ARCHITECHTURE equ 0
HEADER_LENGTH equ multiboot_header_end - multiboot_header_start
CHECKSUM equ 0x100000000 - (MAGIC + ARCHITECHTURE + HEADER_LENGTH)

multiboot_header_start:
	dd MAGIC
	dd ARCHITECHTURE
	dd HEADER_LENGTH
	dd CHECKSUM
entry_address_tag_start:
	dw 3
	dw 0
	dd entry_address_tag_end - entry_address_tag_start
	dd 0 ; low
	dd boot_entry ; high
entry_address_tag_end:
framebuffer_tag_start:
        dw 5
        dw 0
        dd framebuffer_tag_end - framebuffer_tag_start
        dd 1920
        dd 1200
        dd 32
framebuffer_tag_end:
	dw 0
	dw 0
	dd 8
multiboot_header_end:

global boot_entry
extern kernel_main
boot_entry:
	mov esp, stack_top
	push esp
	push ebx
	push eax
	cli
	call kernel_main
	hlt
	jmp $

section .bss
align 32
stack_bottom:
	resb 0x4000
stack_top:
