/*----------------------------------------------------------------------------*/
/*      Définition des outils de journalisation des messages du noyau.        */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#ifndef JOURNAL_DEF
#define JOURNAL_DEF

#ifdef MANUX_JOURNAL_USES_FILES
#   include <manux/fichier.h>
#else
#   include <manux/console.h>
#endif

#ifdef MANUX_JOURNAL_USES_FILES
extern Fichier * journal;

void initialiserJournal(Fichier * f);
/*
 * Initialisation du système de journalisation.
 */

#else
extern Console * journal;
void initialiserJournal(Console * console);
/*
 * Initialisation du système de journalisation.
 */
#endif

void journaliser(char * message, int len);
/*
 * Journalisation d'un message.
 */

void afficherJournal();
/*
 * Forcer l'affichage du journal à l'écran
 */


#endif
