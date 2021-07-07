/* MIT License, Copyright (c) 2021 Marvin Borner */

OUTPUT_ARCH(i386)
ENTRY(boot_entry)
phys = 0x00100000;

SECTIONS
{
	. = phys;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text*)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata*)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data*)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	end = .;
}
