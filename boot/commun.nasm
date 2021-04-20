;-------------------------------------------------------------------------------
;      Les variables importantes de l'initialisation de ManuX
;
;      WARNING : à mettre dans make.conf ?
;-------------------------------------------------------------------------------
MANUX_INIT_SEG_16       equ 1000h
MANUX_KERNEL_SEG_16     equ 2000h
MANUX_STACK_SEG_16      equ 9000h
ELF_HEADER_SIZE         equ 80h
ADRESSE_ECRAN           equ 0b8000h

NB_SECT_INIT            equ 02h

NB_SECTEURS_RAMDISK     equ 2h     ; WARNING à automatiser, et < 14h !!
SEGMENT_TRANSIT_RAMDISK equ 4000h

PREMIER_SECT_RAMDISK    equ 0x10

;-------------------------------------------------------------------------------
;      Les ports
;-------------------------------------------------------------------------------
portCmdClavier        equ 064h
portDonneesClavier    equ 060h
