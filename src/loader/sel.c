// MIT License, Copyright (c) 2021 Marvin Borner
// Selection interface

#include <cfg.h>
#include <def.h>
#include <log.h>
#include <pnc.h>
#include <sel.h>

struct {
	struct cfg_entry *cfg;
} sel_entries[16] = { 0 };

static u8 sel_entry_add(struct cfg_entry *entry)
{
	static u8 index = 0;
	assert(index + 1 < (u8)sizeof(sel_entries));

	sel_entries[index].cfg = entry;

	index++;
	return 0;
}

// TODO: Keyboard listener
static void sel_entry_select(u8 entry)
{
	cfg_exec(sel_entries[entry].cfg);
}

static void sel_entries_draw(void)
{
	for (u8 i = 0; i < COUNT(sel_entries); i++) {
		if (!sel_entries[i].cfg)
			break;

		vga_log("> '%s' (%s)\n", sel_entries[i].cfg->name, sel_entries[i].cfg->path);
	}
}

void sel_draw(void)
{
	vga_clear();

	cfg_foreach(&sel_entry_add);
	sel_entries_draw();

	// Just for testing
	sel_entry_select(0);
}
