.SUFFIXES: .nasm .bin .obj .c

ASM_EXT  = nasm

ASM         = nasm
ASM_BIN_OPT = -f elf

OBJETS      = interruptions.o segment.o interBasNiveau.bin irq.bin gestionGDT.bin

include ../make.conf

ASM_DEF	     = -D MANUX_KERNEL_TASK_TSS_IND=$(MANUX_KERNEL_TASK_TSS_IND)
ASM_DEF     += -D MANUX_DATA_SEG_SEL=$(MANUX_DATA_SEG_SEL)
ASM_DEF     += -D MANUX_CODE_SEG_SEL=$(MANUX_CODE_SEG_SEL)
ASM_DEF     += $(if $(MANUX_APPELS_SYSTEME), -D MANUX_APPELS_SYSTEME)
ASM_DEF     += $(if $(MANUX_CLAVIER), -D MANUX_CLAVIER)

libi386.a : $(OBJETS)
	ar rf libi386.a	$(OBJETS)

.$(ASM_EXT).bin :
	$(ASM) $(ASM_DEF)  $(ASM_BIN_OPT) $< -o $@

.c.o :
	gcc $(CFLAGS) -c $<


clean :
	rm -f bochs.out *.bin *.obj *.o *.a

