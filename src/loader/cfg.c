// MIT License, Copyright (c) 2021 Marvin Borner

#include <cfg.h>
#include <dev.h>
#include <lib.h>
#include <log.h>
#include <pnc.h>

// Keys
#define TIMEOUT "TIMEOUT"
#define PATH "PATH"

// Config structure
static struct cfg cfg = { 0 };

// Config file contents (if found)
static char file[1024] = { 0 };

// Find config file
static u8 cfg_find(struct dev *dev)
{
	if (!dev->p.disk.fs.read)
		return 0; // No fs found or not readable - continue!

	s32 res = dev->p.disk.fs.read("/boot/segelboot.cfg", file, 0, sizeof(file), dev);
	if (res > 0)
		return 1; // Break foreach

	// Continue foreach
	return 0;
}

// Checks if index is appropriate as some key/value need to be in entry
static void cfg_in_entry(u8 index)
{
	if (index == 0xff)
		panic("No entry name given\n");
}

// Add/overwrite value by key and entry index
static void cfg_add(u8 index, enum cfg_key key, const char *value)
{
	struct cfg_entry *entry = &cfg.entry[index];
	entry->exists = 1;

	switch (key) {
	case CFG_NAME:
		cfg_in_entry(index);
		strlcpy(entry->name, value, sizeof(entry->name));
		break;
	case CFG_TIMEOUT:
		cfg.timeout = atoi(value);
		break;
	case CFG_PATH:
		cfg_in_entry(index);
		strlcpy(entry->path, value, sizeof(entry->path));
		break;
	case CFG_NONE:
	default:
		panic("Invalid config\n");
	}
}

// TODO: This code is kind of messy
// Structure per line: KEY=VALUE
static void cfg_parse(void)
{
	// Entry index
	u8 entry = 0xff;

	// Value per key
	char value[64] = { 0 };
	u8 value_index = 0;

	// States
	enum cfg_key current = CFG_NONE; // Value key type
	u8 state = 0; // 0 is key, 1 is value, 2 is entry

	const char *start = file; // Start is at the beginning of the key
	for (const char *p = start; *p; p++) {
		if (state == 0) {
			// We're at key parsing
			if (*p == '\n') { // A key can't just end but ok
				start = p + 1;
			} else if (*p == '#') {
				state = 2; // Let's parse the entry name
				p++;
				continue;
			} else if (*p != '=') {
				continue;
			}

			// The key is now at start until p
			u8 diff = p - start;

			// Timeout key
			if (diff == sizeof(TIMEOUT) - 1 && memcmp(start, TIMEOUT, diff) == 0) {
				current = CFG_TIMEOUT;
				state = 1;
				continue;
			}

			// Path key
			if (diff == sizeof(PATH) - 1 && memcmp(start, PATH, diff) == 0) {
				current = CFG_PATH;
				state = 1;
				continue;
			}
		} else if (state == 1) {
			// We're at value parsing
			assert(value_index + 1 < (u8)sizeof(value));
			if (*p == '\n') { // Finished
				value[value_index] = 0;
				cfg_add(entry, current, value);
				value_index = 0;
				state = 0;
				p--; // Repeat parse normally
			} else {
				value[value_index++] = *p;
			}
		} else if (state == 2) {
			// We're at entry name parsing
			assert(value_index + 1 < (u8)sizeof(value));
			if (*p == '\n') { // Finished
				entry = entry == 0xff ? 0 : entry + 1;
				value[value_index] = 0;
				cfg_add(entry, CFG_NAME, value);
				value_index = 0;
				state = 0;
				p--; // Repeat parse normally
			} else {
				value[value_index++] = *p;
			}
		}
	}
}

// Extract the disk from path by returning index of delimiter or 0xff
// Example: disk:/boot/config.cfg returns 4 (:)
static u8 cfg_path_disk(const char *path)
{
	for (const char *p = path; *p; p++)
		if (*p == ':')
			return p - path;
	return 0xff;
}

// Find matching disk dev for every entry and verify path existence and readability
static void cfg_verify(void)
{
	for (u8 i = 0; i < COUNT(cfg.entry) && cfg.entry[i].exists; i++) {
		struct cfg_entry *entry = &cfg.entry[i];

		u8 len = cfg_path_disk(entry->path);
		struct dev *dev = dev_get_by_name(entry->path, len);
		if (!dev || dev->type != DEV_DISK)
			panic("Invalid device in config\n");
		entry->dev = dev;

		if (!dev->p.disk.fs.read)
			panic("Device fs not readable\n");

		// This is now the correct path (due to "disk:PATH")
		const char *path = &entry->path[len + 1];

		u8 buf[1] = { 0 }; // Just for existence-check
		s32 ret = dev->p.disk.fs.read(path, buf, 0, sizeof(buf), dev);
		if (ret != 1 || !buf[0])
			panic("Path is invalid\n");

		if (!impl_detect(dev, path))
			panic("No boot implementation found\n");
	}
}

// Call cb for each entry config
void cfg_foreach(u8 (*cb)(struct cfg_entry *))
{
	for (u8 i = 0; i < COUNT(cfg.entry) && cfg.entry[i].exists; i++) {
		if (cb(&cfg.entry[i])) // 1 means break
			break;
	}
}

// Print all configs and entry values
static void cfg_print(void)
{
	log("[CFG] Global: %d\n", cfg.timeout);

	for (u8 i = 0; i < COUNT(cfg.entry) && cfg.entry[i].exists; i++)
		log("[CFG] Entry: %s at %s\n", cfg.entry[i].name, cfg.entry[i].path);
}

// Execute entry implementation
void cfg_exec(struct cfg_entry *entry)
{
	impl_exec(entry->dev, &entry->path[cfg_path_disk(entry->path) + 1]);
}

void cfg_read(void)
{
	dev_foreach(DEV_DISK, &cfg_find);
	if (!file[0])
		panic("No config found\n");
	cfg_parse();
	cfg_verify();
	cfg_print();
}
