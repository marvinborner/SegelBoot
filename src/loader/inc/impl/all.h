// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef IMPL_ALL_H
#define IMPL_ALL_H

#include <def.h>

enum impl_type {
	IMPL_NONE,
	IMPL_MB1,
	IMPL_MB2,
};

struct impl {
	enum impl_type type;
	void *start; // Of header/entry
};

#include <dev.h>

u8 impl_detect(struct dev *dev, const char *path);

#endif
