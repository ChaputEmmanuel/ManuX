/*----------------------------------------------------------------------------*/
/*      Ma version des assertions et autres outils de debug.                  */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_DEBUG_DEF
#define MANUX_DEBUG_DEF

#include <manux/config.h>
#include <manux/printk.h>

#define masqueDebugage 0xFFFFFFFF

#define DBG_KERNEL_START 0x00000001

/*
 * Une fonction permettant d'afficher des messages de debug thématiques
 * et avec un formatage homogène.
 */
#define printk_debug(lvl, fmt, args...)	 \
   if ((lvl)& masqueDebugage)                    \
     printk("[%10d] %s line %d : %s" fmt,  nbTicks, __FUNCTION__ , __LINE__, ## args)

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
