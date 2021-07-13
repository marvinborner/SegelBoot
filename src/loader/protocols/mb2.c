// MIT License, Copyright (c) 2021 Marvin Borner
// Everything according to spec

#include <elf.h>
#include <library.h>
#include <panic.h>
#include <protocols/mb2.h>

// The address where data gets stored
#define MB2_LOAD_ADDRESS 0x10000

struct mb2_entry {
	u32 magic;
	u32 flags;
	u32 header_length;
	u32 checksum; // Everything after that is optional
	u8 *tags;
};

// The (really simple) multiboot checksum algorithm
/* static u32 mb2_checksum(struct mb2_entry *entry) */
/* { */
/* 	return -(entry->magic + entry->flags); */
/* } */

// Load data into memory and return address (not overlapping
static u32 mb2_store(void *data, u32 size)
{
	static u32 offset = 0;
	memcpy((void *)MB2_LOAD_ADDRESS, data, size);
	offset += size;
	return MB2_LOAD_ADDRESS + (size - offset);
}

// Load the mb2 structs into memory
static void mb2_load(struct mb2_entry *entry)
{
	(void)mb2_store;
	(void)entry;
}

// Jump to kernel with correct info pointer in eax
static void mb2_jump(u32 entry, u32 info)
{
	log("Jumping. So long and thanks for all the fish!\n");

	// Move and jump!
	__asm__ volatile("movl $" STRINGIFY(MB2_LOAD_MAGIC) ", %%eax\n\t"
							    "jmpl *%%edi\n\t"
			 :
			 : "D"(entry), "b"(info)
			 : "memory");

	panic("Jumper returned\n");
}

// Detect and verify mb2
u8 mb2_detect(struct config_entry *cfg)
{
	u8 header[8192] = { 0 };

	s32 ret = cfg->dev->p.disk.fs.read(cfg->path, header, 0, sizeof(header), cfg->dev);
	if (ret < 12)
		return 0;

	// Find start of multiboot entry by searching for magic
	struct mb2_entry *entry = 0;
	for (u32 i = 0; i < sizeof(header); i++) {
		u32 *p = (u32 *)&header[i];
		if (*p == MB2_MAGIC) {
			entry = (void *)p;
			break;
		}
	}

	if (!entry)
		return 0;

	// TODO: mb2 checksum
	/* u32 checksum = mb2_checksum(entry); */
	/* if (checksum != entry->checksum) */
	/* 	return 0; */

	cfg->impl.type = IMPL_MB2;
	cfg->impl.offset = (u32)entry - (u32)header;

	return 1;
}

// Execute mb2 type kernel
void mb2_exec(struct config_entry *cfg)
{
	struct mb2_entry mb2_entry = { 0 };
	s32 ret = cfg->dev->p.disk.fs.read(cfg->path, &mb2_entry, cfg->impl.offset,
					   sizeof(mb2_entry), cfg->dev);
	assert(ret == sizeof(mb2_entry));
	mb2_load(&mb2_entry);

	u32 entry = elf_load(cfg->dev, cfg->path);

	// This is a kind of hacky parameter stack pushing thing, just disable warning :)
#pragma GCC diagnostic ignored "-Wpedantic"
	jmp_kernel((void *)mb2_jump, 2, entry, MB2_LOAD_ADDRESS);
#pragma GCC diagnostic pop
}
