.SUFFIXES: .nasm .bin .obj .c

#
# -fno-stack-protector permet d'éviter les "undefined references to `__stack_chk_fail'" sur les GCC récents
#
CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I../usr/include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder -fno-stack-protector

ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f bin

OBJETS      = stdio.o unistd.o init.o

libusr.a : $(OBJETS) 
	ar rf libusr.a $(OBJETS)

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.a

