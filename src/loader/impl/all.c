// MIT License, Copyright (c) 2021 Marvin Borner

#include <impl/all.h>
#include <impl/mb1.h>
#include <pnc.h>

u8 impl_detect(struct dev *dev, const char *path)
{
	if (mb1_detect(dev, path))
		return 1;

	return 0;
}

void impl_exec(struct dev *dev, const char *path)
{
	assert(dev->type == DEV_DISK);

	switch (dev->p.disk.impl.type) {
	case IMPL_MB1:
		mb1_exec(dev, path);
		break;
	case IMPL_MB2:
	case IMPL_NONE:
	default:
		panic("Invalid implementation\n");
	}

	panic("Couldn't execute implementation\n");
}
