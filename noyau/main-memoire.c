/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/debug.h>
#include <manux/bootloader.h>

#define NB_PAGES_ALLOC 128

void _startManuX()
{
   void * p;

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
   printk("Le noyau va de 0x%x a 0x%x\n",
          adresseDebutManuX, adresseFinManuX);
   printk("La pile actuelle va de 0x%x a 0x%x\n",
          adressePileManuX,
          adresseLimitePileManuX);

   /* Initialisation de la gestion mémoire */
   initialiserMemoire(infoSysteme.memoireDeBase,
		      infoSysteme.memoireEtendue);

   printk("Allocation des pages :\n");
   for (int n = 0; n < NB_PAGES_ALLOC; n++) {
      p = allouerPage();
      printk("0x%x ", p);
   }
   while (1){};   // A remplacer par un halt() 
}   /* _start */

