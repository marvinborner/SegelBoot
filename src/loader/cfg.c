// MIT License, Copyright (c) 2021 Marvin Borner

#include <cfg.h>
#include <dev.h>
#include <lib.h>
#include <log.h>
#include <pnc.h>

// Keys
#define TIMEOUT "TIMEOUT"
#define PATH "PATH"

// Config struct (gettable using cfg_get)
struct {
	u32 timeout;
	struct {
		u8 exists : 1;
		char name[64];
		char path[64];
		struct dev *dev;
	} elem[16]; // Up to 16 different selections
} cfg = { 0 };

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

// Checks if index is appropriate as some key/value need to be in element
static void cfg_in_element(u8 index)
{
	if (index == 0xff)
		panic("No element name given\n");
}

// Add/overwrite value by key and element index
static void cfg_add(u8 index, enum cfg_key key, const char *value)
{
	cfg.elem[index].exists = 1;

	switch (key) {
	case CFG_NAME:
		cfg_in_element(index);
		strlcpy(cfg.elem[index].name, value, sizeof(cfg.elem[index].name));
		break;
	case CFG_TIMEOUT:
		cfg.timeout = atoi(value);
		break;
	case CFG_PATH:
		cfg_in_element(index);
		strlcpy(cfg.elem[index].path, value, sizeof(cfg.elem[index].path));
		break;
	case CFG_NONE:
	default:
		panic("Invalid config\n");
	}
}

const void *cfg_get(u8 index, enum cfg_key key)
{
	switch (key) {
	case CFG_NAME:
		return &cfg.elem[index].path;
	case CFG_TIMEOUT:
		return &cfg.timeout;
	case CFG_PATH:
		return &cfg.elem[index].path;
	case CFG_NONE:
	default:
		return NULL;
	}
}

// TODO: This code is kind of messy
// Structure per line: KEY=VALUE
static void cfg_parse(void)
{
	// Element index
	u8 elem = 0xff;

	// Value per key
	char value[64] = { 0 };
	u8 value_index = 0;

	// States
	enum cfg_key current = CFG_NONE; // Value key type
	u8 state = 0; // 0 is key, 1 is value, 2 is elem

	const char *start = file; // Start is at the beginning of the key
	for (const char *p = start; *p; p++) {
		if (state == 0) {
			// We're at key parsing
			if (*p == '\n') { // A key can't just end but ok
				start = p + 1;
			} else if (*p == '#') {
				state = 2; // Let's parse the element name
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
				cfg_add(elem, current, value);
				value_index = 0;
				state = 0;
				p--; // Repeat parse normally
			} else {
				value[value_index++] = *p;
			}
		} else if (state == 2) {
			// We're at element name parsing
			assert(value_index + 1 < (u8)sizeof(value));
			if (*p == '\n') { // Finished
				elem = elem == 0xff ? 0 : elem + 1;
				value[value_index] = 0;
				cfg_add(elem, CFG_NAME, value);
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
	for (u8 i = 0; i < COUNT(cfg.elem) && cfg.elem[i].exists; i++) {
		u8 len = cfg_path_disk(cfg.elem[i].path);
		struct dev *dev = dev_get_by_name(cfg.elem[i].path, len);
		if (!dev || dev->type != DEV_DISK)
			panic("Invalid device in config\n");
		cfg.elem[i].dev = dev;

		if (!dev->p.disk.fs.read)
			panic("Device fs not readable\n");

		// This is now the correct path (due to "disk:PATH")
		const char *path = &cfg.elem[i].path[len + 1];

		u8 buf[1] = { 0 }; // Just for existence-check
		s32 ret = dev->p.disk.fs.read(path, buf, 0, sizeof(buf), dev);
		if (ret != 1 || !buf[0])
			panic("Path is invalid\n");

		if (!impl_detect(dev, path))
			panic("No boot implementation found\n");
	}
}

// Print all configs and entry values
static void cfg_print(void)
{
	log("[CFG] Global: %d\n", cfg.timeout);

	for (u8 i = 0; i < COUNT(cfg.elem) && cfg.elem[i].exists; i++)
		log("[CFG] Element: %s at %s\n", cfg.elem[i].name, cfg.elem[i].path);
}

void cfg_exec(void)
{
	dev_foreach(DEV_DISK, &cfg_find);
	if (!file[0])
		panic("No config found\n");
	cfg_parse();
	cfg_print();
	cfg_verify();
}
