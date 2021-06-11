/*----------------------------------------------------------------------------*/
/*      Ma version des assertions et autres outils de debug.                  */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_DEBUG_DEF
#define MANUX_DEBUG_DEF

#include <manux/config.h>
#include <manux/printk.h>
#include <manux/stdarg.h>
#include <manux/i386.h>       // halt()
#include <manux/scheduler.h>  // tacheEnCours

#define DBG_KERNEL_ERREUR   0x00000001
#define DBG_KERNEL_START    0x00000002
#define DBG_KERNEL_PAGIN    0x00000004
#define DBG_KERNEL_SYSFI    0x00000008
#define DBG_KERNEL_ORDON    0x00000010
#define DBG_KERNEL_TACHE    0x00000020
#define DBG_KERNEL_MEMOIRE  0x00000040
#define DBG_KERNEL_ALL      0xFFFFFFFF

// WARNING ! A voir pourquoi la définition suivante ne fonctionne pas
// Pour être plus précis, sa valeur ne change rien ... sauf lorsque
// j'essaie de l'afficher avec printk_debug !!!! Ca sent le gros bug,
// mais je ne le trouve évidemment pas, ...
//    Après, ce n'est pas hyper important en soi, mais ça agace !
//
//static unsigned long masqueDebugage __attribute__ ((unused)) = 0x00000000
  // | DBG_KERNEL_START
  // | DBG_KERNEL_PAGIN
  // | DBG_KERNEL_FILES
  // | DBG_KERNEL_ALL
//  ;

#define masqueDebugage (0x00000000  \
 | DBG_KERNEL_START \
 | DBG_KERNEL_TACHE \
 | DBG_KERNEL_MEMOIRE \
| DBG_KERNEL_ALL \
			)

//  | DBG_KERNEL_START
//  | DBG_KERNEL_TACHE
//  | DBG_KERNEL_ORDON
//  | DBG_KERNEL_ALL
//  | DBG_KERNEL_FILES

/*
 * Une fonction permettant d'afficher des messages de debug thématiques
 * et avec un formatage homogène.
 */
#define printk_debug(lvl, fmt, args...)	 \
   if ((lvl)& masqueDebugage)                    \
     printk("[%d] %s line %d : " fmt, nbTopHorloge, __FUNCTION__ , __LINE__, ## args)

/*
 * Affichage d'un message de panique
 */
#ifdef MANUX_TACHES
#define paniqueNoyau(fmt, args...)	                                  \
   printk("\n*** PANIQUE NOYAU (tache %d) ***\n", tacheEnCours->numero);  \
   printk("%s (dans %s ligne %d)\n", __FUNCTION__, __FILE__, __LINE__);   \
   printk("" fmt, ## args);                                               \
   asm( "hlt" );
#else
#define paniqueNoyau(fmt, args...)	                                  \
  printk("\n*** PANIQUE NOYAU  ***\n");                                   \
   printk("%s (dans %s ligne %d)\n", __FUNCTION__, __FILE__, __LINE__);   \
   printk("" fmt, ## args);                                               \
   asm( "hlt" );
#endif

/*
 * Ma version simplifiée de assert
 */
#ifdef MANUX_ASSERT_ACTIVES
#define assert(cond)     \
  if (!(cond)) {					      \
    paniqueNoyau("L'assertion '"#cond"' n'est pas verifiee"); \
  }

#else
#define assert(cond) {}
#endif

#endif
