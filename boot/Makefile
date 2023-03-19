.SUFFIXES: .nasm .bin .obj .c .elf

#KERN_SRC = noyau.$(ASM_EXT)

include ../make.conf
include ../make.commons
include ../make.rules

# Le fichier décrivant la taille du noyau
sinclude $(wildcard $(TAILLE_CONF))

all : floppyboot grubinit

# L'image disquette nécessite un certain nombre de fichiers
floppyboot : $(TAILLE_CONF)
	(make floppyboot2)

floppyboot2 : bourrage $(BSEC_BIN) $(INIT_BIN) 

# L'image sur disquette nécessite éventuellement du bourrage
bourrage :
	dd if=/dev/zero of=bourrage bs=1 count=$(MANUX_TAILLE_BOURRAGE)

# Calcul de la taille du noyau et du bourrage nécessaire
$(TAILLE_CONF) :  $(OUTILS) $(NOYAU_ELF)
	../outils/taillenoyau $(NOYAU_ELF) > $@

grubinit :  $(MB_MANUX_ELF)

clean :
	rm -f bochs.out *.bin *.obj *.o bourrage *.elf *.img $(TAILLE_CONF)
