;-------------------------------------------------------------------------------
;      Initialisation de ManuX
;                                                           (C) Manu Chaput 2000
;-------------------------------------------------------------------------------
org  MANUX_INIT_START_ADDRESS
global InitManuX

InitManuX :
        mov ax, 0 ; MANUX_INIT_START_ADDRESS   ; C'est à cette adresse qu'est chargé l'init
        mov ds, ax
        mov es, ax
        mov si, MsgChargement       ; AfficheBIOS le message de chargement
        call AfficheBIOS

        ; Détection du matériel
        ;----------------------
        mov si, MsgDetection
        call AfficheBIOS

        ; Lecture de la mémoire conventionnelle
        int 12h
        mov [MemoireDeBase], ax

        ; Lecture de la taille mémoire étendue ([3] p 984)
        mov al, 17h               ;  Demande lecture de la taille
        out 70h, al               ; mémoire étendue (poids faible).
        in al, 71h                ;  Lecture et 
        mov bl, al                ; stockage dans bl
        mov al,18h                ;  Demande lecture de la taille
        out 70h,al                ; mémoire étendue (poids fort).
        in al,71h                 ;  Lecture et
        mov bh, al                ; stockage dans bh
        mov [MemoireEtendue], bx  ; @ 0x10028

        ; Vérifions que nous sommes bien en mode réel
        ;--------------------------------------------
        mov eax, cr0      ; L'info est dans cr0
        and al, 1         ; On vérifie le bit de poids faible
        jz ModeReelOK     ; Il doit être nul ...

        mov si, MsgPasEnModeReel
        call AfficheBIOS

        mov ah, 9
        int 21h
        mov ax, 4cffh
        int 21h
        jmp BlocageDebug

ModeReelOK :
        mov si, MsgModeReel
        call AfficheBIOS        ; @ 0x10048

        ; Activation de la ligne A20
        ;---------------------------
        mov si, MsgValideA20
        call AfficheBIOS

        cli
        call ValideA20
        sti

%ifdef MANUX_RAMDISK
        call ChargerRamdisk
%endif

        ; Passage en mode protégé
        ;------------------------
        mov si, MsgPassageProtege
        call AfficheBIOS

        ; Chargement de la GDT
        ;---------------------
	cli

        lgdt [LaGDT]                ; Chargement de la GDT

        ; Changement effectif de mode
        ;----------------------------
        mov eax, cr0     ; On positionne
        or eax, 01       ; à un le bit pmode
        mov cr0, eax     ; du registre cr0


        jmp IndSegCode32:Mode32       ; Pour vider le pipe

%ifdef MANUX_RAMDISK
%include "gestion-ramdisk.nasm"
%endif

[bits 32]

Mode32:

        ; Initialisation des segments
        ;----------------------------
	mov ax, IndSegData32    ; @ 0x7e71
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        ; La pile ...
        mov bx, IndSegStack32
        mov ss, bx
        mov eax, 90000h - 4
        mov esp, eax

	sti
;        mov si, MsgProut
;        call AfficheBIOS   ; Aucune raison que ça marche maintenant !

%ifdef MANUX_RAMDISK
        call DeplaceRamDisk
%endif

        ; Calcul de l'adresse des infos Système
        ;--------------------------------------
;        mov eax, 0                   ; C'est à cette adresse que
;        mov ax, 0x10000 ; MANUX_INIT_START_ADDRESS   ; sont actuellement les infos
;        shl eax, 4                   ; Il faut transformer
;        add eax, InfoSysteme         ; en adresse "flat"
;        push eax   ; Non, on ne passe rien en paramètre en fait

        ; Annonce du saut ...
        ;--------------------
;        mov si, MsgRunningManux
;        call AfficheBIOS

        ; Et c'est parti, on saute sur le noyau !
        ;----------------------------------------
        mov eax, MANUX_KERNEL_START_ADDRESS 
        ;shl eax, 4
        call eax

[bits 16]

;-------------------------------------------------------------------------------
;      Les sous-programmes
;-------------------------------------------------------------------------------

;      Validation de la ligne A20
;--------------------------------
ValideA20 :
        call AttenteClavierPret
        jnz ValideA20Fin          ; Si pas pret, on arrète
        mov al, 0d1h              ; On veut écrire dans le port de sortie
        out MANUX_portCmdClavier, al

        call AttenteClavierPret
        jnz ValideA20Fin           ; Si pas pret, on arrète
        mov al, 0dfh               ; Activation ligne A20
        out MANUX_portDonneesClavier, al ;

ValideA20Fin :
        ret

;      Attente que le clavier soit pret avant de lui envoyer des données
;-----------------------------------------------------------------------
AttenteClavierPret :
        mov ecx, 0ffffffffh
AttenteClavierBcle :
        in al, MANUX_portCmdClavier     ;  On attend que la mémoire du contrôleur
        test al, 2                ; ne soit pas pleine.
        loopnz AttenteClavierBcle ;
        ret

;      Affichage par le BIOS de la chaine pointée par SI
;-------------------------------------------------------
AfficheBIOS:
        lodsb
        or al,al
        jz short FinAfficheBIOS
        mov ah,0x0E
        mov bx,0x0007
        int 0x10
        jmp AfficheBIOS
FinAfficheBIOS:
        retn

;      Blocage de la machine pour débugger
;-----------------------------------------
BlocageDebug :
        mov si, MsgBlocage
        call AfficheBIOS

BoucleFolle :
        jmp BoucleFolle

;-------------------------------------------------------------------------------
;      Les données
;-------------------------------------------------------------------------------
[bits 32]
; Les infos sur le système pour passer au noyau
;----------------------------------------------
InfoSysteme :
   PourFlagsMB :        ; Attention 
        dd 0h           ;
   MemoireDeBase :      ; seuls les deux premiers sont
        dd 0h           ; compatibles avec multiboot
   MemoireEtendue :     ; mais la suite (ramdisk) ne l'est
        dd 0h           ; absolument pas

%ifdef MANUX_RAMDISK
   TailleRamdisk :
        dd 0
   AdresseRamdisk :
        dd 0
%endif

[bits 16]

%include "gdt.nasm"

; Les messages
;-------------
MsgChargement     db      'ManuX loading ...',13,10,0
MsgDetection      db      'La, je devrais detecter le matos !', 13, 10, 0
MsgBlocage        db      '(Blocage de debug)', 13, 10, 0
MsgPassageProtege db      'Allez zou, on passe en mode protege ...', 13, 10, 0
MsgModeReel       db      'OK, on est en mode reel !', 13, 10, 0
MsgPasEnModeReel  db      'ERREUR, on n est pas en mode reel ...', 13, 10, 0
MsgValideA20      db      'Validation de la ligne A20 ...', 13, 10, 0
%ifdef MANUX_RAMDISK
MsgLoadRamDisk    db      'Chargement du RamDisk ...', 13, 10, 0
MsgNoRamDisk      db      'Pas de RamDisk ...', 13, 10, 0
MsgErreurRamDisk  db      'Erreur de chargement RamDisk ...', 13, 10, 0
%endif
MsgRunningManux   db      'On lance ManuX, ...', 13, 10, 0
MsgProut          db      'Surprise suprise mother fucker !', 13, 10, 0

; Le bourrage (pour faire 2 blocs)
;---------------------------------
        times   1024-($-$$) db 0

