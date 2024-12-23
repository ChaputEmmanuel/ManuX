/**
 * @file : noyau/main-net.c
 * @brief : Un exemple de début de noyau utilisant les éléments de base réseau.
 *                                                                            
 *                                                     (C) Manu Chaput 2000-2024
 **/
#include <manux/config.h>
#include <manux/debug.h>
#include <manux/bootloader.h>
#include <manux/errno.h>
#include <manux/console.h>
#include <manux/memoire.h>
#include <manux/interruptions.h>
#include <manux/pci.h>
#include <manux/virtio-net.h>

void startManuX()
{
   // Initialisation de la console noyau
   consoleInitialisation();

   bootloaderInitialiser();

   /* Initialisation de la gestion mémoire */
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);
   
   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();   

   /* Initialisation du bus PCI */
   printk_debug(DBG_KERNEL_START, "Initialisation du bus PCI ...\n");
   PCIEnumerationDesEquipements();
   printk_debug(DBG_KERNEL_START, "Bus PCI initialise...\n");

   printk_debug(DBG_KERNEL_START, "Initialisation de virtio net ...\n");
   if (virtioNetInit() != ESUCCES) {
      printk_debug(DBG_KERNEL_START, "Impossible d'initialiser periph reseau ...\n");
   }

   while(1){};   
}   /* startManuX */


