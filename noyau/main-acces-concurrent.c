/**
 * @file main-acces-concurrent.c
 * @brief Un exemple pitoyable de début de noyau.
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/errno.h>
#include <manux/debug.h>      // A virer j'espère !
#include <manux/console.h>
#include <manux/kmalloc.h>    // Pour l'initialisation
#include <manux/printk.h>
#include <manux/memoire.h>
#include <manux/journal.h>    // initialiserJournal()
#include <manux/fichier.h>
#include <manux/bootloader.h>
#include <manux/clavier.h>
#include <manux/pagination.h>

extern void init(void); // Faire un init.h
void startManuX(void);

void startManuX(void)
{
   // Initialisation de la console
   consoleInitialisation();

   // Récupération des informations depuis le bootloader
   bootloaderInitialiser();

   // Initialisation de la gestion des pages mémoire (utilise le
   // bootloader)
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
   
   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();

   // Initialisation de l'allocateur généraliste
   kmallocInitialisation();
   
   // Le clavier nous permettra de basculer entre consoles
   initialiserClavier();

   // On va utiliser des appels systèmes
   initialiserAppelsSysteme();

   // On va utiliser des tubes, donc le système de fichiers
   sfInitialiser();

   // Initialisation de la gestion des processus
   initialiserScheduler();

   // On a besoin de l'horloge pour l'ordonnanceur
   initialiserHorloge();

   printk_debug(DBG_KERNEL_START, "On passe en usr ...\n");

   // Avant de passer la main à init, on relâche le verrou global. On
   // n'a donc plus accès au noyau, il faudra passer par des appels
   // systèmes
   
#if defined(MANUX_TACHES) && !defined(MANUX_REENTRANT)
   tacheDansLeNoyau = 0;
   sortirExclusionMutuelle(&verrouGeneralDuNoyau);
#endif

   init();
}   /* _startManuX */


