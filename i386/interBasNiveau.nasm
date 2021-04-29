;-------------------------------------------------------------------------------
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                      (C) Manu Chaput 2000-2021
;-------------------------------------------------------------------------------
bits 32

extern handlerTimer
extern handlerPanique
extern handlerClavier
extern vecteurAppelsSysteme         ; WARNING, inutile si on vire les AS de là
extern entrerAppelSysteme
extern sortirAppelSysteme
extern autoriserIRQ

global stubHandlerTimer
global stubHandlerClavier
global stubHandlerNop
global stubHandlerPanique_0
global stubHandlerPanique_1
global stubHandlerPanique_2
global stubHandlerPanique_3
global stubHandlerPanique_4
global stubHandlerPanique_5
global stubHandlerPanique_6
global stubHandlerPanique_7
global stubHandlerPanique_8
global stubHandlerPanique_9
global stubHandlerPanique_10
global stubHandlerPanique_11
global stubHandlerPanique_12
global stubHandlerPanique_13
global stubHandlerPanique_14
global stubHandlerPanique_15
global stubHandlerPanique_16
global stubHandlerPanique_17
global stubHandlerPanique_18
global stubHandlerPanique_19
global stubHandlerPanique_20
global stubHandlerPanique_21
global stubHandlerPanique_22
global stubHandlerPanique_23
global stubHandlerPanique_24
global stubHandlerPanique_25
global stubHandlerPanique_26
global stubHandlerPanique_27
global stubHandlerPanique_28
global stubHandlerPanique_29
global stubHandlerPanique_30
global stubHandlerPanique_31

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

; Un handler qui va afficher un message
;--------------------------------------
%macro   stubHandlerPanique 1

         pusha                ; Je pense qu'on peut s'en passer, ...
	 push dword %1

         call handlerPanique
	 
         add esp, 4
	 popa

         iret
%endmacro

stubHandlerPanique_0: stubHandlerPanique 0
stubHandlerPanique_1 : stubHandlerPanique 1
stubHandlerPanique_2 : stubHandlerPanique 2
stubHandlerPanique_3 : stubHandlerPanique 3
stubHandlerPanique_4 : stubHandlerPanique 4
stubHandlerPanique_5 : stubHandlerPanique 5
stubHandlerPanique_6 : stubHandlerPanique 6
stubHandlerPanique_7 : stubHandlerPanique 7
stubHandlerPanique_8 : stubHandlerPanique 8
stubHandlerPanique_9 : stubHandlerPanique 9
stubHandlerPanique_10 : stubHandlerPanique 10
stubHandlerPanique_11 : stubHandlerPanique 11
stubHandlerPanique_12 : stubHandlerPanique 12
stubHandlerPanique_13 : stubHandlerPanique 13
stubHandlerPanique_14 : stubHandlerPanique 14
stubHandlerPanique_15 : stubHandlerPanique 15
stubHandlerPanique_16 : stubHandlerPanique 16
stubHandlerPanique_17 : stubHandlerPanique 17
stubHandlerPanique_18 : stubHandlerPanique 18
stubHandlerPanique_19 : stubHandlerPanique 19
stubHandlerPanique_20 : stubHandlerPanique 20
stubHandlerPanique_21 : stubHandlerPanique 21
stubHandlerPanique_22 : stubHandlerPanique 22
stubHandlerPanique_23 : stubHandlerPanique 23
stubHandlerPanique_24 : stubHandlerPanique 24
stubHandlerPanique_25 : stubHandlerPanique 25
stubHandlerPanique_26 : stubHandlerPanique 26
stubHandlerPanique_27 : stubHandlerPanique 27
stubHandlerPanique_28 : stubHandlerPanique 28
stubHandlerPanique_29 : stubHandlerPanique 29
stubHandlerPanique_30 : stubHandlerPanique 30
stubHandlerPanique_31 : stubHandlerPanique 31

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

