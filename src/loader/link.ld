/* MIT License, Copyright (c) 2021 Marvin Borner */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)
phys = 0x7e00;

SECTIONS
{
	. = phys;

	.entry :
	{
		*(.entry*)
	}

	.realmode : {
		*(.realmode*)
	}

	.text :
	{
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
		bss_begin = .;
		*(COMMON)
		*(.bss)
		bss_end = .;
	}

	end = .;
}
