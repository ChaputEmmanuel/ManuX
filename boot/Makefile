.SUFFIXES: .nasm .bin .obj .c .elf

#KERN_SRC = noyau.$(ASM_EXT)


include ../make.conf
include ../make.commons
include ../make.rules

# Le fichier décrivant la taille du noyau
sinclude $(wildcard taille.conf)

all : floppyboot grubinit

# L'image disquette nécessite un certain nombre de fichiers
floppyboot : $(BSEC_BIN) $(INIT_BIN)

# L'image sur disquette nécessite éventuellement du bourrage
bourrage : $(TAILLE_CONF) 
	dd if=/dev/zero of=bourrage bs=1 count=$(MANUX_TAILLE_BOURRAGE)

# Calcul de la taille du noyau et du bourrage nécessaire
$(TAILLE_CONF) : $(NOYAU_BIN) $(OUTILS)
	../outils/taillenoyau $(NOYAU_BIN) > $@

grubinit :  $(MB_MANUX_ELF)

clean :
	rm -f bochs.out *.bin *.obj *.o bourrage *.elf *.img
