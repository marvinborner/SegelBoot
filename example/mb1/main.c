// MIT License, Copyright (c) 2021 Marvin Borner

typedef unsigned int u32;

int kernel_main(u32 magic, u32 addr, u32 esp); // Decl
int kernel_main(u32 magic, u32 addr, u32 esp)
{
	(void)magic;
	(void)addr;
	(void)esp;

	while (1)
		;

	return 1;
}
