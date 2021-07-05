# MIT License, Copyright (c) 2021 Marvin Borner

# Obviously needs cross compiler
CC = $(PWD)/cross/opt/bin/i686-elf-gcc
LD = $(PWD)/cross/opt/bin/i686-elf-ld
OC = $(PWD)/cross/opt/bin/i686-elf-objcopy
ST = $(PWD)/cross/opt/bin/i686-elf-strip
AS = nasm

BLD = $(PWD)/build
SRC = $(PWD)/src

SRCS = $(shell find $(SRC)/loader/ -type f -name "*.c")
OBJS = $(patsubst $(SRC)/%.c,$(BLD)/%.o,$(SRCS))

# Enable many warnings for less bugs :)
WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wwrite-strings -Wredundant-decls -Wnested-externs -Wformat=2 -Wmissing-declarations -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code -Wundef -Wold-style-definition -Wvla -pedantic-errors

# Disable some GCC features and boilerplate generation
CFLAGS = $(WARNINGS) -std=c99 -m32 -nostdlib -nostdinc -ffunction-sections -fno-builtin -fno-profile-generate -fno-omit-frame-pointer -fno-common -fno-asynchronous-unwind-tables -fno-stack-protector -fno-pie -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Ofast -ffreestanding -Wl,-estart -I$(SRC)/loader/inc/

ASFLAGS = -f elf32

all: dir $(BLD)/boot.bin

dir:
	@mkdir -p $(BLD)/entry/
	@mkdir -p $(BLD)/loader/fs/

$(BLD)/boot.bin: $(BLD)/loader.bin
	@$(AS) -f bin $(SRC)/entry/bootsector.asm -o $@

$(BLD)/loader.o: $(OBJS)
	@$(LD) -N -z max-page-size=0x1000 -estart -T$(SRC)/loader/link.ld -o $@ $^

$(BLD)/loader.bin: $(BLD)/loader.o
	@$(OC) -O binary $^ $@

$(OBJS): $(BLD)/%.o : $(SRC)/%.c
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@rm -rf $(BLD)/*
