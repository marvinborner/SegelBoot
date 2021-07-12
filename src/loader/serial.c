// MIT License, Copyright (c) 2021 Marvin Borner

#include <cpu.h>
#include <log.h>
#include <panic.h>

#define PORT 0x3f8

void serial_install(void)
{
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03);
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03);
	outb(PORT + 2, 0xc7);

	// Test serial chip
	outb(PORT + 4, 0x1e); // Enable loopback
	outb(PORT + 0, 0xae); // Write
	assert(inb(PORT + 0) == 0xae); // Verify receive

	// Activate
	outb(PORT + 4, 0x0f);

	log("[LOG] Initiated\n");
}

static int serial_empty(void)
{
	return inb(PORT + 5) & 0x20;
}

static void serial_put(char ch)
{
	while (serial_empty() == 0)
		;
	outb(PORT, (u8)ch);
}

void serial_print(const char *data)
{
	for (const char *p = data; *p; p++)
		serial_put(*p);
}

void log(const char *format, ...)
{
	char buf[1024] = { 0 };

	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	serial_print(buf);
}
