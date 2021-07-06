// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef LOG_H
#define LOG_H

void serial_install(void);
void serial_print(const char *data);

void vga_clear(void);

void log(const char *format, ...);
void vga_log(const char *format, ...);

#endif
