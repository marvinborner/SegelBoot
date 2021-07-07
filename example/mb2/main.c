// MIT License, Copyright (c) 2021 Marvin Borner

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

static void draw(u8 color, u8 data)
{
	u16 *out = (u16 *)0xb8000;
	for (u16 i = 0; i < 80 * 25; i++) {
		out[i] = data | (color << 8);
	}
}

int kernel_main(u32 magic, u32 addr, u32 esp); // Decl
int kernel_main(u32 magic, u32 addr, u32 esp)
{
	(void)magic;
	(void)addr;
	(void)esp;

	if (magic == 0x36d76289)
		draw(0x02, 'y');
	else
		draw(0x04, 'n');

	while (1)
		;

	return 1;
}
