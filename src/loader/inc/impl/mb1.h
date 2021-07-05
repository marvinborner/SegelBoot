// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef MB1_H
#define MB1_H

#include <def.h>
#include <dev.h>

#define MB1_MAGIC 0x1badb002

u8 mb1_probe(struct dev *dev);

#endif
