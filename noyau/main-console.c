/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>
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


void _start(InfoSysteme * infoSysteme,
	    uint32_t adresseDebutManuX,
	    uint32_t adresseFinManuX)
{
  // Initialisation de la console noyau
   consoleInitialisation();

   // Un petit message
   consoleAfficher(consoleNoyau(), "Bonjour le monde !\n");
   
}   /* _start */

