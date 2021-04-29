/*----------------------------------------------------------------------------*/
/*      Définition des outils de journalisation des messages du noyau.        */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#ifndef JOURNAL_DEF
#define JOURNAL_DEF

#include <manux/console.h>

void initialiserJournal(Console * console);
/*
 * Initialisation du système de journalisation.
 */

void journaliser(char * message);
/*
 * Journalisation d'un message.
 */

void afficherJournal();
/*
 * Forcer l'affichage du journal à l'écran
 */


#endif
