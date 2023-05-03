/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>

void startManuX()
{
   // Initialisation de la console noyau
   consoleInitialisation();

   // Affichage d'un message
   consoleNoyauAfficher("La console vous salue ...\n");

   while (1){asm( "hlt" );};   // A remplacer par un halt() 
}   /* _startManuX */

