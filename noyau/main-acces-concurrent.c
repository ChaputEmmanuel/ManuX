/**
 * @file main-acces-concurrent.c
 * @brief Un exemple pitoyable de début de noyau.
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>

extern void init(); // Faire un init.h

void _startManuX()
{
  // Initialisation de la console noyau
   consoleInitialisation();

   // Un petit message
   printk("Tests d'acces concurrents, ...\n");

   init();
}   /* _startManuX */


