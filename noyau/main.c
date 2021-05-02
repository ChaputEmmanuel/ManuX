/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>
#include <manux/clavier.h>
#include <manux/tache.h>
#include <manux/scheduler.h>
#include <manux/interruptions.h>
#include <manux/irq.h>
#include <manux/io.h>
#include <manux/segment.h>
#include <manux/memoire.h>
#include <manux/atomique.h>
#include <manux/appelsysteme.h>
#ifdef MANUX_RAMDISK
#   include <manux/ramdisk.h>
#endif
#include <manux/printk.h>
#include <manux/debug.h>
#include <manux/limites.h>
#ifdef MANUX_PAGINATION
#   include <manux/pagination.h>
#endif
#include <manux/string.h>        /* bcopy() */
#ifdef MANUX_JOURNAL
#   include <manux/journal.h>       /* initialiserJournal() */
#endif
#ifdef MANUX_FS
#   include <manux/fichier.h>
#endif

extern void init(); // Faire un init.h

/*
 * Structure passée en paramètre par la phase d'init
 * (cf init-manux.nasm)
 */
typedef struct _InfoSysteme {
   uint16 memoireDeBase;   // En Ko
   uint16 memoireEtendue;  // En Ko
   uint16 tailleRamdisk;   // En Ko (0 si pas de ramdisk)
   uint16 adresseRamdiskLo;// WARNING, pourquoi cet entremellage ?
   uint16 adresseRamdiskHi;
} InfoSysteme;

Console * console; // La console système
 
void _start(InfoSysteme * infoSysteme)
{
#ifdef MANUX_RAMDISK
   uint32 adresseRamdisk = infoSysteme->adresseRamdiskHi * 65536
                         + infoSysteme->adresseRamdiskLo;
#endif
   
   /*
    * Ici, c'est surtout pour le côté artistique car si l'assertion
    * est fausse, le co de ne sera pas exécuté :-/
    */
   assert(_start == (void *)0x20000);
   
   interdireIRQ(IRQTimer);

   /* Initialisation de la console noyau */
   console = consoleInit();
   
   /* Initialisation du journal */
#ifdef MANUX_JOURNAL
   initialiserJournal(console);
#endif
   
   //basculerConsole();
   printk_debug(DBG_KERNEL_START, "32 bit ManuX running ...\n");

   /* Affichage de la mémoire disponible */
   printk_debug(DBG_KERNEL_START, "Memoire : %d + %d Ko\n", infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);

   /* Initialisation de la gestion mémoire */
   printk_debug(DBG_KERNEL_START, "Initialisation memoire ...\n");
   initialiserMemoire(infoSysteme->memoireEtendue);
   printk_debug(DBG_KERNEL_START, "Memoire initialisee\n");

   /* Initialisation de la pagination */
#ifdef MANUX_PAGINATION
   printk_debug(DBG_KERNEL_START, "Initialisation pagination ...\n");
   initialiserPagination(infoSysteme->memoireEtendue);
   printk_debug(DBG_KERNEL_START, "Paginiation initialisee\n");
#endif
   
   /* Initilisation des descripteurs de segments */
   printk_debug(DBG_KERNEL_START, "Chargement GDT, ...\n");
   initialiserGDT();
   printk_debug(DBG_KERNEL_START, "GDT chargee\n");

   /* Initialisation de la table des interruptions */
   printk_debug(DBG_KERNEL_START, "Chargement IDT ...\n");
   initialiserIDT();
   printk_debug(DBG_KERNEL_START, "IDT chargee\n");

   /* Initialisation de la table des appels système*/
   printk_debug(DBG_KERNEL_START, "Initialisation appels systeme ...\n");
   initialiserAppelsSysteme();
   printk_debug(DBG_KERNEL_START, "Appels systeme initialises\n");

#ifdef MANUX_FS
   printk_debug(DBG_KERNEL_START, "Initialisation du systeme de fichiers : ");
   sfInitialiser();
   printk_debug(DBG_KERNEL_START, "OK\n");
#endif
   
   /* Initialisation du ramdisk */
#ifdef MANUX_RAMDISK
   if (infoSysteme->tailleRamdisk > 0) {
      printk_debug(DBG_KERNEL_START, "Ramdisk (a %d de taille %d Ko) ...\n",
	     adresseRamdisk, infoSysteme->tailleRamdisk);
      initialiserRamDisk(adresseRamdisk, infoSysteme->tailleRamdisk);
      printk_debug(DBG_KERNEL_START, "Ramdisk initilise\n");
   }
#endif
   
   /* Initialisation du clavier */
   printk_debug(DBG_KERNEL_START, "Initialisation du clavier ...\n");
   initialiserClavier();
   printk_debug(DBG_KERNEL_START, "Clavier initalise\n");

   /* Initialisation de la fréquence du timer */
   setFrequenceTimer(18);
   autoriserIRQ(IRQTimer);

   /* Initialisation de la gestion des processus */

   printk_debug(DBG_KERNEL_START, "Initialisation du scheduler ...\n");
   initialiserScheduler();
   printk_debug(DBG_KERNEL_START, "Scheduler initialise\n"); 

   printk_debug(DBG_KERNEL_START, "C'est parti mon kiki, ...\n");
   printk_debug(DBG_KERNEL_START, "Adresse de __start : %x\n", _start);
   //   halt();
   init();
}   /* main */

