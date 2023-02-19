;-------------------------------------------------------------------------------
; Quelques macros spécifiques à multiboot
;-------------------------------------------------------------------------------
MULTIBOOT_PAGE_ALIGN  equ 0x00000001 ; Alignement des modules sur des pages
MULTIBOOT_MEMORY_INFO equ 0x00000002 ; Pour que multiboot nous donne le plan

;-------------------------------------------------------------------------------
; Définition de notre entête
;-------------------------------------------------------------------------------
FLAGS    equ  MULTIBOOT_MEMORY_INFO
MAGIC    equ  0x1BADB002        
CHECKSUM equ -(MAGIC + FLAGS)   

;-------------------------------------------------------------------------------
; L'entête à proprement parler
;-------------------------------------------------------------------------------
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
     

