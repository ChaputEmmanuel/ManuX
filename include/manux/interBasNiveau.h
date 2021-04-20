/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de bas-niveau permettant de manipuler les    */
/*   interruptions.                                                           */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef INTER_BAS_NIVEAU_DEF
#define INTER_BAS_NIVEAU_DEF

void stubHandlerTimer();
/*
 * Un handler de bas niveau pour le timer (irq 8)
 */

void stubHandlerClavier();
/*
 * Un handler de bas niveau pour le clavier
 */

void stubHandlerNop();
/*
 * Un handler de bas niveau qui ne fait rien
 */

#endif
