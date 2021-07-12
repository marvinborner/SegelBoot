// MIT License, Copyright (c) 2021 Marvin Borner

#ifndef IMPL_H
#define IMPL_H

#include <device.h>

enum impl_type {
	IMPL_NONE,
	IMPL_MB1,
	IMPL_MB2,
};

struct impl {
	enum impl_type type;
	u32 offset; // Of header/entry
	void (*load)(struct dev *, const char *);
};

#endif
