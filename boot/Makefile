.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I./include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

BSEC_SRC = bootsector.$(ASM_EXT)
KERN_SRC = noyau.$(ASM_EXT)

BSEC_BIN = bootsector.bin
INIT_BIN = init-manux.bin

ASM         = nasm

ASM_BIN_OPT = -f bin -D MANUX_KERNEL_SECT=$(MANUX_KERNEL_SECT)

AS= gcc

include ../make.conf
ifeq (../taille.conf,$(wildcard ../taille.conf)) 
include ../taille.conf
endif

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

all : $(BSEC_BIN) $(INIT_BIN) $(CRT0_ELF) $(INIT_ELF) bourage

bourage :
	dd if=../noyau/noyau.obj of=bourage bs=1 count=$(MANUX_TAILLE_BOURAGE)

.c.o :
	gcc $(CFLAGS) -c $<

clean :
	rm -f bochs.out *.bin *.obj *.o bourage

