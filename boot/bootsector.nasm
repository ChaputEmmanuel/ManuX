;-------------------------------------------------------------------------------
;      Secteur de boot de ManuX ...
;
;      Ce code doit tenir dans un secteur (512 octets). Il se contente donc de
;   charger en mémoire le code de init puis le code du noyau et de faire un
;   saut à l'adrese de init.
;                                                           (C) Manu Chaput 2000
;-------------------------------------------------------------------------------

        org 0x7C00                 ; Le secteur de boot est chargé à cette adresse

        cli                        ; Pas d'interruption SVP
        mov ax, MANUX_STACK_SEG_16 ; Initialisation du segment de pile
        mov ss, ax
        mov sp, 0
        sti                        ; OK, on accepte à nouveau


        ; Chargement de l'init depuis le disque 
        ;--------------------------------------
ChargeInit :
        mov ax, MANUX_INIT_START_ADDRESS ; Adresse de destination
        mov es,ax                 ; es:bx
        mov bx, 0

        mov ah, 2                 ; Lecture = fonction 2
        mov al, MANUX_NB_SECT_INIT      ; taille d'init
        mov cx, 2                 ; à partir du secteur 2
        mov dx, 0                 ; head=0, drive=0
        int 13h                   ; On place ça en ES:BX

        jc InitDisquette

        ; Chargement du noyau depuis le disque. Attention, l'adresse est
	; probablement exprimée sur plus de 16 bits, d'où le décalage à
	; droite. On pert les 4 bits de poids faible. On pourrait les
	; récpérer dans bx, mais est-ce bien utile ? On va partir du fait
	; qu'ils sont nuls.
        ;-------------------------------------
        mov ax, MANUX_KERNEL_START_ADDRESS>>4 ; Adresse de destination 
        mov es,ax                       ; es:bx 
        mov bx, 0                       ; cf 3 lignes plus haut

        mov ah, 2                   ; On veut lire
        mov al, MANUX_NB_SECT_KERNEL      ; x secteurs (taille du noyau)
        mov cx, 4                   ; à partir du secteur 4
        mov dx, 0                   ; head=0, drive=0
        int 13h                     ; On place ça en ES:BX

        jc InitDisquette

        ; Arret du lecteur de disquette WARNING violent et pas beau
        ;------------------------------
        mov dx, 03f2h
        in al, dx                    ; On lit l'état du contrôleur
        and al, 0fh                  ; on y met à 0 le "bit moteur"
        out dx, al                   ; et on lui renvoie ...

        ; On saute à l'adresse de l'init
        ;-------------------------------
        mov ax, MANUX_INIT_START_ADDRESS
        mov es,ax
        mov ds,ax
        push ax
        mov ax,0x0000
        push ax
        retf

	jmp BoucleFolle            ; Inutile a priori !

        ; Réinitialisation du lecteur de disquette (en cas de pb)
        ;--------------------------------------------------------
InitDisquette :
        mov dl, 0                  ; Disquette A: = 0
        mov ax, 0                  ; Initialisation = fonction 0
        int 13h
        jnc ChargeInit             ; Si erreur carry = 0

BoucleFolle:
        jmp BoucleFolle

times 512-($-$$)-2 db 0
        dw 0AA55h
