;===============================================================================
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                      (C) Manu Chaput 2000-2023
;===============================================================================


;===============================================================================
;   Gestion des exceptions
;
;   Certaines exceptions empilent un code d'erreur (32 bits), d'autres
; non. Nous allons les traiter en passant par une fonction commune,
;   écrite en C et nommée "handlerException". C'est elle qui
;   aiguillera ensuite vers la fonction de traitement spéfifique à
;   chaque exception.
; Il est donc nécessaire de définir deux types de handler "bas niveau".
; Un handler associé à une exception avec un code d'erreurs va empiler
;   le numéro de l'exception (qui servira à handlerException pour
;   réaliser l'aiguillage). Un handler associé à une exception sans
;   code d'erreur

;
;===============================================================================
%macro stubHandlerException 1
       push dword 0x0           ; On empile un "code"
       push dword %1            ; On empile le numéro de l'exception
       jmp versHandlerException ; On invoque la fonction d'aiguillage
%endmacro
%macro stubHandlerExceptionCode 1
       push dword %1            ; On empile le numéro de l'exception
       jmp versHandlerException ; On invoque la fonction d'aiguillage
%endmacro

versHandlerException :
       pushad                   ; On sauvegarde les registres
       call handlerException    ; On invoque la fonction d'aiguillage
       popad                    ; On restaure les registres
       add esp, 0x08            ; On "dépile" le code et le numéro
       iret

; Et maintenant on définit tous les handlers d'exception
;-------------------------------------------------------
golbal stubHandlerExceptionDivO
stubHandlerExceptionDivO :                ; Exception "division par zéro"
       stubHandlerException 0x00

global stubHandlerExceptionDebug
stubHandlerExceptionDebug :               ; Exception "debug"
       stubHandlerException 0x01
       
stubHandlerExceptionNMI :                 ; Exception "non maskable interrupt"
       stubHandlerException 0x02
       
stubHandlerExceptionBreakpoint:           ;
       stubHandlerException 0x03
       
stubHandlerExceptionOverflow :            ;
       stubHandlerException 0x04
       
stubHandlerExceptionBoundExceeded :       ;
       stubHandlerException 0x05

stubHandlerExceptionDeviceInvalidOpcode : ; Exception "invalid opcode"
       stubHandlerException 0x06
              
stubHandlerExceptionDeviceUnavailable :   ; Exception "device unavailable"
       stubHandlerException 0x07
              
stubHandlerExceptionDoubleFault :
       stubHandlerExceptionCode 0x08

stubHandlerExceptionCoproOverrun :        ; Exception "Coprocessor Segment Overrun"
       stubHandlerException 0x09

stubHandlerExceptionInvalidTSS :
       stubHandlerExceptionCode 0x0a

stubHandlerExceptionSegmentNotPresent :
       stubHandlerExceptionCode 0x0b
       
stubHandlerExceptionStackSegmentFault :
       stubHandlerExceptionCode 0x0c

stubHandlerExceptionGeneralProtectionFault :
       stubHandlerExceptionCode 0x0d
       
stubHandlerExceptionPageFault :
       stubHandlerExceptionCode 0x0e

stubHandlerExceptionReserved :
       stubHandlerException 0x0f

stubHandlerExceptionFloatingPoint :
       stubHandlerException 0x10

stubHandlerExceptionAlignmentCheck :
       stubHandlerExceptionCode 0x11
       
stubHandlerExceptionFloatingMachineCheck :
       stubHandlerException 0x12

stubHandlerExceptionFloatingSIMDFPE :
       stubHandlerException 0x13

stubHandlerExceptionFloatingVirtualization :
       stubHandlerException 0x14

stubHandlerExceptionControlProtection :
      stubHandlerExceptionCode 0x15

stubHandlerExceptionReserved2 :
       stubHandlerException 0x16

stubHandlerExceptionReserved3 :
       stubHandlerException 0x17

stubHandlerExceptionReserved4 :
       stubHandlerException 0x18

stubHandlerExceptionReserved5 :
       stubHandlerException 0x19

stubHandlerExceptionReserved6 :
       stubHandlerException 0x1a

stubHandlerExceptionReserved7 :
       stubHandlerException 0x1b

stubHandlerExceptionHypervisionInjection :
       stubHandlerException 0x1c

stubHandlerExceptionVMMCommunication :
      stubHandlerExceptionCode 0x1d

stubHandlerExceptionSecurity :
      stubHandlerExceptionCode 0x1e

stubHandlerExceptionReserved8 :
      stubHandlerException 0x1f

;===============================================================================
;   Gestion des IRQ.
;
:   Ces IRQ sont transmises par un PIC. Nous supposerons que ce dernier gère
; un nombre d'IRQ défini dans MANUX_NB_IRQ et que la fonction de gestion
; associée est définie dans MANUX_HANDLER_IRQ.
;
;   On définit (à l'aide de la macro stubHandlerIRQn) un handler pour chacune
; des IRQ. Ce handler empile le numéro de l'IRQ (pas de l'interruption,
; qui dépend du remapping configuré sur le PIC) puis invoque le gestionnaire
; global (la fonction C dont le nom est donné par MANUX_HANDLER_IRQ).
;
;    Ces handlers ont pour nom stubHandlerIRQ1, stubHandlerIRQ2, ...
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

; Génération des MANUX_NB_IRQ handlers
;--------------------------------------
%assign i 0
%rep 16
stubHandlerIRQ%[i] : stubHandlerIRQn i
%assign i i+1
%endrep

;===============================================================================
;   Gestion des appels système
;
;   Le mécanisme des appels système est fondé sur l'utilisation d'une
; interruption logicielle. Son handler est un peu particulier : il va s'occuper
; de sauvegarder l'état du processeur et invoquer la fonction de traitement de
; l'appel système visé.
;   Deux fonctions supplémentaires d'entrée et sortie du noyau sont invoquées,
; qui pourront servir par exemple à de l'audit, à verouiller le noyau, ....
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
