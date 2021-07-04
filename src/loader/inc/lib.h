// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef LIB_H
#define LIB_H

#include <def.h>

u32 strlen(const char *str);
u32 strnlen(const char *s, u32 max);
u32 strlcpy(char *dst, const char *src, u32 size);

int itoa(s32 value, char *buffer, u32 base);

#endif
