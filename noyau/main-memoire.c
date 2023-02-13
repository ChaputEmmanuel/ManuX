/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/memoire.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/debug.h>
/*
 * Structure passée en paramètre par la phase d'init
 * (cf init-manux.nasm)
 */
typedef struct _InfoSysteme {
   uint32_t flags;           // Pour compatibilité avec multiboot
   uint32_t memoireDeBase;   // En Ko
   uint32_t memoireEtendue;  // En Ko
   uint32_t tailleRamdisk;   // En Ko (0 si pas de ramdisk)
   uint32_t adresseRamdisk;
} InfoSysteme;

#define NB_PAGES_ALLOC 64

void _start(InfoSysteme * infoSysteme,
	    uint32_t adresseDebutManuX,
	    uint32_t adresseFinManuX)
{
   void * p;
   
   // Initialisation de la console noyau
   consoleInitialisation();

   // Affichage de la mémoire disponible 
   printk_debug(DBG_KERNEL_START, "Memoire : %d + %d Ko\n",
		infoSysteme->memoireDeBase,
		infoSysteme->memoireEtendue);

   /* Initialisation de la gestion mémoire */
   printk_debug(DBG_KERNEL_START, "Initialisation memoire ...\n");
   initialiserMemoire(infoSysteme->memoireDeBase,
		      infoSysteme->memoireEtendue,
		      adresseDebutManuX,
		      adresseFinManuX);
   printk_debug(DBG_KERNEL_START, "Memoire initialisee\n");

   printk("Allocation des pages :\n");
   for (int n = 0; n < NB_PAGES_ALLOC; n++) {
      p = allouerPage();
      printk("0x%x ", p);
   }
}   /* _start */

