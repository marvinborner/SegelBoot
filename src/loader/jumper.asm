; MIT License, Copyright (c) 2021 Marvin Borner
; Exit the bootloader and clear registers

bits 32

; Empty (invalid) idt
idt:
	dd 0, 0

global jmp_kernel
jmp_kernel:
	cli ; Disable interrupts because of invalid idt
	lidt [idt] ; Load empty (invalid) idt

	xor eax, eax
	lldt ax ; Clear LDT by pointing to nothing

	add esp, 4
	pop edi
	add esp, 4

	mov eax, 0x00000001
	mov cr0, eax ; Clear cr0 except protected mode

	xor eax, eax
	mov cr4, eax ; Clear cr4 completely

	call edi ; Call custom (specified) jumper
