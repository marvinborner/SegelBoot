; MIT License, Copyright (c) 2021 Marvin Borner

; This file includes definitions to reduce magic numbers
%include "src/entry/definitions.asm"

bits 16 ; Real mode is 16 Bit
org LOCATION ; Bootsector location

global _start
_start:
	jmp .skip_bpb ; Some BIOSes override the BPB area
	dd "START"
	nop
	times 87 db 0 ; Fill BPB area with 0
.skip_bpb:

	; Clear registers (some BIOSes are weird)
	xor bx, bx
	mov ds, bx
	mov es, bx
	mov ss, bx
	; Other registers may contain relevant data

	mov sp, LOCATION ; Set stack (top) pointer - grows downwards

	mov ax, SCREEN_CLEAR ; Clear screen command
	int SCREEN_INT ; CLEAR!

	; Print friendly welcome message
	mov si, hello_msg
	call print

	call disk_support ; Is disk supported? Implicitly verifies that this is a 386+ architecture
	mov esp, LOCATION ; Clear upper 16 Bit of esp, now that 16 Bit support is guaranteed
	cli ; Disable interrupts

	jmp load_stage ; Load and execute main stage

; Print function uses si as string pointer
print:
	; a and b regs get partially destroyed - push for restoration
	push bx
	push ax

	mov ah, SCREEN_OUT ; Set VGA command
	xor bh, bh ; Clear b register (according to BIOS spec)
.putch:
	lodsb ; Load next string byte (using ds:si)
	test al, al ; Test loaded byte
	jz .end ; End if al is zero (using previous test); NULL-termination
	int SCREEN_INT ; WRITE!
	jmp .putch ; Continue
.end:
	; Restore ax/bx
	pop ax
	pop bx
	ret

; Check if disk is supported using ID checks and LBA test
disk_support:
	; BIOS puts the disk id into dl
	cmp dl, 0x80
	jb disk_error ; Error if below 0x80 - probably floppy disk
	cmp dl, 0x8f
	ja disk_error ; Error if above 0x8f - invalid

	; Check if int 0x13 and LBA are supported
	mov ah, DISK_EXT_CHECK ; Set needed interrupt values
	mov bx, DISK_EXT_CHECK_REQ
	int DISK_INT ; CHECK!
	jc disk_error ; Carry means something went wrong
	cmp bx, DISK_EXT_CHECK_RESP
	jne disk_error ; Response is incorrect => error!
	ret

; Read sectors from disk using dap information
disk_read:
	mov si, dap
	mov ah, DISK_READ
	int DISK_INT
	jc error_loop
	ret

; Loads the main stage (main.c)
load_stage:
	mov bx, loader
	mov [dap.dest], bx
	call disk_read

	lgdt [gdt] ; Load GDT

	; Set protected mode (32 Bit mode)
	mov eax, cr0
	or ax, 1 ; Set PE (Protection Enable) Bit
	mov cr0, eax

	jmp 0x08:protected_mode

bits 32
protected_mode:
	; Set segment registers
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	call loader
	jmp error_loop
bits 16

; Error handling
error_loop:
	cli ; Disable interrupts
	hlt ; Wait for interrupt; should wait forever
	jmp error_loop ; Loop if returns anyway

disk_error:
	mov si, disk_msg
	call print
	jmp error_loop

; Messages (NULL-terminated strings with newline/return)
hello_msg db "Hello world! Booting...", NEWLINE, RETURN, NULL
disk_msg db "Disk is invalid or unsupported", NEWLINE, RETURN, NULL

; Disk configuration data for DISK_READ interrupt (filled according to spec)
dap: ; Disk Address Packet
	db 0x10 ; Disk address packet size
	db 0 ; Always 0
.count:
	dw 64 ; Number of sectors (512B each) to read ; 0x8000
.dest:
	dw 0 ; Destination offset
	dw 0 ; Destination segment
.lba:
	dd 1 ; LBA number ; Inter stage is directly after first LBA (0)
	dd 0 ; More storage bytes

; Global Descriptor Table (GDT)
gdt:
	dw .size - 1 + 8 ; GDT size
	dd .start - 8 ; GDT start address
.start:
	; Code
	dw 0xffff ; Limit
	dw 0x0000 ; Base (low 16 bits)
	db 0x00 ; Base (mid 8 bits)
	db 10011010b ; Access
	db 11001111b ; Granularity
	db 0x00 ; Base (high 8 bits)

	; Data
	dw 0xffff ; Limit
	dw 0x0000 ; Base (low 16 bits)
	db 0x00 ; Base (mid 8 bits)
	db 10010010b ; Access
	db 11001111b ; Granularity
	db 0x00 ; Base (high 8 bits)
.end:
.size: equ .end - .start

times SECTOR_SIZE - 2 - ($ - $$) db 0 ; Fill until 512 (SECTOR_SIZE) - 2 bytes; -2 because of 2B signature
dw SECTOR_END_SIG ; Bootsector end signature

loader:
incbin "build/loader.bin"

; Limit to 0x8000 due to ext2 superblock start at 1024 on partition 1
times 0x8000 - ($ - $$) db 0
superblock:
