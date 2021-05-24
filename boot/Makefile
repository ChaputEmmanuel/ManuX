.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I./include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

BSEC_SRC = bootsector.$(ASM_EXT)
KERN_SRC = noyau.$(ASM_EXT)

BSEC_BIN = bootsector.bin
INIT_BIN = init-manux.bin

ASM         = nasm

ASM_DEF     = -D INIT_START_ADDRESS=$(INIT_START_ADDRESS) \
              -D KERNEL_START_ADDRESS=$(KERNEL_START_ADDRESS) \
              -D MANUX_STACK_SEG_16=$(MANUX_STACK_SEG_16) \
              -D ELF_HEADER_SIZE=$(ELF_HEADER_SIZE) \
              -D ADRESSE_ECRAN=$(ADRESSE_ECRAN) \
              -D NB_SECT_INIT=$(NB_SECT_INIT) \
              -D NB_SECT_KERNEL=$(NB_SECT_KERNEL) \
              -D NB_SECT_RAMDISK=$(NB_SECT_RAMDISK) \
              -D SEGMENT_TRANSIT_RAMDISK=$(SEGMENT_TRANSIT_RAMDISK) \
              -D portDonneesClavier=$(portDonneesClavier) \
              -D portCmdClavier=$(portCmdClavier) \
              -D PREMIER_SECT_RAMDISK=$(PREMIER_SECT_RAMDISK)

ASM_BIN_OPT = -f bin

AS= gcc

include ../make.conf
ifeq (../taille.conf,$(wildcard ../taille.conf)) 
include ../taille.conf
endif

.$(ASM_EXT).bin :
	$(ASM) $(ASM_DEF) $(ASM_BIN_OPT) $< -o $@

all : $(BSEC_BIN) $(INIT_BIN) $(CRT0_ELF) $(INIT_ELF) bourage

bourage :
	dd if=../noyau/noyau.obj of=bourage bs=1 count=$(MANUX_TAILLE_BOURAGE)

.c.o :
	gcc $(CFLAGS) -c $<

clean :
	rm -f bochs.out *.bin *.obj *.o bourage

