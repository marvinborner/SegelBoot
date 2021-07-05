// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef DSK_H
#define DSK_H

#include <def.h>

// General disk assumptions
#define BLOCK_SIZE 1024
#define BLOCK_COUNT 256 // BLOCK_SIZE / sizeof(u32)
#define SECTOR_SIZE 512
#define SECTOR_COUNT (BLOCK_SIZE / SECTOR_SIZE) // 2

struct dev;
struct fs {
	s32 (*read)(const char *, void *, u32, u32, struct dev *);
};

void dsk_detect(struct dev *dev);

#endif
