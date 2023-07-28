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
#include <manux/intel-8259a.h>
#include <manux/pagination.h>
#ifdef MANUX_VIRTIO_CONSOLE
#   include <manux/virtio-console.h>
#endif

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
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();   

   i8259aInit(MANUX_INT_BASE_IRQ);

   // Initialisation du journal
   journalInitialiser(&iNoeudConsole);

   // On va utiliser des appels systèmes
   initialiserAppelsSysteme();

   initialiserClavier();

   // On va utiliser des tubes, donc le système de fichiers
   sfInitialiser();

   // On utilise une console virtio pour afficher la console
#ifdef MANUX_VIRTIO_CONSOLE
   printk_debug(DBG_KERNEL_START, "Initialisation de virtio console ...\n");
   if (virtioConsoleInitialisation(&iNoeudVirtioConsole) == ESUCCES) {
      ouvrirFichier(&iNoeudVirtioConsole, &fichierVirtioConsole);
      journalAffecterFichier(&fichierVirtioConsole);
   }
   printk_debug(DBG_KERNEL_START, "Virtio console initialise...\n");
#endif

   // Initialisation de la gestion des processus
   initialiserScheduler();

   // On a besoin de l'horloge pour l'ordonnanceur
   initialiserHorloge();

   // Un petit message
   printk("Tests d'acces concurrents, ...\n");

   init();
}   /* _startManuX */


