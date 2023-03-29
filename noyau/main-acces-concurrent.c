/**
 * @file main-acces-concurrent.c
 * @brief Un exemple pitoyable de début de noyau.
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/memoire.h>
#include <manux/journal.h>       /* initialiserJournal() */
#include <manux/fichier.h>
#include <manux/bootloader.h>
#include <manux/clavier.h>
#include <manux/intel-8259a.h>
#include <manux/pagination.h>

/**
 * Configuration de la console
 */
INoeud  iNoeudConsole;  // Le INoeud qui décrit la console

extern void init(); // Faire un init.h

void _startManuX()
{
   // Récupération des informations depuis le bootloader
   bootloaderLireInfo();

   // Initialisation de la console noyau
   consoleInitialisation(&iNoeudConsole);

   bootloaderInitialiser();

   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
   
   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();   

   i8259aInit(MANUX_INT_BASE_IRQ);

   // Initialisation du journal
   journalInitialiser(&iNoeudConsole);

   initialiserPagination(infoSysteme.memoireEtendue);

   initialiserAppelsSysteme();

   initialiserClavier();

   /* Initialisation de la gestion des processus */
   initialiserScheduler();

   initialiserHorloge();

   // Un petit message
   printk("Tests d'acces concurrents, ...\n");

   init();
}   /* _startManuX */


