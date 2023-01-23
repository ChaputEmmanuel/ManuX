.SUFFIXES: .nasm .bin .obj .c

# Les principales valeurs globales
ROOTDIR := $(shell pwd)
include make.commons

BSEC_SRC = bootsector.$(ASM_EXT)
BSEC_BIN = boot/bootsector.bin
INIT_BIN = boot/init-manux.bin
BOOT_SRC =  boot/bootsector.nasm boot/init-manux.nasm

DEMARAGE    = outils boot
BOURRAGE    = ./boot/bourrage ./boot/ramdisk.ram
OUTILS      = ./outils/taillenoyau ./outils/makeconfig

# Les includes de usr/include/manux qui sont des copies de include/manux
# ils sont édités dans l'arborescence du noyau et doivent donc être
# mis à jour dans la partie usr
USR_INC_D   = usr/include/manux
USR_INC_F   = appelsystemenum.h config.h types.h string.h i386.h
USR_INC     = $(USR_INC_F:%.h=$(USR_INC_D)/%.h)

# Le ficher de configuration est généré par make en fonction du
# contenu de include/manux/config.h
-include make.conf

# Quels sont les composants d'un noyau fonctionnel (hors processus de boot)
MANUX_PARTS  = i386 lib usr noyau  

MANUX_PARTS += $(if $(MANUX_FS), sf)

# Les sous-répertoires (pour le nettoyage par exemple)
SOUS_REP  = $(MANUX_PARTS) $(DEMARAGE) 

export CFLAGS ROOTDIR

#    Les cibles voulues, ce sera probablement des images finales
all : manux bootgrub
	(cd noyau; make all)

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

#...............................................................................
#    Le fichier de configuration (une des premières choses à faire !)
#...............................................................................
make.conf : $(MANUX_FICHIER_CONFIG)
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

# L'image bootgrub nécessite un code d'initialisation spécifique
grubinit :
	(cd boot ; make grubinit)

$(OUTILS) : 
	(cd outils ; make)

bootage : 
	(cd boot ; make floppyboot)

bourrage: 
	(cd boot ; make bourrage)

#-------------------------------------------------------------------------------
#    Dernière phase : les images utilisables directement
#-------------------------------------------------------------------------------
#...............................................................................
# L'image sur disquette est obtenue en concaténant les fichiers de boot, d'init,
# et le noyau
#...............................................................................
manux : configuration composants $(NOYAU_BIN) bootfloppy 
	cat $(BSEC_BIN) $(INIT_BIN) $(NOYAU_BIN) $(BOURRAGE) > manux

#...............................................................................
#  L'image bootgrub est en fait un noyau linké avec le code pour
# la compatibilité bootgrub
#...............................................................................
bootgrub : configuration  grubinit  composants 
	(cd noyau ; make noyau.elf)

# Une image ISO
iso : bootgrub
	$(CREER_ISO) $(ISO_REP_BASE) $(ISO_FICHIER) $(NOYAU_ELF)

$(ISO_FICHIER) : iso

#...............................................................................
#    Création d'une image iso intégrant un noyau pour chaque fichier de config
# du répertoire multiconf
#...............................................................................
multiso :
	(rm -rf noyaux/* $(ISO_REP_BASE)/* | true)
	(for c in $(ROOTDIR)/multiconf/* ; do (echo "\033[0;34m*****" ; echo "*****  Construction de $$c *****" ;echo "*****\033[0m" ;  make clean ; make MANUX_FICHIER_CONFIG="$$c" bootgrub ; cp noyau/noyau.elf noyaux/`basename $$c .h` ) ; done )
	$(CREER_ISO) $(ISO_REP_BASE) $(ISO_FICHIER) noyaux/*

#-------------------------------------------------------------------------------
#    Lancement du noyau
#-------------------------------------------------------------------------------
run : bootgrub
	$(RUN_MANUX_ELF)

oldrun : manux
	$(RUN_MANUX_FLOPPY)

runiso : #$(ISO_FICHIER)
	$(RUN_MANUX_ISO) $(ISO_FICHIER)

runmultiso : multiso
	$(RUN_MANUX_ISO) $(ISO_FICHIER)

vbrun : iso
	$(RUN_MANUX_VBOX)

#-------------------------------------------------------------------------------
#    Quelques cibles complémentaires en vrac
#-------------------------------------------------------------------------------
dump :
	ndisasm $(NOYAU_BIN) -u > dump

clean :
	(for r in $(SOUS_REP) doc ; do (cd $$r ; make clean) ; done)
	rm -f bochs.out *.bin manux *.obj *.o dump $(TAILLE_CONF) *~ __bfe.log__ $(ISO_FICHIER) dump.dat make.conf

distclean :
	(for r in $(SOUS_REP) ; do (cd $$r ; make clean) ; done)
	rm -f bochs.out *.bin *.obj *.o dump $(TAILLE_CONF) *~

tar : clean
	(cd .. ; tar cvf -  ManuX-32 | $(COMPRESS) > manux-32-`date +"%Y-%m-%d"`.tgz ; echo Archive dans manux-32-`date +"%Y-%m-%d"`.tgz )

