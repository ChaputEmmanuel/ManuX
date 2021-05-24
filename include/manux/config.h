/*----------------------------------------------------------------------------*/
/*      Définition des éléments de configuration de ManuX.                    */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

/*----------------------------------------------------------------------------*/
/* Organisation de la mémoire lors du boot.                                   */
/*----------------------------------------------------------------------------*/
#ifndef INIT_START_ADDRESS
#   define INIT_START_ADDRESS  0x1000
#endif

/*
 * Adresse de la fonction _start de main.c attention, les 4 bits de
 * poids faible doivent être nuls (voir bootsector.nasm)
 */
#ifndef KERNEL_START_ADDRESS
#   define KERNEL_START_ADDRESS 0x20000
#endif

#ifndef MANUX_STACK_SEG_16
#   define MANUX_STACK_SEG_16 0x9000
#endif

#ifndef ELF_HEADER_SIZE
#   define ELF_HEADER_SIZE 0x80
#endif

/*
 * Adresse de l'écran
 */
#ifndef ADRESSE_ECRAN
#   define ADRESSE_ECRAN 0xb8000
#endif

#ifndef NB_SECT_INIT
#   define NB_SECT_INIT 0x02
#endif

/*
 * Gestion du RAMdisk (WARNING : à calculer comme pour le noyau)
 */
#ifndef NB_SECT_RAMDISK
#   define NB_SECT_RAMDISK 0x2
#endif

#ifndef SEGMENT_TRANSIT_RAMDISK
#   define SEGMENT_TRANSIT_RAMDISK 0x4000
#endif

#ifndef PREMIER_SECT_RAMDISK 
#   define PREMIER_SECT_RAMDISK 0x10
#endif

/*----------------------------------------------------------------------------*/
/* Définition des indices de descripteurs dans la GDT.                        */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_CODE_SEG_SEL
#   define MANUX_CODE_SEG_SEL 0x08
#endif

#ifndef MANUX_DATA_SEG_SEL
#   define MANUX_DATA_SEG_SEL 0x10
#endif

#ifndef MANUX_KERNEL_TASK_TSS_IND
#   define MANUX_KERNEL_TASK_TSS_IND  0x20
#endif

/*
 * Taille d'une page mémoire (4 Ko)
 */
#define TAILLE_PAGE           0x1000

/*
 * Nombres de pages "système" c'est-à-dire communes à toutes les tâches.
 * WARNING, il serait bon de le calculer en fonction de la taille de la
 * mémoire physique. 
 */
#define NOMBRE_PAGES_SYSTEME 0x800   /* 8 Mo */

/*
 * Adresse utilisée pour le tableau d'affectation des pages
 */
#ifndef AFFECTATION_PAGES
#   define AFFECTATION_PAGES 0x1000
#endif

/*
 * Adresse de début de la zone gérée par malloc
 */
#ifndef ADRESSE_DEBUT_TAS
#   define ADRESSE_DEBUT_TAS 0x1000000
#endif

/*
 * Utilisation d'un système de fichiers
 */
#define MANUX_FS

/*
 * Utilisation d'un RAMdisk
 */
//#define MANUX_RAMDISK

/*
 * La fréquence du timer
 */
#define MANUX_FREQUENCE_TIMER 100

/*
 * Ordonnancement préemptif ?
 */
#define MANUX_PREEMPTIF

/*
 * Le nombre maximal de fichiers manipulés par un processus
 * WARNING : sans aucun intéret pour le moment !
 */
#ifndef NB_MAX_FICHIERS
#   define NB_MAX_FICHIERS  4
#endif

/*
 * Utilisation (ou non) des consoles virtuelles. Si on ne les utilise
 * pas, tout ce qui est affiché est mélangé à l'écran.
 */
#define CONSOLES_VIRTUELLES 

/*
 * Utilise-t-on un mécanisme de journal des messages du noyau ?
 */
//#define MANUX_JOURNAL

/*
 * Doit-on activer les "assert" ? Si cette macro n'est pas définie,
 * les assert ne produisent aucun code.
 */
#define MANUX_ASSERT_ACTIVES

/*
 * Active-t-on la pagination ?
 */
//#define MANUX_PAGINATION

/*----------------------------------------------------------------------------*/
/* Gestion du clavier.                                                        */
/*----------------------------------------------------------------------------*/
#ifndef portCmdClavier
#   define portCmdClavier  0x64
#endif

#ifndef portDonneesClavier
#   define portDonneesClavier  0x60
#endif

#endif
