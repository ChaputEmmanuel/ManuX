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
#include <manux/printk.h>
#include <manux/memoire.h>
#include <manux/journal.h>       /* initialiserJournal() */
#include <manux/fichier.h>
#include <manux/bootloader.h>
#include <manux/clavier.h>
#include <manux/pagination.h>

/**
 * Configuration de la console
 */
INoeud  iNoeudConsole;  // Le INoeud qui décrit la console

extern void init(); // Faire un init.h

#ifdef MANUX_VIRTIO_CONSOLE
INoeud iNoeudVirtioConsole;
Fichier fichierVirtioConsole;
#endif

void startManuX()
{
   // Récupération des informations depuis le bootloader
   bootloaderLireInfo();

   // Initialisation de la console noyau
   consoleInitialisationINoeud(&iNoeudConsole);

   bootloaderInitialiser();

   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
   
   /* Initilisation des descripteurs de segments */
   printk_debug(DBG_KERNEL_START, "Initialisation de la GDT ...\n");
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();

   // Le clavier nous permettra de basculer entre consoles
   initialiserClavier();

   // Initialisation du journal
   printk_debug(DBG_KERNEL_START, "Initialisation du journal ...\n");
   journalInitialiser();

   // On va utiliser des appels systèmes
   printk_debug(DBG_KERNEL_START, "Initialisation des AS ...\n");
   initialiserAppelsSysteme();

   // Le clavier va nous servir à basculer entre consoles
   printk_debug(DBG_KERNEL_START, "Initialisation du clavier ...\n");
   initialiserClavier();

   // On va utiliser des tubes, donc le système de fichiers
   printk_debug(DBG_KERNEL_START, "Initialisation des fichiers ...\n");
   sfInitialiser();

   // Initialisation de la gestion des processus
   printk_debug(DBG_KERNEL_START, "Initialisation du scheduler ...\n");
   initialiserScheduler();

   // On a besoin de l'horloge pour l'ordonnanceur
   printk_debug(DBG_KERNEL_START, "Initialisation de l'horloge...\n");
   initialiserHorloge();

   // Un petit message
   printk("Tests d'acces concurrents, ...\n");

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


