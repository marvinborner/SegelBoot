/* MIT License, Copyright (c) 2021 Marvin Borner */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start)
phys = 0x7e00;

SECTIONS
{
	. = phys;

	.text :
	{
		*(.text.start)
		*(.text*)
	}

	.rodata :
	{
		*(.rodata*)
	}

	.data :
	{
		*(.data*)
	}

	.bss :
	{
		*(COMMON)
		*(.bss)
	}

	end = .;
}
