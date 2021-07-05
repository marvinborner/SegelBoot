// MIT License, Copyright (c) 2021 Marvin Borner

#include <fs.h>
#include <mbr.h>

void fs_detect(struct dev *dev)
{
	if (mbr_detect(dev))
		return;
}
