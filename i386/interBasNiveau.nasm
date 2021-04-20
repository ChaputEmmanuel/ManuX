;-------------------------------------------------------------------------------
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                      (C) Manu Chaput 2000-2002
;-------------------------------------------------------------------------------
bits 32

extern handlerTimer
extern handlerWarning
extern handlerClavier
extern vecteurAppelsSysteme         ; WARNING, inutile si on vire les AS de là
extern entrerAppelSysteme
extern sortirAppelSysteme
extern autoriserIRQ

global stubHandlerTimer
global stubHandlerClavier
global stubHandlerNop
global stubHandlerWarning
global handlerAppelSysteme          ; WARNING, à mettre ailleurs

global halt                         ; WARNING, à mettre ailleurs

; Handler de l'interruption du Timer
;-----------------------------------
stubHandlerTimer :
        pusha                       ; On sauvegarde les registres

        mov al, 20h                 ; On envoie un EOI
        out Port8259_1, al          ; au 8259 ([3] p 1120)

        call handlerTimer             ; On apelle le handler C

        popa                        ; On restaure les registres
        iret                        ; On revient ...

; Handler de l'interruption du clavier
;-------------------------------------
stubHandlerClavier :
        pusha                       ; On sauvegarde les registres

        call handlerClavier         ; On apelle le handler C

        mov al, 20h                 ; On envoie un EOI
        out Port8259_2, al          ; au 8259 ([3] p 1120)
        out Port8259_1, al          ; WARNING : a virer !

        popa                        ; On restaure les registres
        iret                        ; On revient ...

; Le handler des appels systèmes
;-------------------------------
handlerAppelSysteme :
        push edi
        push esi
        push ebp
        push esp
        push ebx
        push edx
        push ecx
        shl eax, 02h
        push eax
	
        ; On verouille le noyau WARNING : non réentrance
;        call entrerAppelSysteme
        ; On autorise le timer WARNING, est-ce bien raisonable ?
        sti
        ; On invoque l'AS
	pop eax
        call [vecteurAppelsSysteme+eax] ; Le numéro est dans EAX (cf appelsysteme.h)
        ; On déverouille le noyau WARNING : non réentrance
;        call sortirAppelSysteme

        pop ecx
        pop edx
        pop ebx
        pop esp
        pop ebp
        pop esi
        pop edi
        iret

; Un handler qui ne fait rien ...
;--------------------------------
stubHandlerNop :
        iret                        ; On revient ...

; Un handler qui ne fait (presque) rien ...
;------------------------------------------
stubHandlerWarning :
        pusha                       ; On sauvegarde les registres

        call handlerWarning         ; On apelle le handler C

        popa                        ; On restaure les registres
        iret                        ; On revient ...

; Lancer une tache (WARNING, ça n'a rien à faire là mais je ne sais pas faire)
;-----------------
;lanceTacheInitiale :
;        call MANUX_KERNEL_TASK_TSS_IND:0h
;        jmp lanceTacheInitiale ; WARNING ???

; Arret du système
;-----------------
halt :
        hlt
        jmp halt

; Variables WARNING à mettre ailleurs
;----------
Port8259_1  equ 020h
Port8259_2  equ 0A0h

