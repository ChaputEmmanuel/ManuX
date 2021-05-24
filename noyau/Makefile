.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I../include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f bin

LIBI386   = ../i386/libi386.a
LIBMANUX  = ../lib/libmanux.a
LIBSF     = ../sf/libsf.a
LIBUSR    = ../usr/libusr.a

OBJETS      = 	main.o scheduler.o memoire.o listetaches.o tache.o \
		semaphore.o appelsysteme.o pagination.o

include ../make.conf

all : noyau.obj symbols

noyau.obj : $(OBJETS) $(LIBI386) $(LIBMANUX) $(LIBSF) $(LIBUSR)
	ld -m elf_i386 -N --section-start=.text=$(KERNEL_START_ADDRESS) --oformat binary  $(OBJETS) $(KERN_O) $(LIBMANUX) $(LIBI386) $(LIBSF) $(LIBUSR) ../lib/clavier.o ../i386/irq.bin -o $@

symbols : $(OBJETS) $(LIBI386) $(LIBMANUX)
	ld -m elf_i386 -N --section-start=.text=$(KERNEL_START_ADDRESS) $(OBJETS) $(KERN_O) $(LIBMANUX) $(LIBI386) $(LIBSF)  $(LIBUSR) ../lib/clavier.o ../i386/irq.bin -o noyau.elf
	nm noyau.elf | sort > symbols

.$(ASM_EXT).bin :
	$(ASM) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -DKERNEL_START_ADDRESS=$(KERNEL_START_ADDRESS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.elf *.a symbols

