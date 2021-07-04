// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef CPU_H
#define CPU_H

#include <def.h>

u8 inb(u16 port);
u16 inw(u16 port);

void outb(u16 port, u8 data);

#endif
