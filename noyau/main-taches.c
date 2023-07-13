/**
 * @file : main-taches.c
 * @brief : Démonstration d'utilisation des tâches
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/clavier.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/interruptions.h>
#include <manux/intel-8259a.h>
#include <manux/tache.h>
#include <manux/scheduler.h>
#include <manux/memoire.h>
#include <manux/bootloader.h>

void albert()
{
  while (1) {
    printk("O");
  }
}

void barnabe()
{
  while (1) {
    printk("X");
  }
}

void startManuX()
{
   Tache * t0, *t1;

   // Récupération des informations depuis le bootloader
   bootloaderLireInfo();
   
   // Initialisation de la console noyau
   consoleInitialisation();

   // Initialisation des informations depuis le bootloader
   bootloaderInitialiser();

   // Affichage de la mémoire disponible 
   printk("Memoire : %d + %d Ko\n",
	  infoSysteme.memoireDeBase,
	  infoSysteme.memoireEtendue);

   /* Initialisation de la gestion mémoire */
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
  
   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();   

   i8259aInit(MANUX_INT_BASE_IRQ);

   initialiserClavier();

   initialiserScheduler();

   initialiserHorloge();

   // Affichage d'un message
   consoleNoyauAfficher("Creation de deux taches ...\n");

   t0 = tacheCreer(albert);
   //   tacheSetConsole(t0, creerConsoleVirtuelle());
   t1 = tacheCreer(barnabe);
   //tacheSetConsole(t1, creerConsoleVirtuelle());

   ordonnanceurAddTache(t0);
   ordonnanceurAddTache(t1);
   while (1) {
     //     printk(".");
   }
   
}   /* _startManuX */

