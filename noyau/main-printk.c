/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>

void _startManuX()
{
  // Initialisation de la console noyau
   consoleInitialisation();

   // Un petit message
   printk("Printk dit bonjour !\n");
  
   while (1){};   // A remplacer par un halt() 
}   /* _startManuX */


