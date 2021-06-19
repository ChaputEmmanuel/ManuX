; Declare constants for the multiboot header.
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

extern adresseDebutManuX
extern adresseFinManuX

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
     
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 
section .text
global start:function (start.end - start)
start:
	mov esp, stack_top

	; On passe les adresses de debut et fin (definies dans le
	; script du linker)
	push adresseFinManuX
	push adresseDebutManuX

        ; on passe le pointeur (contenu dans ebx) sur les infos
        push ebx

	extern _start
	call _start

	cli
.hang:	hlt
	jmp .hang
.end:
