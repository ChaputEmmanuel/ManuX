/*----------------------------------------------------------------------------*/
/*      Ma version des assertions et autres outils de debug.                  */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_DEBUG_DEF
#define MANUX_DEBUG_DEF

#include <manux/config.h>
#include <manux/printk.h>

#define DBG_KERNEL_START 0x00000001
#define DBG_KERNEL_PAGIN 0x00000002
#define DBG_KERNEL_SYSFI 0x00000004
#define DBG_KERNEL_ORDON 0x00000008
#define DBG_KERNEL_TACHE 0x00000010

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
  // | 0xFFFFFFFF
//  ;

#define masqueDebugage (0x00000000  \
  | DBG_KERNEL_START    \
  | DBG_KERNEL_TACHE    \
  | DBG_KERNEL_SYSFI    \
			)

//  | DBG_KERNEL_FILES

/*
 * Une fonction permettant d'afficher des messages de debug thématiques
 * et avec un formatage homogène.
 */
#define printk_debug(lvl, fmt, args...)	 \
   if ((lvl)& masqueDebugage)                    \
     printk("[%10d] %s line %d : " fmt, nbTicks, __FUNCTION__ , __LINE__, ## args)

/*
 * Affichage d'un message de panique
 */
void paniqueNoyau(const char * funcName, const char * fileName, int lineNum, char * msg);

/*
 * Ma version simplifiée de assert
 */
#ifdef MANUX_ASSERT_ACTIVES
#define assert(cond)(				\
   (cond)?0:paniqueNoyau(__FUNCTION__, __FILE__, __LINE__, "L'assertion '"#cond"' n'est pas verifiee") \
   )
#else
#define assert(cond) {}
#endif

#endif
