# MIT License, Copyright (c) 2021 Marvin Borner
# It's not as complicated as it looks

# Obviously needs cross compiler
CC = $(PWD)/cross/opt/bin/i686-elf-gcc
LD = $(PWD)/cross/opt/bin/i686-elf-ld
OC = $(PWD)/cross/opt/bin/i686-elf-objcopy
ST = $(PWD)/cross/opt/bin/i686-elf-strip
AS = nasm

BLD = $(PWD)/build
SRC = $(PWD)/src

# C code
CSRCS = $(shell find $(SRC)/loader/ -type f -name "*.c")
COBJS = $(patsubst $(SRC)/%.c,$(BLD)/%_c.o,$(CSRCS))

# Assembly code
ASRCS = $(shell find $(SRC)/loader/ -type f -name "*.asm")
AOBJS = $(patsubst $(SRC)/%.asm,$(BLD)/%_asm.o,$(ASRCS))

# Enable many warnings for less bugs :)
WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wredundant-decls -Wnested-externs -Wformat=2 -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wundef -Wold-style-definition -Wvla -pedantic-errors

# Disable some GCC features and boilerplate generation
CFLAGS = $(WARNINGS) -std=c99 -m32 -nostdlib -nostdinc -ffunction-sections -fno-builtin -fno-profile-generate -fno-omit-frame-pointer -fno-common -fno-asynchronous-unwind-tables -fno-stack-protector -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Ofast -ffreestanding -Wl,-e_start -I$(SRC)/loader/inc/

ASFLAGS = -f elf32

export

all: dir $(BLD)/boot.bin mb1 mb2

dir:
	@mkdir -p $(BLD)/entry/
	@mkdir -p $(BLD)/loader/fs/
	@mkdir -p $(BLD)/loader/protocols/

mb1:
	@$(MAKE) --no-print-directory -C example/$@

mb2:
	@$(MAKE) --no-print-directory -C example/$@

$(BLD)/boot.bin: $(BLD)/loader.bin
	@$(AS) $(ASFLAGS) -f bin $(SRC)/entry/bootsector.asm -o $@

$(BLD)/loader.o: $(COBJS) $(AOBJS)
	@$(LD) -N -z max-page-size=0x1000 -e_start -T$(SRC)/loader/link.ld -o $@ $^

$(BLD)/loader.bin: $(BLD)/loader.o
	@$(OC) -O binary $^ $@

$(COBJS): $(BLD)/%_c.o : $(SRC)/%.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(AOBJS): $(BLD)/%_asm.o : $(SRC)/%.asm
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf $(BLD)/*
