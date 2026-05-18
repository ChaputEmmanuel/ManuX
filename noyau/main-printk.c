/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include "manux/ascii.h"
#include "manux/ecran.h"
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>

void startManuX()
{
   uint8_t fond, texte;

   // Initialisation de la console noyau
   consoleInitialisation();

   // Un petit message
   printk("Printk dit bonjour !\n");

   // En couleurs
   printk("%c[34m  Bleu\n", ASCII_ESC);
   printk("%c[37m  Blanc\n", ASCII_ESC);
   printk("%c[31m  Rouge\n", ASCII_ESC);
   printk("%c[0m", ASCII_ESC);

   for (texte = 30; texte <= 37; texte ++){
      for (fond = 40; fond <= 107; fond ++) {
         printk("%c[%d;%dH", ASCII_ESC, (texte - 25), 4*(fond-39));
         printk("%c[%dm", ASCII_ESC, texte);
         printk("%c[%dm", ASCII_ESC, fond);
	 printk("%d", texte);
         if (fond == 47) {fond = 99;} // J'ai honte !
      }
   }
   for (texte = 90; texte <= 97; texte ++){
      for (fond = 40; fond <= 107; fond ++) {
         printk("%c[%d;%dH", ASCII_ESC, (texte - 85), 4*(fond-39));
         printk("%c[%dm", ASCII_ESC, texte);
         printk("%c[%dm", ASCII_ESC, fond);
	 printk("%d", texte);
         if (fond == 47) {fond = 99;} // J'ai honte !
      }
   }
   
   // Monter et descendre
   printk("%c[44mFond bleu\n", ASCII_ESC);
   printk("%c[42mFond vert\n", ASCII_ESC);

   // Changement de fond
   printk("%c[3FX", ASCII_ESC);
     
   // Positionnement absolu
   printk("%c[12;40H+", ASCII_ESC);

}   /* startManuX */


