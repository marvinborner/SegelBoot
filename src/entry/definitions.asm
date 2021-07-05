; MIT License, Copyright (c) 2021 Marvin Borner
; This file includes definitions to reduce magic numbers

; Boot constants
%define LOCATION 0x7c00 ; Bootloader location
%define SECTOR_END_SIG 0xaa55 ; Bootsector end signature
%define SECTOR_SIZE 512 ; Bootsector size

; Interrupts
%define SCREEN_INT 0x10 ; Screen/video BIOS interrupt
%define DISK_INT 0x13 ; Disk BIOS interrupt

; Characters
%define NEWLINE 0x0A ; Newline character (\n)
%define RETURN 0x0D ; Return character (\r)
%define NULL 0x00 ; NULL character (\0)

; Screen/video commands (for SCREEN_INT; using VGA interface)
%define SCREEN_CLEAR 0x03 ; Clear screen command
%define SCREEN_OUT 0x0e ; Screen output command

; Disk commands
%define DISK_EXT_CHECK 0x41 ; Disk extension check command
%define DISK_EXT_CHECK_REQ 0x55aa ; First extension check signature (request)
%define DISK_EXT_CHECK_RESP 0xaa55 ; Second extension check signature (response)
%define DISK_READ 0x42 ; Disk extended read command

; GDT constants (bitmap)
%define GDT_MAX_LIMIT 0xffff ; I just use the max limit lel
%define GDT_PRESENT 0b10000000 ; Is present
%define GDT_RING3 0b01100000 ; Privilege level 3
%define GDT_DESCRIPTOR 0b00010000 ; Descriptor type, set for code/data
%define GDT_EXECUTABLE 0b00001000 ; Can be executed
%define GDT_READWRITE 0b00000010 ; Read/write access for code/data
%define GDT_ACCESSED 0b00000001 ; Whether segment is accessible
%define GDT_GRANULARITY 0x80 ; Page granularity (4KiB)
%define GDT_SIZE 0x40 ; Use 32 bit selectors
%define GDT_DATA_OFFSET 0x10 ; Offset to GDT data segment
