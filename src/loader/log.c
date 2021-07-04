#include <assert.h>
#include <cpu.h>
#include <lib.h>
#include <log.h>

/**
 * Formatting
 */

static u32 vsnprintf(char *str, u32 size, const char *format, va_list ap)
{
	u32 length = 0;

	int temp_int;
	char temp_ch;
	char *temp_str;

	char buffer[64] = { 0 };

	// TODO: Fix potential memory overflows because of str[length++]=xxx
	char ch;
	while ((ch = *format++)) {
		if (ch == '%') {
			switch (*format++) {
			case '%':
				str[length++] = '%';
				break;
			case 'c':
				temp_ch = va_arg(ap, int);
				str[length++] = temp_ch;
				break;
			case 's':
				temp_str = va_arg(ap, char *);
				length += strlcpy(&str[length], temp_str, size - length);
				break;
			case 'b':
				temp_int = va_arg(ap, int);
				itoa(temp_int, buffer, 2);
				length += strlcpy(&str[length], buffer, size - length);
				break;
			case 'd':
				temp_int = va_arg(ap, int);
				itoa(temp_int, buffer, 10);
				length += strlcpy(&str[length], buffer, size - length);
				break;
			case 'x':
				temp_int = va_arg(ap, int);
				itoa(temp_int, buffer, 16);
				length += strlcpy(&str[length], buffer, size - length);
				break;
			default:
				serial_print("Unknown printf format\n");
			}
		} else {
			str[length++] = ch;
		}
	}

	return length;
}

/**
 * Serial
 */

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

/**
 * VGA
 */

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xb8000

void vga_clear(void)
{
	u16 *out = (u16 *)VGA_ADDRESS;
	for (u16 i = 0; i < 80 * 25; i++)
		out[i] = 0;
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

/**
 * Universal print function
 */

void log(const char *format, ...)
{
	char buf[1024] = { 0 };

	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);

	serial_print(buf);
	vga_print(buf);
}
