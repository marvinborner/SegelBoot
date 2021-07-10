// MIT License, Copyright (c) 2021 Marvin Borner
// Everything according to spec

#include <elf.h>
#include <impl/mb1.h>
#include <lib.h>
#include <mem.h>
#include <pnc.h>

// The address where data gets stored
#define MB1_LOAD_ADDRESS 0x10000

struct mb1_entry {
	u32 magic;
	u32 flags;
	u32 checksum; // Everything after that is optional
	u32 header_addr;
	u32 load_addr;
	u32 load_end_addr;
	u32 bss_end_addr;
	u32 entry_addr;
	u32 mode_type;
	u32 width;
	u32 height;
	u32 depth;
};

// The (really simple) multiboot checksum algorithm
static u32 mb1_checksum(struct mb1_entry *entry)
{
	return -(entry->magic + entry->flags);
}

// Load data into memory and return address (not overlapping
static u32 mb1_store(void *data, u32 size)
{
	static u32 offset = 0;
	memcpy((void *)(MB1_LOAD_ADDRESS + offset), data, size);
	offset += size;
	return MB1_LOAD_ADDRESS + (offset - size);
}

static void mb1_store_mmap(struct mb1_info *info)
{
	struct mem_map *mem_map = mem_map_get();
	info->flags |= MB1_INFO_MEM_MAP;
	info->mmap_length = mem_map->count * sizeof(struct mb1_mmap_entry);
	info->mmap_addr = mb1_store(NULL, 0);
	for (u32 i = 0; i < mem_map->count; i++) {
		struct mb1_mmap_entry mmap_entry;
		mmap_entry.struct_size = sizeof(mmap_entry) - 4;
		mmap_entry.addr_low = mem_map->entry[i].base;
		mmap_entry.len_low = mem_map->entry[i].length;
		mmap_entry.type = mem_map->entry[i].type;
		mb1_store(&mmap_entry, sizeof(mmap_entry));
	}
}

// Load the mb1 structs into memory
static void mb1_load(struct mb1_entry *entry)
{
	(void)entry;

	struct mb1_info info_struct = { 0 };
	struct mb1_info *info = (void *)mb1_store(&info_struct, sizeof(info_struct));

	// Set boot device
	info->flags |= MB1_INFO_BOOTDEV;
	info->boot_device = boot_disk;

	// Set bootloader name
	info->flags |= MB1_INFO_BOOT_LOADER_NAME;
	char loader_name[] = "SegelBoot";
	info->boot_loader_name = mb1_store(loader_name, sizeof(loader_name));

	// Set memory map
	/* if (entry->flags & 2) TODO */
	mb1_store_mmap(info);
}

// Jump to kernel with correct info pointer in eax
static void mb1_jump(u32 entry, u32 info)
{
	log("Jumping. So long and thanks for all the fish!\n");

	// Move and jump!
	__asm__ volatile("movl $" STRINGIFY(MB1_LOAD_MAGIC) ", %%eax\n\t"
							    "jmpl *%%edi\n\t"
			 :
			 : "D"(entry), "b"(info)
			 : "memory");

	panic("Jumper returned\n");
}

// Detect and verify mb1
u8 mb1_detect(struct cfg_entry *cfg)
{
	u8 header[8192] = { 0 };

	s32 ret = cfg->dev->p.disk.fs.read(cfg->path, header, 0, sizeof(header), cfg->dev);
	if (ret < 12)
		return 0;

	// Find start of multiboot entry by searching for magic
	struct mb1_entry *entry = 0;
	for (u32 i = 0; i < sizeof(header); i++) {
		u32 *p = (u32 *)&header[i];
		if (*p == MB1_MAGIC) {
			entry = (void *)p;
			break;
		}
	}

	if (!entry)
		return 0;

	u32 checksum = mb1_checksum(entry);
	if (checksum != entry->checksum)
		return 0;

	cfg->impl.type = IMPL_MB1;
	cfg->impl.start = entry;

	return 1;
}

#include <pic.h>

// Execute mb1 type kernel
void mb1_exec(struct cfg_entry *cfg)
{
	u32 entry = elf_load(cfg->dev, cfg->path);
	mb1_load(cfg->impl.start);

	// This is a kind of hacky parameter stack pushing thing, just disable warning :)
#pragma GCC diagnostic ignored "-Wpedantic"
	jmp_kernel((void *)mb1_jump, 2, entry, MB1_LOAD_ADDRESS);
#pragma GCC diagnostic pop
}
