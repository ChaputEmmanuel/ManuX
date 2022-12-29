.SUFFIXES: .nasm .bin .obj .c

# Les principales valeurs globales
include ../make.commons

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I../include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f bin

include ../make.conf

LIBRAIRIES_MANUX = $(LIBI386) $(LIBMANUX) $(LIBUSR)
LIBRAIRIES_MANUX += $(if  $(MANUX_FS), $(LIBSF))

OBJETS      = 	main.o

OBJETS     +=   $(if $(MANUX_TACHES), listetaches.o tache.o)
OBJETS     +=   $(if $(MANUX_TACHES), scheduler.o)
OBJETS     +=   $(if $(MANUX_GESTION_MEMOIRE), memoire.o)
OBJETS     +=   $(if $(MANUX_PAGINATION), pagination.o)
OBJETS     +=   $(if $(MANUX_OUTILS_SYNCHRO), semaphore.o)
OBJETS     +=   $(if $(MANUX_APPELS_SYSTEME), appelsysteme.o)
OBJETS     +=   $(if $(MANUX_PCI), pci.o)
OBJETS     +=   $(if $(MANUX_VIRTIO), virtio.o)
OBJETS     +=   $(if $(MANUX_VIRTIO_NET), virtio-net.o)
OBJETS     +=   $(if $(MANUX_VIRTIO_CONSOLE), virtio-console.o)

# Une cible pour compiler les binaires, mais sans édition de liens
binaries :  $(OBJETS)

all : binaries noyau.bin noyau.elf symbols_elf symbols_bin

#----------------------------------------------------------------------------------
# Une fois qu'on a tout compilé, il est temps de construire les noyau
#----------------------------------------------------------------------------------
# Création d'un noyau "binaire", utilisé pour le moment pour le boot sur disquette
noyau.bin : $(OBJETS) $(LIBRAIRIES_MANUX)
#	ld -m elf_i386 -N -t mini.lds $(OBJETS) $(LIBRAIRIES_MANUX) -o $@
	ld -m elf_i386 -N --section-start=.text=$(MANUX_KERNEL_START_ADDRESS) --oformat binary  $(OBJETS) $(LIBRAIRIES_MANUX) -o $@

# Création d'un noyau "elf", pour une utilisation de Multiboot
noyau.elf  : $(LD_SCRIPT) $(MB_MANUX_ELF) $(OBJETS) $(LIBRAIRIES_MANUX)
	ld -m elf_i386 -nmagic -T $(LD_SCRIPT) $(MB_MANUX_ELF) $(OBJETS) $(LIBRAIRIES_MANUX) -o $@

# Création des symboles, WARNING : à revoir
symbols_bin : $(OBJETS) $(LIBRAIRIES_MANUX)
	ld -m elf_i386 -N --section-start=.text=$(MANUX_KERNEL_START_ADDRESS) $(OBJETS) $(LIBRAIRIES_MANUX) -o noyau.sym
	nm noyau.sym | sort > symbols_bin
	rm noyau.sym

symbols_elf : noyau.elf
	nm noyau.elf | sort > symbols_elf

#----------------------------------------------------------------------------------

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -DKERNEL_START_ADDRESS=$(KERNEL_START_ADDRESS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.elf *.a symbols_bin symbols_elf
