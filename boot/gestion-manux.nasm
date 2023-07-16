;===============================================================================
; Sous-programme de chargement de ManuX en mémoire.
;                                                      (C) Manu Chaput 2000-2023
;===============================================================================
chargerManuX :

        ; Chargement du noyau depuis le disque. On va la convertir
        ; sous la forme es:bx 
        ;---------------------------------------------------------
        mov ax, MANUX_KERNEL_START_ADDRESS>>4      ; Adresse de destination 
        mov es,ax                                  ; es:bx
        mov bx, MANUX_KERNEL_START_ADDRESS & 0x0F  ; cf 3 lignes plus haut

        mov word ax, MANUX_NB_SECT_KERNEL    ; x secteurs (taille du noyau)
;        mov ah, 2                       ; On veut lire
        mov cx, 4                       ; à partir du secteur 4
        mov dx, 0                       ; head=0, drive=0

        call chargerPleinSecteurs
	
chargerManuXFin :
	ret

;-------------------------------------------------------------------------------
;    Utilisation du BIOS pour charger des secteurs en mémoire depuis le lecteur
; de disquettes.
;    On va utiliser l'interruption 13h (avec ah=02h : read sectors from drive),
; les paramètres sont
;    . al    : nombre de secteurs
;    . ch    : numéro de cylindre
;    . cl    : numéro de secteur
;    . dh    : tête
;    . dl    : lecteur
;    . es:bx : adresse destination
;-------------------------------------------------------------------------------
chargerSecteurs :
        int 13h                    ; C'est tout !
;        jc reInitDisquette         ; En cas de soucis
        ret

        ; Réinitialisation du lecteur de disquette (en cas de pb)
        ;--------------------------------------------------------
reInitDisquette :
        mov dl, 0                  ; Disquette A: = 0
        mov ax, 0                  ; Initialisation = fonction 0
        int 13h
        jnc chargerSecteurs        ; Si erreur carry = 0
                      ; WARNING il vaudrait mieux un message d'erreur +halt

;-------------------------------------------------------------------------------
;
;-------------------------------------------------------------------------------
arretDisquette :
        ; Arret du lecteur de disquette WARNING violent et pas beau
        ;----------------------------------------------------------
        mov dx, 03f2h
        in al, dx                    ; On lit l'état du contrôleur
        and al, 0fh                  ; on y met à 0 le "bit moteur"
        out dx, al                   ; et on lui renvoie ...

        ret

;-------------------------------------------------------------------------------
; Chargement de plus de 256 secteurs.
;
; On va utiliser chargerSecteurs dont on reprend sensiblement les paramètres
;    . ax    : nombre de secteurs    <− C'est la seule différence (ax vs al)
;    . ch    : numéro de cylindre
;    . cl    : numéro de secteur
;    . dh    : tête
;    . dl    : lecteur
;    . es:bx : adresse destination
;-------------------------------------------------------------------------------
%define NB_SECT_MAX 72

chargerPleinSecteurs :

        ; On stocke les paramètres
        mov word [nbTotalSecteurs], ax
	mov [numCylindre], ch
	mov [numSecteur], cl
	mov [numTete], dh
	mov [numLecteur], dl

        mov ch, 0
        mov [numAbsProchainSec], cx
	
cpsProchaineLecture :
        ; Si il reste moins de 255 secteurs, on est ok !
	cmp word [nbTotalSecteurs], NB_SECT_MAX
        jbe cpsDerniereLecture

        ; On ne va pas lire plus de 256 secteurs
        mov byte [nbSecteurs], NB_SECT_MAX
		
cpsDerniereLecture :
        mov word ax, [nbTotalSecteurs]
        mov byte [nbSecteurs], al
	
cpsUneLecture :
        mov ah, 0h
	mov al, [nbSecteurs]
        sub word [nbTotalSecteurs], ax

        ; On récupère les paramètres et on appelle chargerSecteurs
	mov al, [nbSecteurs]
	mov ah, 2               ; Lecture
	mov ch, [numCylindre]
	mov cl, [numSecteur]
	mov dh, [numTete]
	mov dl, [numLecteur]
	
        call chargerSecteurs

        ; On calcule la position absolue du prochain secteur
	mov ah, 0
	add ax, [numAbsProchainSec]
	mov [numAbsProchainSec], ax

        ; On en déduit les paramètres
        mov cx, 18
        div cx       ; al <- quotient, ah <- reste
	mov ax, 0
	mov [numSecteur], ah
	mov cx, 2
	div cx       ; al <- quotient, ah <- reste
	mov [numTete], ah
	mov [numCylindre], al

        ; S'il en reste à lire, on recommence
        cmp word [nbTotalSecteurs], 0
	ja cpsProchaineLecture

        ret
	
; Le nombre total de secteurs à lire
nbTotalSecteurs   dw 0

; le numéro absolu du prochain secteur
numAbsProchainSec dw 0

; Ci dessous les paramètres à passer à chargerSecteurs
nbSecteurs        db 0
numSecteur        db 0
numCylindre       db 0
numLecteur        db 0
numTete           db 0

 