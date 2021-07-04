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
