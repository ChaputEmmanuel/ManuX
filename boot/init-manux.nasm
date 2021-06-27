;-------------------------------------------------------------------------------
;      Initialisation de ManuX
;                                                           (C) Manu Chaput 2000
;-------------------------------------------------------------------------------
global InitManuX

InitManuX :
        mov ax, MANUX_INIT_START_ADDRESS   ; C'est à cette adresse qu'est chargé l'init
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
        mov [MemoireEtendue], bx

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
        call AfficheBIOS

        ; Activation de la ligne A20
        ;---------------------------
        mov si, MsgValideA20
        call AfficheBIOS

        cli
        call ValideA20
        sti

%ifdef MANUX_RAMDISK
        ; Chargement du RamDisk, si nécessaire
        ;-------------------------------------
        mov ax, MANUX_NB_SECT_RAMDISK
        cmp ax, 0h
        je PasDeRamdisk

        mov si, MsgLoadRamDisk
        call AfficheBIOS

        mov ax, MANUX_NB_SECT_RAMDISK
        shr ax, 1
        mov [TailleRamdisk], ax
        xor ebx, ebx
        mov bx, [MemoireEtendue]    ; OK, bx = taille de la mémoire étendue
        add bx, 0400h               ; on lui ajoute le Méga de base ...
        mov ax, MANUX_NB_SECT_RAMDISK     ; ... et on lui enlève la taille du ramdisk
        shr ax, 1                   ; 1 secteur = 1/2 Ko
        sub bx, ax
        shl ebx, 10                 ; 1 Ko = 2^10 Octets ...
        mov [AdresseRamdisk], ebx    ; On stoque l'adresse de début du RamDisk

        ; Chargement effectif
        mov ax, MANUX_SEGMENT_TRANSIT_RAMDISK ; Adresse de destination ...
        mov es,ax                       ; ... dans es:bx
        mov bx, 0

        mov ah, 2                 ; Lecture = fonction 2
        mov al, MANUX_NB_SECT_RAMDISK
        mov cx, MANUX_NB_SECT_INIT + MANUX_NB_SECT_KERNEL + 2
        mov dx, 0                 ; head=0, drive=0
        int 13h                   ; On place ça en ES:BX

        jc ErreurChargeRamDisk

        jmp SuiteRamdisk

ErreurChargeRamDisk :
        mov si, MsgErreurRamDisk
        call AfficheBIOS

PasDeRamdisk :
        mov si, MsgNoRamDisk
        call AfficheBIOS

SuiteRamdisk:
%endif

        ; Passage en mode protégé
        ;------------------------
        mov si, MsgPassageProtege
        call AfficheBIOS

        ; Chargement de la GDT
        ;---------------------
        mov     eax, 0              ; C'est à cette adresse qu'est
        mov     ax, ds              ; actuellement la GDT
        shl     eax, 4              ; Il faut transformer
        add     eax, SegCodeNul     ; en adresse "flat"
        mov     [AdresseGDT],eax    ; et enregistrer dans la GDT

        lgdt [LaGDT]                ; Chargement de la GDT

        cli

        ; Changement effectif de mode
        ;----------------------------
        mov eax, cr0     ; On positionne
        or eax, 01       ; à un le bit pmode
        mov cr0, eax     ; du registre cr0

        jmp Vidage       ; Pour vider le pipe
Vidage : 
        db 066h, 0eah
        dw VidageTer, 01h
        dw 08h

[bits 32]

VidageTer:

        ; Initialisation des segments
        ;----------------------------
        mov ax, IndSegData32
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        ; La pile ...
        mov ebx, IndSegStack32
        mov ss, bx
        mov eax, 90000h - 4
        mov esp, eax

%ifdef MANUX_RAMDISK

        ; On déplace ensuite le RamDisk vers le haut de la mémoire

        mov eax, MANUX_SEGMENT_TRANSIT_RAMDISK ; Calcul de l'adresse "flat" ...
        shl eax, 0x4                     ; ... du transit du ramdisk.
        mov esi, eax

        mov eax, MANUX_INIT_START_ADDRESS   ; Calcul de l'adresse "flat" ...
        shl eax, 0x4                  ; ... de la variable ...
        add eax, AdresseRamdisk       ; ... AdresseRamdisk .
        mov edi, [eax]

        mov eax, MANUX_INIT_START_ADDRESS   ; Calcul de l'adresse "flat" ...
        shl eax, 0x4                  ; ... de la variable ...
        add eax, TailleRamdisk        ; ... TailleRamdisk .

        mov ecx, [eax]
        shl ecx, 10

        cld
        rep movsb                     ; [ES:ESI]->[DS:EDI] ECX fois
%endif

        ; Calcul de l'adresse des infos Système
        ;--------------------------------------
        mov eax, 0                   ; C'est à cette adresse que
        mov ax, MANUX_INIT_START_ADDRESS   ; sont actuellement les infos
        shl eax, 4                   ; Il faut transformer
        add eax, InfoSysteme         ; en adresse "flat"
        push eax

        ; Annonce du saut ...
        ;--------------------
        mov si, MsgRunningManux
        call AfficheBIOS

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

; La GDT ( [1] p 3-11, [2] )
;-------
LaGDT :
        dw TailleGDT  ; Taille de notre GDT
   AdresseGDT :
        dd LaGDT      ; Son Adresse
   SegCodeNul :        ; Un descripteur de segment nul obligatoire
        dw 0
        dw 0
        db 0
        db 0
        db 0
        db 0

   SegCode32 :         ; Descripteur pour le segment de code 32 bits
        dw 0ffffh        ; Segment limit 15:00  (4 GB)
        dw 00000h        ; Segment base address 15:00
        db 00            ; Segment base address 23:16
        db 09ah          ; P=1 (present) DPL=0 (privilège) S=1 Type=Code Read/Exec
        db 0cfh          ; Segment limit 19:16 AVL=0 D/B=1 G=1 (4GB)
        db 0             ; Segment base address 31:24

   SegData32 :         ; Descripteur pour le segment de données 32 bits
        dw 0ffffh        ; Segment limit 15:00  (4 GB)
        dw 00000h        ; Segment base address 15:00
        db 00            ; Segment base address 23:16
        db 092h          ; P=1 (present) DPL=0 (privilège) S=1 Type=Data r/w
        db 0cfh          ; Segment limit 19:16 AVL=0 D/B=1 G=1 (4GB)
        db 0             ; Segment base address 31:24

   SegStack32 :         ; Descripteur pour le segment de pile 32 bits
        dw 0ffffh        ; Segment limit 15:00  (4 GB)
        dw 00000h        ; Segment base address 15:00
        db 00            ; Segment base address 23:16
        db 092h          ; P=1 (present) DPL=0 (privilège) S=1 Type=Data r/w
        db 0cfh          ; Segment limit 19:16 AVL=0 D/B=1 G=1 (4GB)
        db 0             ; Segment base address 31:24

   SegTask :            ; Descripteur pour la première tache (WARNING à virer)
        dw 0ffffh        ; Segment limit 15:00  (4 GB)
        dw 00000h        ; Segment base address 15:00
        db 00            ; Segment base address 23:16
        db 089h          ; P=1 (present) DPL=0 (privilège) S=1 Type=Data r/w
        db 08fh          ; Segment limit 19:16 AVL=0 G=1 (4GB)
        db 0             ; Segment base address 31:24

        TailleGDT equ $-SegCodeNul-1
; Fin de la GDT

; Calcul des indices des différents descripteurs
IndSegCode32  equ SegCode32 - SegCodeNul
IndSegData32  equ SegData32 - SegCodeNul
IndSegStack32 equ SegStack32 - SegCodeNul
IndSegTask32  equ SegTask - SegCodeNul

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

; Le bourrage (pour faire 2 blocs)
;---------------------------------
        times   1024-($-$$) db 0

