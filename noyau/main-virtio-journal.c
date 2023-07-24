/**
 * @brief : exemple de noyau utilisant le journal via l'interface fichiers
 *
 *                                                     (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/debug.h>
#include <manux/console.h>
#include <manux/printk.h>
#include <manux/fichier.h>
#include <manux/journal.h>
#include <manux/pci.h>
#include <manux/virtio-console.h>

void startManuX()
{
   INoeud  iNoeudConsole;  // Le INoeud qui décrit la console

   INoeud iNoeudVirtioConsole; 
   Fichier fichierVirtioConsole;
  
   // Initialisation de la console noyau
   consoleInitialisation(&iNoeudConsole);

   printk("Bonjour !!!\n");
   
   // Initialisation du journal
   journalInitialiser(&iNoeudConsole);

   // Initialisation du bus PCI
   printk_debug(DBG_KERNEL_START, "Initialisation du bus PCI ...\n");
   PCIEnumerationDesEquipements();
   printk_debug(DBG_KERNEL_START, "Bus PCI initialise...\n");

   // Initialisation de la console virtio
   printk_debug(DBG_KERNEL_START, "Initialisation de virtio console ...\n");
   if (virtioConsoleInitialisation(&iNoeudVirtioConsole) == 0/*ESUCCES*/) {
      ouvrirFichier(&iNoeudVirtioConsole, &fichierVirtioConsole);
      journalAffecterFichier(&fichierVirtioConsole);
   }
   printk_debug(DBG_KERNEL_START, "Virtio console initialise...\n");
   
   // Un petit message
   printk("Printk dit bonjour via le journal !\n");
  
}   /* startManuX */


