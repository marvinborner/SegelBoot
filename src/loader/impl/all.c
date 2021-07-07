// MIT License, Copyright (c) 2021 Marvin Borner

#include <impl/all.h>
#include <impl/mb1.h>
#include <impl/mb2.h>
#include <pnc.h>

u8 impl_detect(struct cfg_entry *cfg)
{
	if (mb1_detect(cfg))
		return 1;

	if (mb2_detect(cfg))
		return 1;

	return 0;
}

void impl_exec(struct cfg_entry *cfg)
{
	assert(cfg->dev->type == DEV_DISK);

	switch (cfg->impl.type) {
	case IMPL_MB1:
		mb1_exec(cfg);
		break;
	case IMPL_MB2:
		mb2_exec(cfg);
		break;
	case IMPL_NONE:
	default:
		panic("Invalid implementation\n");
	}

	panic("Couldn't execute implementation\n");
}
