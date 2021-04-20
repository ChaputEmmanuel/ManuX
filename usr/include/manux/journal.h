/*----------------------------------------------------------------------------*/
/*      Définition des outils de journalisation des messages du noyau.        */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                       (C) Manu Chaput 2002 */
/*----------------------------------------------------------------------------*/
#ifndef JOURNAL_DEF
#define JOURNAL_DEF

void initialiserJournal();
/*
 * Initialisation du système de journalisation.
 */

void journaliser(char * message);
/*
 * Journalisation d'un message.
 */

#endif
