/**
 * @file  : main-parametres.c
 * @brief : 
 *
 *                                                  (C) Manu Chaput 2000-2024 
 */
#include <manux/config.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/bootloader.h>
#include <manux/base-de-parametres.h>

#include <manux/debug.h>

void startManuX()
{
   // Initialisation de la console noyau
   consoleInitialisation();

   bootloaderInitialiser();

   // On initialise lla base de données
   baseDeParametresInitialiser();

   manuXAjouterParametre(typeParametreU32, (char*)0xFFFFFFFF, "systeme", "debug", "mask");

}   /* startManuX */


