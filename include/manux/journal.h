/*----------------------------------------------------------------------------*/
/*      Définition des outils de journalisation des messages du noyau.        */
/*   Pour le moment, ces messages seront envoyés sur une console virtuelle.   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#ifndef JOURNAL_DEF
#define JOURNAL_DEF

#include <manux/types.h>
#include <manux/fichier.h>
#include <manux/console.h>

/**
 * Initialisation du système de journalisation.
 */
void journalInitialiser(INoeud * iNoeudConsole);

void journalAffecterFichier(Fichier * pc);

void journaliser(char * message, int len);
/*
 * Journalisation d'un message.
 */

booleen journalOperationnel();


#endif
