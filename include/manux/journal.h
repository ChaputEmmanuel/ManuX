/**
 * @file journal.h
 * @brief Définition des outils de journalisation des messages du noyau.      
 * 
 *  Pour le moment, ces messages seront envoyés sur une console virtuelle. 
 *
 *                                                  (C) Manu Chaput 2002-2023
 */
#ifndef JOURNAL_DEF
#define JOURNAL_DEF

#include <manux/types.h>
#include <manux/fichier.h>
#include <manux/console.h>

/**
 * @brief Initialisation du système de journalisation.
 */
void journalInitialiser(INoeud * iNoeudConsole);

/**
 * @brief Affectation d'un fichier sur lequel seront envoyés les
 * messages journalisés
 */
void journalAffecterFichier(Fichier * pc);

/**
 * @brief Journalisation d'un message.
 */
void journaliser(char * message, int len);

booleen journalOperationnel();


#endif
