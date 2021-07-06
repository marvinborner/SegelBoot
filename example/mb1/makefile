# MIT License, Copyright (c) 2021 Marvin Borner
# Gets called by main makefile (their variables are exported)

OBJS = entry_asm.o main.o

all: compile

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

%_asm.o: %.asm
	@$(AS) $(ASFLAGS) $< -o $@

compile: $(OBJS)
	@mkdir -p $(BLD)/examples/
	@$(LD) -N -z max-page-size=0x1000 -eboot_entry -Tlink.ld -o $(BLD)/examples/mb1.elf $+
