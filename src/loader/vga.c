// MIT License, Copyright (c) 2021 Marvin Borner

#include <log.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xb8000

void vga_clear(void)
{
	u16 *out = (u16 *)VGA_ADDRESS;
	for (u16 i = 0; i < 80 * 25; i++)
		out[i] = 0;
}

void vga_put_at(char ch, u8 x, u8 y, u8 color)
{
	u8 *out = (u8 *)(VGA_ADDRESS + 2 * (x + y * VGA_WIDTH));
	*out++ = ch;
	*out++ = color;
}

static void vga_put(char ch)
{
	static u8 x = 0;
	static u8 y = 0;

	if (ch == '\n') {
		x = 0;
		y++;
		return;
	} else if (x + 1 == VGA_WIDTH) {
		x = 0;
		y++;
	} else if (y + 1 == VGA_HEIGHT) {
		x = 0;
		y = 0;
		vga_clear();
	}

	u8 *out = (u8 *)(VGA_ADDRESS + 2 * (x + y * VGA_WIDTH));
	*out++ = ch;
	*out++ = 0x07;

	x++;
}

static void vga_print(const char *data)
{
	for (const char *p = data; *p; p++)
		vga_put(*p);
}

void vga_log(const char *format, ...)
{
	char buf[1024] = { 0 };

	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	vga_print(buf);
}
