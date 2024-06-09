/**
 * @file  : main-registre.c
 * @brief : 
 *
 *                                                  (C) Manu Chaput 2000-2024 
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
   uint32_t bm;
  
   // Initialisation de la console noyau
   consoleInitialisation();

   bootloaderInitialiser();

   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);

   // Initialsation du système kmalloc
   kmallocInitialisation();

   // On initialise le registre
   registreSystemeInitialiser();

   registreSystemeAjouterParametre(typeParametreU32,
				   (char*)0x483242FF,
				   "systeme", "debug", "mask", NULL);

   registreSystemeAjouterParametre(typeParametreU32,
				   (char*)TRUE,
				   "systeme", "debug", "assert", NULL);

   registreSystemeAjouterParametre(typeParametreU32,
				   (char*)0xc0a80a01,
				   "reseau", "ip", "eth0", "addr", NULL);

   registreSystemeAjouterParametre(typeParametreU32,
				   (char*)0xc0a80c01,
				   "reseau", "ip", "eth1", "addr", NULL);

   registreSystemeAfficher();

   registreSystemeLire(&bm, "systeme", "debug", "mask", NULL);
   printk("Masque debug  : 0x%x\n", bm);
   
   registreSystemeLire(&bm, "reseau", "ip", "eth0", "addr", NULL);
   printk("Addresse eth0 : 0x%x\n", bm);
   
   registreSystemeLire(&bm, "reseau", "ip", "eth1", "addr", NULL);
   printk("Addresse eth1 : 0x%x\n", bm);
}   /* startManuX */


