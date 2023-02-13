.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I./include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

BSEC_SRC = bootsector.$(ASM_EXT)
KERN_SRC = noyau.$(ASM_EXT)

BSEC_BIN = bootsector.bin
INIT_BIN = init-manux.bin

ASM         = nasm

ASM_DEF     = -D MANUX_INIT_START_ADDRESS=$(MANUX_INIT_START_ADDRESS) \
              -D MANUX_KERNEL_START_ADDRESS=$(MANUX_KERNEL_START_ADDRESS) \
              -D MANUX_STACK_SEG_16=$(MANUX_STACK_SEG_16) \
              -D MANUX_ELF_HEADER_SIZE=$(MANUX_ELF_HEADER_SIZE) \
              -D MANUX_ADRESSE_ECRAN=$(MANUX_ADRESSE_ECRAN) \
              -D MANUX_NB_SECT_INIT=$(MANUX_NB_SECT_INIT) \
              -D MANUX_NB_SECT_KERNEL=$(MANUX_NB_SECT_KERNEL) \
              -D MANUX_NB_SECT_RAMDISK=$(MANUX_NB_SECT_RAMDISK) \
              -D MANUX_SEGMENT_TRANSIT_RAMDISK=$(MANUX_SEGMENT_TRANSIT_RAMDISK) \
              -D MANUX_portDonneesClavier=$(MANUX_portDonneesClavier) \
              -D MANUX_portCmdClavier=$(MANUX_portCmdClavier) \
              -D MANUX_PREMIER_SECT_RAMDISK=$(MANUX_PREMIER_SECT_RAMDISK)

ASM_BIN_OPT = -f bin
ASM_ELF_OPT = -f elf32

AS= gcc

include ../make.conf
include ../make.commons

MB_MANUX_ELF = mb-manux.elf

# Le fichier décrivant la taille du noyau
sinclude $(wildcard taille.conf)

all : floppyboot grubinit

# L'image disquette nécessite un certain nombre de fichiers
floppyboot : $(BSEC_BIN) $(INIT_BIN) $(CRT0_ELF) $(INIT_ELF) 


# L'image sur disquette nécessite éventuellement du bourrage
bourrage : $(NOYAU_BIN) $(TAILLE_CONF) 
	dd if=$(NOYAU_BIN) of=bourrage bs=1 count=$(MANUX_TAILLE_BOURAGE)

$(TAILLE_CONF) : $(NOYAU_BIN) $(OUTILS)
	../outils/taillenoyau $(NOYAU_BIN) > $@

grubinit :  $(INIT_ELF) $(MB_MANUX_ELF)

.$(ASM_EXT).bin :
	$(ASM) $(ASM_DEF) $(ASM_BIN_OPT) $< -o $@

.$(ASM_EXT).elf :
	$(ASM) $(ASM_DEF) $(ASM_ELF_OPT) $< -o $@

mb-manux.elf : mb-manux.$(ASM_EXT)
	$(ASM) $(ASM_DEF) $(ASM_ELF_OPT) $ mb-manux.$(ASM_EXT) -o mb-manux.elf

.c.o :
	gcc $(CFLAGS) -c $<

clean :
	rm -f bochs.out *.bin *.obj *.o bourrage *.elf
