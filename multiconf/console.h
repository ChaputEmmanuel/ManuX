/*----------------------------------------------------------------------------*/
/*      Définition des éléments de configuration de ManuX. Les options et     */
/* valeurs définies ici sont utilisées dans le code C mais également parfois  */
/* ailleurs. Le Makefile génère pour cela un fichier make.conf qui est ensuite*/
/* inclus.                                                                    */
/*                                                                            */
/*    Pour le bon fonctionnement de cette procédure, il est impératif que les */
/* macros en question débutent par le préfixe MANUX_                          */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

/*----------------------------------------------------------------------------*/
/* Organisation de la mémoire lors du boot.                                   */
/*----------------------------------------------------------------------------*/
/*
 * Positionnement du code d'init, utilisé en cas de boot sur disquette. On peut
 * réutiliser cette mémoire dans le noyau, puisque le code d'init est terminé
 * lorsque le noyau est initialisé.
 */
#ifndef MANUX_BOOT_START_ADDRESS
#   define MANUX_BOOT_START_ADDRESS  0x7c00
#endif
#ifndef MANUX_INIT_START_ADDRESS
#   define MANUX_INIT_START_ADDRESS  0x7e00
#endif

/*----------------------------------------------------------------------------*/
/* Organisation de la mémoire au démarrage.                                   */
/*----------------------------------------------------------------------------*/
/*
 * La position et la taille du BIOS. Par prudence, j'y intègre tout l'EBDA, même
 * s'il est peu probable que ce soit utile ! Voir 
 *    https://wiki.osdev.org/Memory_Map_(x86)
 *    https://stackoverflow.com/questions/64817723/relocating-bootloader-into-ebda
 */
#ifndef MANUX_ADRESSE_BIOS
#   define MANUX_ADRESSE_BIOS 0x80000
#endif

#ifndef MANUX_BIOS_NB_PAGES
#   define MANUX_BIOS_NB_PAGES 0x80
#endif

/*
 * Position de l'IDT (Interrupt Descriptor Table) du noyau.
 */
#ifndef MANUX_ADRESSE_IDT
#   define MANUX_ADRESSE_IDT  0x41000 //0x31000
#endif

#ifndef MANUX_IDT_NB_PAGES
#   define MANUX_IDT_NB_PAGES 1
#endif

/*
 * Position de la GDT (Global Descriptor Table) du noyau.
 */
#ifndef MANUX_ADRESSE_GDT
#   define MANUX_ADRESSE_GDT  0x42000 //0x32000
#endif

#ifndef MANUX_GDT_NB_PAGES
#   define MANUX_GDT_NB_PAGES 1
#endif

/*
 * Adresse de la fonction _start de main.c attention, les 4 bits de
 * poids faible doivent être nuls (voir bootsector.nasm)
 */
#ifndef MANUX_KERNEL_START_ADDRESS
#   define MANUX_KERNEL_START_ADDRESS 0x20000
#endif

#ifndef MANUX_STACK_SEG_16
#   define MANUX_STACK_SEG_16 0x9000
#endif

#ifndef MANUX_ELF_HEADER_SIZE
#   define MANUX_ELF_HEADER_SIZE 0x80
#endif

/*
 * Adresse de l'écran
 */
#ifndef MANUX_ADRESSE_ECRAN
#   define MANUX_ADRESSE_ECRAN 0xb8000
#endif

#ifndef MANUX_NB_SECT_INIT
#   define MANUX_NB_SECT_INIT 0x02
#endif

#ifndef MANUX_portCmdClavier
#   define MANUX_portCmdClavier  0x64
#endif

#ifndef MANUX_portDonneesClavier
#   define MANUX_portDonneesClavier  0x60
#endif

/**
 * @brief Taille réservée pour la pile lors du boot
 */
#ifndef MANUX_TAILLE_PILE
#   define MANUX_TAILLE_PILE 16384
#endif

/*----------------------------------------------------------------------------*/
/* Définition du fichier main. Afin de ne pas le surcharger de #ifdef, j'en   */
/* fais plusieurs versions qui me permettent de faire des tests sur des sous  */
/* ensemble plus limités. A utiliser avec un fichier de config spécifique à   */
/* chaque fois a priori.                                                      */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_FICHIER_MAIN
#   define MANUX_FICHIER_MAIN main-console
#endif

/*----------------------------------------------------------------------------*/
/*   Configuration des interruptions                                          */ 
/*----------------------------------------------------------------------------*/

/**
 * Combien d'interuptions ?
 */
#ifndef MANUX_NB_INTERUPTIONS
#   define MANUX_NB_INTERUPTIONS 256
#endif

/*
 * On utilise ici des intel 8259a
 */
#ifndef MANUX_HANDLER_IRQ
#   define MANUX_HANDLER_IRQ i8259aGestionIRQ
#endif

/*
 * Combien de handlers peut-on greffer sur une interruption ?
 */
#ifndef MANUX_NB_HANDLER_PAR_IRQ
#   define MANUX_NB_HANDLER_PAR_IRQ 1
#endif

/*
 * Premier numéro d'interruption utilisé pour repositionner les IRQs
 */
#ifndef MANUX_INT_BASE_IRQ
#   define MANUX_INT_BASE_IRQ 0x20
#endif

/*
 * On a deux circuits et donc 16 IRQ potentielles
 */
#ifndef I8259A_NB_IRQ
#   define I8259A_NB_IRQ 16
#endif

/*
 * Les IRQ des matériels pris en charge
 */
#define IRQ_HORLOGE   0

/*----------------------------------------------------------------------------*/
/*   Configuration générale du noyau                                          */ 
/*----------------------------------------------------------------------------*/

/*
 * La fréquence du timer
 */
#define MANUX_FREQUENCE_HORLOGE 100

#endif  // MANUX_CONFIG
