// MIT License, Copyright (c) 2021 Marvin Borner
// Selection interface

#include <cfg.h>
#include <def.h>
#include <log.h>
#include <sel.h>

static u8 sel_entry(struct cfg_entry *entry)
{
	vga_log("> '%s' (%s)\n", entry->name, entry->path);
	return 0;
}

void sel_draw(void)
{
	vga_clear();

	cfg_foreach(&sel_entry);
}
