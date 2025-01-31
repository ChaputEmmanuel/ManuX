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
#include <manux/bootloader.h>
#include <manux/pagination.h>

#include <manux/pci.h>
#include <manux/reseau.h>
#include <manux/virtio-net.h>

void startManuX()
{
   // Récupération des informations depuis le bootloader
   bootloaderInitialiser();

   // Gestion globale de la mémoire
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
   
   // Initialisation de la console noyau
   consoleInitialisation();

   // Initilisation des descripteurs de segments
   printk_debug(DBG_KERNEL_START, "Initialisation de la GDT ...\n");
   initialiserGDT();

   // Initialisation de la table des interruptions
   initialiserIDT();

   // Initialisation de l'allocateur généraliste
   kmallocInitialisation();
   
   // Initialisation de la gestion des processus
   printk_debug(DBG_KERNEL_START, "Initialisation du scheduler ...\n");
   initialiserScheduler();

   // On a besoin de l'horloge pour l'ordonnanceur
   printk_debug(DBG_KERNEL_START, "Initialisation de l'horloge ...\n");
   initialiserHorloge();

   // On initialise le sous-système réseau
   printk_debug(DBG_KERNEL_START, "Initialisation du reseau ...\n");
   reseauInitialiser();

   // Initialisation du bus PCI 
   printk_debug(DBG_KERNEL_START, "Initialisation du bus PCI ...\n");
   PCIEnumerationDesEquipements();
   printk_debug(DBG_KERNEL_START, "Bus PCI initialise ...\n");

   printk_debug(DBG_KERNEL_START, "Initialisation de virtio net ...\n");
   if (virtioNetInit() != ESUCCES) {
      printk_debug(DBG_KERNEL_START, "Impossible d'initialiser periph reseau ...\n");
   }

   printk_debug(DBG_KERNEL_START, "C'est parti mon kiki ...\n");

   while(1){};
}   /* _startManuX */


