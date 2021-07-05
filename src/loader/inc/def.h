// MIT License, Copyright (c) 2021 Marvin Borner
// Useful macros/types

#ifndef DEF_H
#define DEF_H

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#define NULL ((void *)0)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define COUNT(a) (sizeof(a) / sizeof 0 [a])
#define UNUSED(a) ((void)a);

#define PACKED __attribute__((packed))

#endif
