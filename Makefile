.SUFFIXES: .nasm .bin .obj .c

ROOTDIR := $(shell pwd)

# Les principales valeurs globales
include make.commons

#
# -fno-stack-protector permet d'éviter les "undefined references to `__stack_chk_fail'" sur les GCC récents
#
# Optimisation
#   -O2 par défaut
#   -O0 pour debuguer
CFLAGS   = -g -gdwarf -fno-pie -m32 -march=i686 -Wall  \
	   -I$(ROOTDIR)/include  \
            -nostdinc -fno-builtin -nodefaultlibs -nostartfiles \
           -fno-toplevel-reorder \
           -fno-stack-protector \
           -O0 \
         # -fomit-frame-pointer

RUN_MANUX= qemu-system-i386 -drive format=raw,file=manux,index=0,if=floppy -m 64M

BSEC_SRC = bootsector.$(ASM_EXT)
#KERN_SRC = noyau.$(ASM_EXT)

BSEC_BIN = boot/bootsector.bin
INIT_BIN = boot/init-manux.bin

BOOT_SRC =  boot/bootsector.nasm boot/init-manux.nasm
DEMARAGE  = outils boot
BOURRAGE    = bourage ./boot/ramdisk.ram
OUTILS      = ./outils/taillenoyau ./outils/makeconfig

# Les includes de usr/include/manux qui sont des copies de include/manux
# ils sont édités dans l'arborescence du noyau et doivent donc être
# mis à jour dans la partie usr
USR_INC_D   = usr/include/manux
USR_INC_F   = appelsystemenum.h config.h types.h string.h i386.h
USR_INC     = $(USR_INC_F:%.h=$(USR_INC_D)/%.h)

# Le ficher de configuration est généré par make en fonction de include/manux/config.h
include make.conf

# Quels sont les composants d'un noyau fonctionnel (hors processus de boot)
MANUX_PARTS  = i386 lib usr noyau  

MANUX_PARTS += $(if  $(MANUX_FS), sf)

# Les sous-répertoires (pour le nettoyage par exemple)
SOUS_REP  = $(MANUX_PARTS) $(DEMARAGE) 

export CFLAGS ROOTDIR

ASM         = nasm
ASM_BIN_OPT = -f bin

#    Les cibles voulues, ce sera probablement des images finales
all : manux multiboot

#    Lancement du noyau
run : manux
	$(RUN_MANUX)

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -c $<

#-------------------------------------------------------------------------------
#    Première phase : la configuration générale
#-------------------------------------------------------------------------------
configuration : usrinc make.conf
#    Mise à jour des include du monde utilisateur
usr/include/manux/%.h : include/manux/%.h
	cp $< $@

usrinc : $(USR_INC)

#    Le fichier de configuration (une des premières choses à faire !)
make.conf : include/manux/config.h
	cpp -nostdinc -fno-builtin  -dM $<  | awk '/^#define MANUX_/ {if (length($$3)){val=$$3}else{val="True"};print $$2"="val}' > make.conf

#-------------------------------------------------------------------------------
#    Deuxième phase : les composants de ManuX
#-------------------------------------------------------------------------------
composants :
	(for r in $(MANUX_PARTS) ; do (cd $$r ; make ) ; done)

$(LIBI386) :
	(cd i386 ; make)

$(LIBSF) :
	(cd sf ; make)

$(LIBMANUX) :
	(cd lib ; make)

$(NOYAU_BIN) :
	(cd noyau ; make noyau.bin)

#-------------------------------------------------------------------------------
#    Troisième phase : le code d'initialisation (dépend de l'image)
#-------------------------------------------------------------------------------
bootfloppy : $(OUTILS) bourrage bootage

# L'image multiboot nécessite un code d'initialisation spécifique
multinit :
	(cd boot ; make multinit)

$(OUTILS) : 
	(cd outils ; make)

bootage : 
	(cd boot ; make floppyboot)

bourrage: 
	(cd boot ; make bourrage)

#-------------------------------------------------------------------------------
#    Dernière phase : les images utilisables directement
#-------------------------------------------------------------------------------
# L'image sur disquette est obtenue en concaténant les fichiers de boot, d'init,
# et le noyau
manux : configuration composants $(NOYAU_BIN) bootfloppy 
	cat $(BSEC_BIN) $(INIT_BIN) $(NOYAU_BIN) $(BOURRAGE) > manux

# L'image multiboot est en fait un noyau linké avec le code pour la compatibilité multiboot
multiboot : configuration  multinit  composants 
	(cd noyau ; make noyau.elf)

#-------------------------------------------------------------------------------
#    Quelques cibles complémentaires en vrac
#-------------------------------------------------------------------------------
dump :
	ndisasm $(NOYAU_BIN) -u > dump

clean :
	rm -f bochs.out *.bin manux *.obj *.o dump $(TAILLE_CONF) *~ __bfe.log__
	(for r in $(SOUS_REP) ; do (cd $$r ; make clean) ; done)

distclean :
	rm -f bochs.out *.bin *.obj *.o dump $(TAILLE_CONF) *~
	(for r in $(SOUS_REP) ; do (cd $$r ; make clean) ; done)

tar : clean
	(cd .. ; tar cvf -  ManuX-32 | $(COMPRESS) > manux-32-`date +"%Y-%m-%d"`.tgz ; echo Archive dans manux-32-`date +"%Y-%m-%d"`.tgz )

