// MIT License, Copyright (c) 2021 Marvin Borner

#include <elf.h>
#include <pnc.h>

u32 elf_load(struct dev *dev, const char *path)
{
	assert(dev->type == DEV_DISK);

	struct elf_header header = { 0 };
	s32 rd = dev->p.disk.fs.read(path, &header, 0, sizeof(header), dev);
	if (rd < 0)
		panic("File couldn't be found\n");
	if (rd != sizeof(header))
		panic("File too small\n");

	// Valid?
	u8 *magic = header.ident;
	u8 valid_magic = magic[ELF_IDENT_MAG0] == ELF_MAG0 && magic[ELF_IDENT_MAG1] == ELF_MAG1 &&
			 magic[ELF_IDENT_MAG2] == ELF_MAG2 && magic[ELF_IDENT_MAG3] == ELF_MAG3 &&
			 magic[ELF_IDENT_CLASS] == ELF_IDENT_CLASS_32 &&
			 magic[ELF_IDENT_DATA] == ELF_IDENT_DATA_LSB;
	if (!valid_magic || (header.type != ELF_ETYPE_REL && header.type != ELF_ETYPE_EXEC) ||
	    header.version != 1 || header.machine != ELF_MACHINE_386)
		panic("File is not valid ELF\n");

	// Loop through programs
	for (u32 i = 0; i < header.phnum; i++) {
		struct elf_program program = { 0 };

		if (dev->p.disk.fs.read(path, &program, header.phoff + header.phentsize * i,
					sizeof(program), dev) != sizeof(program))
			panic("Couldn't read program\n");

		if (program.type == ELF_PROGRAM_TYPE_INTERP)
			panic("Program interpreters aren't supported\n");

		if (program.vaddr == 0 || program.type != ELF_PROGRAM_TYPE_LOAD)
			continue;

		if ((u32)dev->p.disk.fs.read(path, (void *)program.vaddr, program.offset,
					     program.filesz, dev) != program.filesz)
			panic("Couldn't load program\n");
	}

	// Find section string table
	struct elf_section section_strings = { 0 };
	if (dev->p.disk.fs.read(path, &section_strings,
				header.shoff + header.shentsize * header.shstrndx,
				sizeof(section_strings), dev) != sizeof(section_strings))
		panic("Couldn't find string section\n");

	if (section_strings.type != ELF_SECTION_TYPE_STRTAB)
		panic("Invalid string section\n");

	return header.entry;
}
