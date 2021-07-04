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

void serial_install(void)
{
	outb(0x3f8 + 1, 0x00);
	outb(0x3f8 + 3, 0x80);
	outb(0x3f8 + 0, 0x03);
	outb(0x3f8 + 1, 0x00);
	outb(0x3f8 + 3, 0x03);
	outb(0x3f8 + 2, 0xc7);
	outb(0x3f8 + 4, 0x0B);
}

static int is_transmit_empty(void)
{
	return inb(0x3f8 + 5) & 0x20;
}

static void serial_put(char ch)
{
	while (is_transmit_empty() == 0)
		;
	outb(0x3f8, (u8)ch);
}

void serial_print(const char *data)
{
	for (const char *p = data; *p; p++)
		serial_put(*p);
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
}
