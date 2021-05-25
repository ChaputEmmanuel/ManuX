.SUFFIXES: .nasm .bin .obj .c

ROOTDIR := $(shell pwd)

COMPRESS = gzip

#
# -fno-stack-protector permet d'éviter les "undefined references to `__stack_chk_fail'" sur les GCC récents
#
# Optimisation
#   -O2 par défaut
#   -O0 pour debuguer
CFLAGS   = -g -gdwarf -fno-pie -m32 -march=i686 -Wall -I$(ROOTDIR)/include -O0 -nostdinc -fno-builtin -fno-toplevel-reorder \
           -fno-stack-protector \
         # -fomit-frame-pointer

RUN_MANUX= qemu-system-i386 -drive format=raw,file=manux,index=0,if=floppy -m 64M

ASM_EXT  = nasm

BSEC_SRC = bootsector.$(ASM_EXT)
KERN_SRC = noyau.$(ASM_EXT)

BSEC_BIN = boot/bootsector.bin
INIT_BIN = boot/init-manux.bin
KERN_BIN = noyau.bin
KERN_OBJ = noyau-c.obj
KERN_O   = noyau-c.o

BOOT_SRC =  boot/bootsector.nasm boot/init-manux.nasm
DEMARAGE  = outils boot
MANUX_PARTS  = i386 lib sf usr noyau 
SOUS_REP  = $(MANUX_PARTS) $(DEMARAGE) 
LIBI386   = ./i386/libi386.a
LIBMANUX  = ./lib/libmanux.a
LIBSF     = ./sf/libsf.a
LIBUSR    = ./usr/libusr.a
NOYAU     = ./noyau/noyau.obj
BOURAGE	  = ./boot/bourage ./boot/ramdisk.ram
TAILLE_CONF = taille.conf
OUTILS      = ./outils/taillenoyau ./outils/makeconfig

# Les includes de usr/include/manux qui sont des copies de include/manux
# ils sont édités dans l'arborescence du noyau et doivent donc être
# mis à jour dans la partie usr
USR_INC_D   = usr/include/manux
USR_INC_F   = appelsystemenum.h config.h types.h string.h i386.h
USR_INC     = $(USR_INC_F:%.h=$(USR_INC_D)/%.h)

include make.conf
export CFLAGS ROOTDIR

ASM         = nasm
ASM_BIN_OPT = -f bin

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

manux : usrinc composants boot $(BOURAGE)
	cat $(BSEC_BIN) $(INIT_BIN) $(NOYAU) $(BOURAGE) > manux

run : manux
	$(RUN_MANUX)

.c.o :
	gcc $(CFLAGS) -c $<

$(LIBI386) :
	(cd i386 ; make)

$(LIBSF) :
	(cd sf ; make)

$(LIBMANUX) :
	(cd lib ; make)

usr/include/manux/%.h : include/manux/%.h
	cp $< $@

usrinc : $(USR_INC)

make.conf : include/manux/config.h outils/makeconfig
	./outils/makeconfig > make.conf

$(NOYAU) :
	(cd noyau ; make)

$(TAILLE_CONF) : $(NOYAU) $(OUTILS)
	./outils/taillenoyau $(NOYAU) > taille.conf

boot : $(TAILLE_CONF) $(BOOT_SRC)
	(cd boot ; make)

$(BOURAGE) : 
	(cd boot ; make bourage)

$(OUTILS) : 
	(cd outils ; make)

composants :
	(for r in $(MANUX_PARTS) ; do (cd $$r ; make ) ; done)

dump :
	ndisasm $(NOYAU) -u > dump

clean :
	rm -f bochs.out *.bin manux *.obj *.o dump $(TAILLE_CONF) *~ __bfe.log__
	(for r in $(SOUS_REP) ; do (cd $$r ; make clean) ; done)

distclean :
	rm -f bochs.out *.bin *.obj *.o dump $(TAILLE_CONF) *~
	(for r in $(SOUS_REP) ; do (cd $$r ; make clean) ; done)

tar : clean
	(cd .. ; tar cvf -  ManuX-32 | $(COMPRESS) > manux-32-`date +"%Y-%m-%d"`.tgz ; echo Archive dans manux-32-`date +"%Y-%m-%d"`.tgz )

