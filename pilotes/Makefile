.SUFFIXES: .nasm .bin .obj .c

include ../make.commons
include ../make.conf
include ../make.rules

ASM_BIN_OPT = -f bin

OBJETS      = 
OBJETS     += $(if $(MANUX_SUPPORT_BLOC), support-bloc.o)
OBJETS     += $(if $(MANUX_SUPPORT_BLOC_RAM), support-bloc-ram.o)

libpilotes.a : $(MANUX_FICHIER_CONFIG) $(OBJETS)
	ar rf libpilotes.a $(OBJETS)

clean :
	rm -f bochs.out *.bin *.obj *.o *.a

