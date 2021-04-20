.SUFFIXES: .nasm .bin .obj .c

#CFLAGS   = -fno-pie -m32 -march=i686 -Wall -I../include -O2 -nostdinc -fno-builtin -fomit-frame-pointer -fno-toplevel-reorder
ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f elf

OBJETS      = interruptions.o segment.o interBasNiveau.bin #irq.bin

include ../make.conf

libi386.a : $(OBJETS) irq.bin
	ar rf libi386.a	$(OBJETS)

.$(ASM_EXT).bin :
	$(ASM) -D MANUX_KERNEL_TASK_TSS_IND=$(MANUX_KERNEL_TASK_TSS_IND) $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.a

