;===============================================================================
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                      (C) Manu Chaput 2000-2021
;===============================================================================
bits 32

extern handlerPanique
extern gestionGeneraleInterruption

;--------------------------------------------------------------------------------
;   Exportation des fonctions définies dans ce fichier
;--------------------------------------------------------------------------------
%assign i 0
%rep 16
global stubHandlerIRQ%[i]
%assign i i+1
%endrep

%assign i 0
%rep 32
global stubHandlerPanique_%[i]
%assign i i+1
%endrep

global handlerAppelSysteme          ; WARNING, à mettre ailleurs
global halt                         ; WARNING, à mettre ailleurs

;===============================================================================
;   Gestion des IRQ.
;
;   On définit (à l'aide de la macro stubHandlerIRQn) un handler pour chacune
; des IRS (16 max). Ce handler empile le numéro de l'IRQ (pas de l'interruption,
; qui dépend du remapping configuré sur le PIC) puis invoque le gestionnaire
; global (une fonction C répondant au doux nom de handlerIRQ).
;===============================================================================

extern MANUX_HANDLER_IRQ            ; La fonction de gestion, liée au PIC

; Un handler pour l'IRQ n
;------------------------
%macro stubHandlerIRQn 1
        push dword %1               ; On empile le numéro de l'IRQ
	jmp  handlerIRQ
%endmacro

handlerIRQ :
	pusha
	
        call MANUX_HANDLER_IRQ

	popa
        add esp, 4                  ; Dépile le numéro d'IRQ

        iret

; Génération des 16 handlers
%assign i 0
%rep 16
stubHandlerIRQ%[i] : stubHandlerIRQn i
%assign i i+1
%endrep

;===============================================================================
;   Gestion des appels système
;===============================================================================
%ifdef MANUX_APPELS_SYSTEME
extern vecteurAppelsSysteme
extern entrerAppelSysteme
extern sortirAppelSysteme

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
        call entrerAppelSysteme
	
        ; On autorise les IT WARNING, est-ce bien raisonable ?
        sti
	
        ; On invoque l'AS
	pop eax
        call [vecteurAppelsSysteme+eax] ; Le numéro est dans EAX (cf appelsysteme.h)

        ; On déverouille le noyau WARNING : non réentrance
        call sortirAppelSysteme

        pop ecx
        pop edx
        pop ebx
        pop esp
        pop ebp
        pop esi
        pop edi
        iret
%endif

; Un handler qui ne fait rien ...
;--------------------------------
global stubHandlerNop 
stubHandlerNop :
        iret                        ; On revient ...

; Un handler qui va afficher un message (WARNING à virer dès que la suite est validée)
;--------------------------------------
%macro   stubHandlerPanique 1

         pusha                ; Je pense qu'on peut s'en passer, ...
	 push dword %1        ; On push le numéro en 32 bits

         call handlerPanique
	 
         add esp, 4           ; On pop le numéro
	 popa

         iret
%endmacro

; Un handler général pour toutes les interruptions
;-------------------------------------------------
%macro   baseGestionGeneraleInterruption 1

         pusha                ; Je pense qu'on peut s'en passer, ...
	 push dword %1        ; On push le numéro en 32 bits

         call gestionGeneraleInterruption
	 
         add esp, 4           ; On pop le numéro
	 popa

         iret
%endmacro


; Génération de tous les handlers de panique
%assign i 0
%rep 32
stubHandlerPanique_%[i]: stubHandlerPanique i
%assign i i+1
%endrep

; Arret du système
;-----------------
halt :
        hlt
        jmp halt

