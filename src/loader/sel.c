// MIT License, Copyright (c) 2021 Marvin Borner
// Selection interface

#include <cfg.h>
#include <cpu.h>
#include <def.h>
#include <int.h>
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

static u8 sel_entry_exists(u8 entry)
{
	return !!sel_entries[entry].cfg;
}

static void sel_entry_select(u8 entry)
{
	if (sel_entry_exists(entry))
		cfg_exec(sel_entries[entry].cfg);
	else
		log("Invalid selection\n");
}

static void sel_entries_draw(void)
{
	for (u8 i = 0; sel_entry_exists(i); i++)
		vga_log("> '%s' (%s)\n", sel_entries[i].cfg->name, sel_entries[i].cfg->path);
}

// Using PS2 magic
static void sel_keyboard_handler(void)
{
	static u8 row = 0;

	while (!(inb(0x64) & 1)) // Wait for data (bit 0 gets set)
		;
	u8 data = inb(0x60);

	// Reset previous selection
	vga_put_at('>', 0, row + 2, 0x07);

	if (data == 0x24 && sel_entry_exists(row + 1)) // j/down key
		row++;
	else if (data == 0x25 && row > 0) // k/up key
		row--;
	else if (data == (0x1c | 0x80)) // enter key release (| 0x80)
		sel_entry_select(row);

	// Draw selection
	vga_put_at('#', 0, row + 2, 0x03);
}

void sel_draw(void)
{
	vga_clear();
	vga_log("SegelBoot by Marvin Borner\n\n");

	cfg_foreach(&sel_entry_add);
	sel_entries_draw();

	int_event_handler_add(1, &sel_keyboard_handler);
}
