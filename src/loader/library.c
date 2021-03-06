// MIT License, Copyright (c) 2021 Marvin Borner

#include <library.h>

/**
 * Common string functions
 */

u32 strlen(const char *str)
{
	const char *s = str;
	while (*s)
		s++;
	return s - str;
}

u32 strnlen(const char *str, u32 max)
{
	const char *s = str;
	while (max && *s) {
		s++;
		max--;
	}
	return s - str;
}

u32 strlcpy(char *dst, const char *src, u32 size)
{
	const char *orig = src;
	u32 left = size;

	if (left)
		while (--left)
			if (!(*dst++ = *src++))
				break;

	if (!left) {
		if (size)
			*dst = 0;
		while (*src++)
			;
	}

	return src - orig - 1;
}

s32 strncmp(const char *s1, const char *s2, u32 n)
{
	const u8 *c1 = (const u8 *)s1;
	const u8 *c2 = (const u8 *)s2;
	u8 ch;
	s32 d = 0;

	while (n--) {
		d = (s32)(ch = *c1++) - (s32)*c2++;
		if (d || !ch)
			break;
	}

	return d;
}

/**
 * Common memory functions
 */

void *memcpy(void *dest, const void *src, u32 n)
{
	// Inspired by jgraef at osdev
	u32 num_dwords = n / 4;
	u32 num_bytes = n % 4;
	u32 *dest32 = (u32 *)dest;
	const u32 *src32 = (const u32 *)src;
	u8 *dest8 = ((u8 *)dest) + num_dwords * 4;
	const u8 *src8 = ((const u8 *)src) + num_dwords * 4;

	__asm__ volatile("rep movsl\n"
			 : "=S"(src32), "=D"(dest32), "=c"(num_dwords)
			 : "S"(src32), "D"(dest32), "c"(num_dwords)
			 : "memory");

	for (u32 i = 0; i < num_bytes; i++)
		dest8[i] = src8[i];

	return dest;
}

void *memset(void *dest, u32 val, u32 n)
{
	// Inspired by jgraef at osdev
	u32 uval = val;
	u32 num_dwords = n / 4;
	u32 num_bytes = n % 4;
	u32 *dest32 = (u32 *)dest;
	u8 *dest8 = ((u8 *)dest) + num_dwords * 4;
	u8 val8 = (u8)val;
	u32 val32 = uval | (uval << 8) | (uval << 16) | (uval << 24);

	__asm__ volatile("rep stosl\n"
			 : "=D"(dest32), "=c"(num_dwords)
			 : "D"(dest32), "c"(num_dwords), "a"(val32)
			 : "memory");

	for (u32 i = 0; i < num_bytes; i++)
		dest8[i] = val8;

	return dest;
}

s32 memcmp(const void *s1, const void *s2, u32 n)
{
	const u8 *a = (const u8 *)s1;
	const u8 *b = (const u8 *)s2;
	for (u32 i = 0; i < n; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

/**
 * Conversion
 */

s32 itoa(s32 value, char *buffer, u32 base)
{
	char tmp[16];
	char *tp = tmp;
	s32 i;
	unsigned v;

	s32 sign = (base == 10 && value < 0);
	if (sign)
		v = -value;
	else
		v = (unsigned)value;

	while (v || tp == tmp) {
		i = v % base;
		v /= base;
		if (i < 10)
			*tp++ = i + '0';
		else
			*tp++ = i + 'a' - 10;
	}

	s32 len = tp - tmp;

	if (sign) {
		*buffer++ = '-';
		len++;
	}

	while (tp > tmp)
		*buffer++ = *--tp;
	*buffer = '\0';

	return len;
}

// From stackoverflow
s32 atoi(const char *inp)
{
	s32 ret = 0;
	while (*inp) {
		ret = (ret << 3) + (ret << 1) + (*inp) - '0';
		inp++;
	}
	return ret;
}
