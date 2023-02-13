/*----------------------------------------------------------------------------*/
/*      Définition des éléments de configuration de ManuX. Les options et     */
/* valeurs définies ici sont utilisées dans le code C mais également parfois  */
/* ailleurs. Le Makefile génère pour cela un fichier make.conf qui est ensuite*/
/* inclus.                                                                    */
/*                                                                            */
/*    Pour le bon fonctionnement de cette procédure, il est impératif que les */
/* macros en question débutent par le préfixe MANUX_                          */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/

/*
 * On peut définir un autre fichier de configuration générale
 * WARNING, vus les résultats, ça fonctione !!! Pourquoi ???
 */
#ifdef MANUX_FICHIER_CONFIG
#   error "Cette chose ne devrait pas se produire !"
#   include MANUX_FICHIER_CONFIG
#else
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

/*============================================================================*/
/*   Définitions liées au démarrage.                                          */
/*============================================================================*/
/*----------------------------------------------------------------------------*/
/* Organisation de la mémoire lors du boot.                                   */
/*----------------------------------------------------------------------------*/
/*
 * Positionnement du code d'init, utilisé en cas de boot sur disquette. On peut
 * réutiliser cette mémoire dans le noyau, puisque le code d'init est terminé
 * lorsque le noyau est initialisé.
 */
#ifndef MANUX_INIT_START_ADDRESS
#   define MANUX_INIT_START_ADDRESS  0x1000
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

/*============================================================================*/
/*   Définitions liées au processeur.                                         */
/*============================================================================*/
/**
 * Utilise-t-on des spécificité i386 ?
 */
#define MANUX_LIBI386

/*----------------------------------------------------------------------------*/
/* Définition du fichier main. Afin de ne pas le surcharger de #ifdef, j'en   */
/* fais plusieurs versions qui me permettent de faire des tests sur des sous  */
/* ensemble plus limités. A utiliser avec un fichier de config spécifique à   */
/* chaque fois a priori.                                                      */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_FICHIER_MAIN
#   define MANUX_FICHIER_MAIN main
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

/*----------------------------------------------------------------------------*/
/* Définition d'une interface de type fichiers. Elle pourra permettre un accès*/
/* homogène à différentes ressources (par exemple la console), et elle sera   */
/* évidemment nécessaire pour implanter un système de fichiers.               */
/*----------------------------------------------------------------------------*/
#define MANUX_FICHIER

/*
 * Le nombre maximal de fichiers manipulés par un processus
 * WARNING : sans aucun intéret pour le moment !
 */
#ifndef MANUX_NB_MAX_FICHIERS
#   define MANUX_NB_MAX_FICHIERS  4
#endif

/*----------------------------------------------------------------------------*/
/* Définition de périphérique caractère. En pause, je n'en vois pas la        */
/* nécessité pour le moment                                                   */
/*----------------------------------------------------------------------------*/
//#define MANUX_PERIPHERIQUE_CARACTERE

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
#   define MANUX_NB_HANDLER_PAR_IRQ 4
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
#define IRQ_CLAVIER   1

/*----------------------------------------------------------------------------*/
/*   Configuration générale du noyau                                          */ 
/*----------------------------------------------------------------------------*/

/*
 * La fréquence du timer
 */
#define MANUX_FREQUENCE_HORLOGE 100

/*
 * Le numéro majeur des consoles
 */
#define MANUX_CONSOLE_MAJEUR  0

/*
 * Utilisation (ou non) des consoles virtuelles. Si on ne les utilise
 * pas, tout ce qui est affiché est mélangé à l'écran.
 */
#define MANUX_CONSOLES_VIRTUELLES 

/*
 * Lorsqu'on crée une nouvelle console, est-ce que l'on bascule
 * automatiquement vers elle ? 
 */
#define MANUX_BASCULER_NOUVELLE_CONSOLE

/*
 * Affectation d'une console à chaque tâche. Si ce n'est pas le cas
 * (et si le reste de la configurtion le permet), ce sont les fichiers
 * associés à la tâche qui sont utilisés pour les entrées-sorties.
 */
#define MANUX_TACHE_CONSOLE

/*
 * Implantation de printk
 */
#define MANUX_PRINTK

/*
 * Utilise-t-on un mécanisme de journal des messages du noyau ?
 */
#define MANUX_JOURNAL

/*
 * Le journal utilise la console via l'interface fichier
 * traditionnelle (read/write). Mais dans certaines phases de debug,
 * ça peut s'avérer utile de contourner ça et de l'accéder
 * directement.
 */
#define MANUX_JOURNAL_DIRECT_CONSOLE

/**
 * On affichera les messages d'un niveau plus faible ou égal à
 * MANUX_JOURNAL_NIVEAU_DEFAUT
 */
#define MANUX_JOURNAL_NIVEAU_DEFAUT       7

/*
 * Doit-on activer les "assert" ? Si cette macro n'est pas définie,
 * les assert ne produisent aucun code.
 */
#define MANUX_ASSERT_ACTIVES

/*
 * Utilisation des outils de synchronisation (mutex, semaphore, ...)
 */
#define MANUX_OUTILS_SYNCHRO

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
/**
 * A-t-on des outils de base de gestion de la mémoire ? On ne parle
 * ici que d'une gestion par page.
 */
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
 * Active-t-on la pagination ?
 */
#define MANUX_PAGINATION

/*----------------------------------------------------------------------------*/
/* Implantation de kmalloc                                                    */
/*----------------------------------------------------------------------------*/
/*
#define MANUX_KMALLOC kmalloc-zs
#define MANUX_KMALLOC_STAT
*/

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

/*
 * Le clavier est transmis vers la console active
 */
#define MANUX_CLAVIER_CONSOLE

/*----------------------------------------------------------------------------*/
/*   Prise en compte du bus PCI.                                              */
/*----------------------------------------------------------------------------*/
#define MANUX_PCI

/*
 * Pour le moment, on ne cherche qu'un bus, donc aucun risque d'avoir plus de 
 * 32 équipements.
 */
#define MANUX_NB_MAX_EQUIPEMENTS_PCI 32

/*----------------------------------------------------------------------------*/
/* Le réseau.                                                                 */
/*----------------------------------------------------------------------------*/
#define MANUX_RESEAU

/*----------------------------------------------------------------------------*/
/* Les pilotes de périphériques.                                              */
/*----------------------------------------------------------------------------*/
#define MANUX_VIRTIO

/*
 * Du réseau
 */
//#define MANUX_VIRTIO_NET

/*
 * Une console
 */
#define MANUX_VIRTIO_CONSOLE

/*
 * Le numéro majeur des consoles virtio
 */
#define MANUX_VIRTIO_CONSOLE_MAJEUR 1

/*----------------------------------------------------------------------------*/
/* La stdlib propose quelques fonctions standard                              */
/*----------------------------------------------------------------------------*/
#define MANUX_STDLIB

/*----------------------------------------------------------------------------*/
/* Est-ce que l'on incorpore le monde utilisateur ?                           */
/*----------------------------------------------------------------------------*/
#define MANUX_USR

/*----------------------------------------------------------------------------*/
/*   Et maintenant quelques vérifications de cohérence de la configuration.   */
/* Sans exhaustivité malheureusement.                                         */
/*----------------------------------------------------------------------------*/
/*
 * Pour la première, je ne suis pas si sûr, ...
 */
#ifdef MANUX_TACHES
#   ifndef MANUX_GESTION_MEMOIRE
#      error "MANUX_TACHES nécessite MANUX_GESTION_MEMOIRE !"
#   endif
#endif

#if defined(MANUX_CLAVIER_CONSOLE) && !defined(MANUX_APPELS_SYSTEME)
#   error "MANUX_CLAVIER_CONSOLE nécessite MANUX_APPELS_SYSTEME"
#endif

#if defined(MANUX_VIRTIO_NET) && !defined(MANUX_RESEAU)
#   error "VIRTIO_NET est un périphérique nécessitant MANUX_RESEAU"
#endif

#if defined(MANUX_VIRTIO_NET) && !defined(MANUX_VIRTIO)
#   error "VIRTIO_NET est un périphérique nécessitant MANUX_VIRTIO"
#endif

#if defined(MANUX_VIRTIO) && !defined(MANUX_PCI)
#   error "VIRTIO est un système nécessitant MANUX_PCI"
#endif

#if defined(MANUX_PERIPHERIQUE_CARACTERE) && !defined(MANUX_FICHIER)
#   error "Les périphériques caractères nécessitent le type fichier"
#endif

#endif  // MANUX_CONFIG
#endif  // MANUX_FICHIER_CONFIG
