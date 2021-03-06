// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef LIB_H
#define LIB_H

#include <def.h>

u32 strlen(const char *str);
u32 strnlen(const char *s, u32 max);
u32 strlcpy(char *dst, const char *src, u32 size);
s32 strncmp(const char *s1, const char *s2, u32 n);

void *memcpy(void *dest, const void *src, u32 n);
void *memset(void *dest, u32 val, u32 n);
s32 memcmp(const void *s1, const void *s2, u32 n);

s32 itoa(s32 value, char *buffer, u32 base);
s32 atoi(const char *inp);

#endif
