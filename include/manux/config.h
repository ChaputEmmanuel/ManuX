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
#ifndef MANUX_INIT_START_ADDRESS
#   define MANUX_INIT_START_ADDRESS  0x1000
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

/*----------------------------------------------------------------------------*/
/* Utilisation d'un système de fichiers                                       */
/*----------------------------------------------------------------------------*/
//#define MANUX_FS

/*
 * Le nombre maximal de fichiers manipulés par un processus
 * WARNING : sans aucun intéret pour le moment !
 */
#ifndef MANUX_NB_MAX_FICHIERS
#   define MANUX_NB_MAX_FICHIERS  4
#endif

/*----------------------------------------------------------------------------*/
/* Gestion du RAMdisk.                                                        */ 
/*----------------------------------------------------------------------------*/
/*
 * Utilisation d'un RAMdisk ou non
 */
//#define MANUX_RAMDISK

/*
 * Sa taille. WARNING : à calculer par un outils comme la taille du noyau
 */
#ifndef MANUX_NB_SECT_RAMDISK
#   define MANUX_NB_SECT_RAMDISK 0x2
#endif

#ifndef MANUX_SEGMENT_TRANSIT_RAMDISK
#   define MANUX_SEGMENT_TRANSIT_RAMDISK 0x4000
#endif

#ifndef MANUX_PREMIER_SECT_RAMDISK 
#   define MANUX_PREMIER_SECT_RAMDISK 0x10
#endif

/*----------------------------------------------------------------------------*/
/*   Configuration générale du noyau                                          */ 
/*----------------------------------------------------------------------------*/
/*
 * Un "noyau" minimaliste : il affiche juste un message. Le but est de
 * le réduire à quasiment rien pour mettre en place les mécanismes de
 * boot.
 */

/*
 * La fréquence du timer
 */
#define MANUX_FREQUENCE_TIMER 100

/*
 * Utilisation (ou non) des consoles virtuelles. Si on ne les utilise
 * pas, tout ce qui est affiché est mélangé à l'écran.
 */
#define MANUX_CONSOLES_VIRTUELLES 

/*
 * Utilise-t-on un mécanisme de journal des messages du noyau ?
 */
#define MANUX_JOURNAL

/*
 * Doit-on activer les "assert" ? Si cette macro n'est pas définie,
 * les assert ne produisent aucun code.
 */
//#define MANUX_ASSERT_ACTIVES

/*
 * Utilisation des outils de synchronisation (mutex, semaphore, ...)
 */
//#define MANUX_OUTILS_SYNCHRO

/*
 * Définition des appels système
 */
#define MANUX_APPELS_SYSTEME

/*----------------------------------------------------------------------------*/
/*   Gestion des tâches et ordonnancement.                                    */
/*----------------------------------------------------------------------------*/
/*
 * Implantation des tâches ? 
 */
#define MANUX_TACHES

/*
 * Ordonnancement préemptif ?
 */
#define MANUX_PREEMPTIF

/*----------------------------------------------------------------------------*/
/*   Gestion de la mémoire.                                                   */
/*----------------------------------------------------------------------------*/
#define MANUX_GESTION_MEMOIRE

/*
 * Taille d'une page mémoire (4 Ko)
 */
#define MANUX_TAILLE_PAGE           0x1000

/*
 * Nombres de pages "système" c'est-à-dire communes à toutes les tâches.
 * WARNING, il serait bon de le calculer en fonction de la taille de la
 * mémoire physique. 
 */
#define MANUX_NOMBRE_PAGES_SYSTEME 0x800   /* 8 Mo */

/*
 * Adresse utilisée pour le tableau d'affectation des pages
 */
#ifndef MANUX_AFFECTATION_PAGES
#   define MANUX_AFFECTATION_PAGES 0x1000
#endif

/*
 * Adresse de début de la zone gérée par malloc
 */
#ifndef MANUX_ADRESSE_DEBUT_TAS
#   define MANUX_ADRESSE_DEBUT_TAS 0x1000000
#endif

/*
 * Active-t-on la pagination ?
 */
#define MANUX_PAGINATION

/*----------------------------------------------------------------------------*/
/* Gestion du clavier.                                                        */
/*----------------------------------------------------------------------------*/
#define MANUX_CLAVIER

#ifndef MANUX_portCmdClavier
#   define MANUX_portCmdClavier  0x64
#endif

#ifndef MANUX_portDonneesClavier
#   define MANUX_portDonneesClavier  0x60
#endif

/*----------------------------------------------------------------------------*/
/*   Et maintenant quelques vérifications de cohérence de la configuration.   */
/* Sans exhaustivité malheureusement.                                         */
/*----------------------------------------------------------------------------*/
/*
 * Le ramdisk ne sait pas trouver la taille seul
 */
#ifdef MANUX_TACHES
#   ifndef MANUX_GESTION_MEMOIRE
#      error "MANUX_TACHES nécessite MANUX_GESTION_MEMOIRE !"
#   endif
#endif

#endif
