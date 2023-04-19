;===============================================================================
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                      (C) Manu Chaput 2000-2023
;===============================================================================
[bits 32]

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

extern gestionException    ; La fonction générale de gestion

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
       pushad                   ; sauvegarde des registres
       call gestionException    ; appel de la fonction d'aiguillage
       popad                    ; restauration des registres
       add esp, 0x08            ; on "dépile" le code et le numéro
       iret

; Et maintenant on définit tous les handlers d'exception
;-------------------------------------------------------
global stubHandlerExceptionDiv0
stubHandlerExceptionDiv0 :                ; Exception "division par zéro"
       stubHandlerException 0x00

global stubHandlerExceptionDebug
stubHandlerExceptionDebug :               ; Exception "debug"
       stubHandlerException 0x01
       
global stubHandlerExceptionNMI
stubHandlerExceptionNMI :                 ; Exception "non maskable interrupt"
       stubHandlerException 0x02
       
global stubHandlerExceptionBreakpoint
stubHandlerExceptionBreakpoint:           ;
       stubHandlerException 0x03
       
global stubHandlerExceptionOverflow
stubHandlerExceptionOverflow :            ; Exception "Overflow"
       stubHandlerException 0x04

global stubHandlerExceptionBoundExceeded
stubHandlerExceptionBoundExceeded :       ;
       stubHandlerException 0x05

global stubHandlerExceptionDeviceInvalidOpcode
stubHandlerExceptionDeviceInvalidOpcode : ; Exception "invalid opcode"
       stubHandlerException 0x06
              
global stubHandlerExceptionDeviceUnavailable
stubHandlerExceptionDeviceUnavailable :   ; Exception "device unavailable"
       stubHandlerException 0x07

global stubHandlerExceptionDoubleFault
stubHandlerExceptionDoubleFault :
       stubHandlerExceptionCode 0x08

global stubHandlerExceptionCoproOverrun
stubHandlerExceptionCoproOverrun :        ; Exception "Coprocessor Segment Overrun"
       stubHandlerException 0x09

global stubHandlerExceptionInvalidTSS
stubHandlerExceptionInvalidTSS :
       stubHandlerExceptionCode 0x0a

global stubHandlerExceptionSegmentNotPresent
stubHandlerExceptionSegmentNotPresent :
       stubHandlerExceptionCode 0x0b

global stubHandlerExceptionStackSegmentFault
stubHandlerExceptionStackSegmentFault :
       stubHandlerExceptionCode 0x0c

global stubHandlerExceptionGeneralProtectionFault
stubHandlerExceptionGeneralProtectionFault :
       stubHandlerExceptionCode 0x0d

global stubHandlerExceptionPageFault
stubHandlerExceptionPageFault :
       stubHandlerExceptionCode 0x0e

global stubHandlerExceptionReserved
stubHandlerExceptionReserved :
       stubHandlerException 0x0f

global stubHandlerExceptionFloatingPoint
stubHandlerExceptionFloatingPoint :
       stubHandlerException 0x10

global stubHandlerExceptionAlignmentCheck
stubHandlerExceptionAlignmentCheck :
       stubHandlerExceptionCode 0x11

global stubHandlerExceptionFloatingMachineCheck
stubHandlerExceptionFloatingMachineCheck :
       stubHandlerException 0x12

global stubHandlerExceptionFloatingSIMDFPE
stubHandlerExceptionFloatingSIMDFPE :
       stubHandlerException 0x13

global stubHandlerExceptionFloatingVirtualization
stubHandlerExceptionFloatingVirtualization :
       stubHandlerException 0x14

global stubHandlerExceptionControlProtection
stubHandlerExceptionControlProtection :
       stubHandlerExceptionCode 0x15

global stubHandlerExceptionReserved2
stubHandlerExceptionReserved2 :
       stubHandlerException 0x16

global stubHandlerExceptionReserved3
stubHandlerExceptionReserved3 :
       stubHandlerException 0x17

global stubHandlerExceptionReserved4
stubHandlerExceptionReserved4 :
       stubHandlerException 0x18

global stubHandlerExceptionReserved5
stubHandlerExceptionReserved5 :
       stubHandlerException 0x19

global stubHandlerExceptionReserved6
stubHandlerExceptionReserved6 :
       stubHandlerException 0x1a

global stubHandlerExceptionReserved7
stubHandlerExceptionReserved7 :
       stubHandlerException 0x1b

global stubHandlerExceptionHypervisionInjection
stubHandlerExceptionHypervisionInjection :
       stubHandlerException 0x1c

global stubHandlerExceptionVMMCommunication
stubHandlerExceptionVMMCommunication :
      stubHandlerExceptionCode 0x1d

global stubHandlerExceptionSecurity
stubHandlerExceptionSecurity :
      stubHandlerExceptionCode 0x1e

global stubHandlerExceptionReserved8
stubHandlerExceptionReserved8 :
      stubHandlerException 0x1f

;===============================================================================
;   Gestion des IRQ.
;
;   Ces IRQ sont transmises par un PIC. Nous supposerons que ce dernier gère
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

; Génération des MANUX_NB_IRQ handlers (ce nombre est défini par le PIC)
;-----------------------------------------------------------------------
%assign i 0
%rep MANUX_NB_IRQ
global stubHandlerIRQ%[i]
stubHandlerIRQ%[i] : stubHandlerIRQn i
%assign i i+1
%endrep

;===============================================================================
;   Gestion des interruptions logicielles
;
;===============================================================================
extern  neRienFaire  ;  gestionInterruption ;

%macro stubHandlerInt 1
        push dword %1               ; On empile le numéro de l'interruption
	jmp  handlerInt
%endmacro

handlerInt :
	pushad
	
        call neRienFaire ;  gestionInterruption ; 

	popad
        add esp, 4                  ; Dépile le numéro d'interruption
        iret

; On génère les gestionnaires bas niveau
;---------------------------------------

%assign i MANUX_INT_BASE_IRQ + MANUX_NB_IRQ
%rep MANUX_NB_INTERRUPTIONS - (MANUX_INT_BASE_IRQ + MANUX_NB_IRQ)
global stubHandlerInt%[i] 
stubHandlerInt%[i] : stubHandlerInt i
%assign i i+1
%endrep

; La fonction suivante initialise un tableau avec les addresses des
; gestionnaires bas niveau. Son interface C est la suivante
;
; void initialiserHandlersInterruption(void * hbn, uint32_t n);
;
; Elle place dans le tableau hbn les adresses des handlers bas niveau que l'on
; vient de définir (au maximum n fonctions)
;------------------------------------------------------------------------------
%macro ajoutHandlerInt 1
        mov dword [eax], stubHandlerInt%[i]
	add eax, 4
        inc ebx
	cmp ebx, [esp + 16]
	ja IHIFin
%endmacro

global initialiserHandlersInterruption
initialiserHandlersInterruption :
        push eax
        push ebx
	mov eax, [esp + 12]
	mov ebx, 1

%assign i MANUX_INT_BASE_IRQ + MANUX_NB_IRQ 
%rep MANUX_NB_INTERRUPTIONS - (MANUX_INT_BASE_IRQ + MANUX_NB_IRQ)
        ajoutHandlerInt i
%assign i i+1
%endrep

IHIFin  pop ebx
        pop eax
        ret
	
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

global handlerAppelSysteme

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
%endif   ; MANUX_APPELS_SYSTEME
