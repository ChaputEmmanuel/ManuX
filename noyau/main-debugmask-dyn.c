/**
 * @file  : main-debugmask-dyn.c
 * @brief : 
 *
 *                                                  (C) Manu Chaput 2000-2025 
 */
#include <manux/config.h>
#include <manux/types.h>     // TRUE
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/memoire.h>
#include <manux/kmalloc.h>
#include <manux/bootloader.h>
#include <manux/registre.h>

#include <manux/debug.h>

void startManuX()
{
   // Initialisation de la console noyau
   consoleInitialisation();

   bootloaderInitialiser();

   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);

   // Initialsation du système kmalloc
   kmallocInitialisation();

   // On initialise le registre
   registreSystemeInitialiser();

   // On peut maintenant analyser la ligne de commande
   bootloaderLireLigneCmd();

   // On affiche le registre
   registreSystemeAfficher();

   printk_debug(DBG_KERNEL_START, "Message de start\n");
   printk_debug(DBG_KERNEL_BOOTLOADER, "Message de debug du bootloader\n");
   printk_debug(DBG_KERNEL_A_FAIRE, "Message a faire\n");
}   /* startManuX */


