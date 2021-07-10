// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef LOG_H
#define LOG_H

#include <def.h>

void serial_install(void);
void serial_print(const char *data);

void vga_put_at(char ch, u8 x, u8 y, u8 color);
void vga_clear(void);

void log(const char *format, ...);
void vga_log(const char *format, ...);

#endif
