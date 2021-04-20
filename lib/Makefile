.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I../include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f bin

OBJETS      = console.o ramdisk.o printk.o journal.o malloc.o string.o#clavier.o

libmanux.a : $(OBJETS) clavier.o
	ar rf libmanux.a $(OBJETS)

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.a

