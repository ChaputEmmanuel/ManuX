;-------------------------------------------------------------------------------
;      Définition des fonctions de bas niveau permettant la gestion des
;   interruptions.
;      Sauf indication contraire, les paramètres (s'il y en a) sont passés "à la
;   C", c'est à dire empilés en ordre inverse et dépilés par l'appelant.
;
;                                                           (C) Manu Chaput 2000
;-------------------------------------------------------------------------------
bits 32

global interdireIRQ
global autoriserIRQ

interdireIRQ :

; Interdiction d'une IRQ
;-----------------------
;   Paramètre
;      Numéro de l'IQR (1 byte)

        push eax
        push ecx

        mov cl, [esp+0ch]           ; cl <- no IRQ
        mov ax, 01h                 ; On positionne à 1 le bit de ax
        shl ax, cl                  ; correspondant au num d'IRQ.
        cmp cl, 07h                 ; Les IRQ 8 à 15 sont gérées
        jg iPremierCircuit          ; par le premier 8259

        mov cl, al                  ; On positionne dans cl le bit de l'IRQ (0-7)
        in al, Port8259_1  + 1      ; On lit le masque actuel
        or al, cl                   ; et on y ajoute notre IRQ.
        out Port8259_1  + 1, al     ; On positionne ce nouveau masque.
        jmp finInterdireIRQ

iPremierCircuit :
        mov cl, ah                  ; On positionne dans cl le bit de l'IRQ (8-15)
        in al, Port8259_2  + 1      ; On lit le masque actuel
        or al, cl                   ; et on y ajoute notre IRQ.
        out Port8259_2  + 1, al     ; On positionne ce nouveau masque.

finInterdireIRQ:
        pop ecx
        pop eax
        ret

autoriserIRQ :

; Autorisation d'une IRQ
;-----------------------
;   Paramètre
;      Numéro de l'IQR (1 byte)

        push eax
        push ecx

        mov cl, [esp+00ch]          ; cl <- no IRQ
        mov ax, 01h                 ; On positionne à 1 le bit de ax
        shl ax, cl                  ; correspondant au num d'IRQ.
        cmp cl, 07h                 ; Les IRQ 8 à 15 sont gérées
        jg aPremierCircuit          ; par le premier 8259

        mov cl, al                  ; On positionne dans cl le bit de l'IRQ (0-7)
        in al, Port8259_1  + 1      ; On lit le masque actuel
        not cl
        and al, cl                  ; et on y supprime notre IRQ.
        out Port8259_1  + 1, al     ; On positionne ce nouveau masque.
        jmp finAutoriserIRQ

aPremierCircuit :
        mov cl, ah                  ; On positionne dans cl le bit de l'IRQ (8-15)
        in al, Port8259_2  + 1      ; On lit le masque actuel
        not cl
        and al, cl                  ; et on y supprime notre IRQ.
        out Port8259_2  + 1, al     ; On positionne ce nouveau masque.

finAutoriserIRQ :
        pop ecx
        pop eax
        ret

; Variables WARNING à mettre ailleurs
;----------
Port8259_1  equ 020h
Port8259_2  equ 0A0h
