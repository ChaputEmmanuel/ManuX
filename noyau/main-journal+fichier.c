/**
 * @brief : exemple de noyau utilisant le journal via l'interface fichiers
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/fichier.h>
#include <manux/journal.h>

void startManuX()
{
   INoeud  iNoeudConsole;  // Le INoeud qui décrit la console
  
   // Initialisation de la console noyau
   consoleInitialisation(&iNoeudConsole);

   printk("Pouet pouet !!!\n");
   
   // Initialisation du journal
   journalInitialiser(&iNoeudConsole);
   
   // Un petit message
   printk("Printk dit bonjour via le journal !\n");
  
}   /* startManuX */


