/*----------------------------------------------------------------------------*/
/*      Définition des IRQ d'un PC de base.                                   */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef IRQ_DEF
#define IRQ_DEF

#include <manux/types.h>

/*
 * Définition de quelques IRQ classiques
 */
#define IRQTimer   0x00
#define IRQClavier 0x01

void interdireIRQ(uint8_t numIRQ);
/*
 * Empécher toute occurence d'une IRQ donnée
 */

void autoriserIRQ(uint8_t numIRQ);
/*
 * Autoriser les occurences d'une IRQ donnée
 */

#endif
