// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef FS_H
#define FS_H

#include <def.h>
struct dev;

struct fs {
	s32 (*read)(void *, u32, u32, struct dev *);
};

void fs_detect(struct dev *dev);

#endif
