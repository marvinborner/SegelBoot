// MIT License, Copyright (c) 2021 Marvin Borner

#include <library.h>
#include <log.h>

/**
 * Formatting
 */

u32 vsnprintf(char *str, u32 size, const char *format, va_list ap)
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
