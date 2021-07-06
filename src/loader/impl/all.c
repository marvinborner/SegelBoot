// MIT License, Copyright (c) 2021 Marvin Borner

#include <impl/all.h>
#include <impl/mb1.h>

u8 impl_detect(struct dev *dev, const char *path)
{
	if (mb1_detect(dev, path))
		return 1;

	return 0;
}
