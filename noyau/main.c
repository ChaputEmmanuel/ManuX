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
   uint32_t flags;           // Pour compatibilité avec multiboot
   uint32_t memoireDeBase;   // En Ko
   uint32_t memoireEtendue;  // En Ko
   uint32_t tailleRamdisk;   // En Ko (0 si pas de ramdisk)
   uint32_t adresseRamdisk;
} InfoSysteme;

Console * console; // La console système

void _start(InfoSysteme * infoSysteme)
{
#ifdef MANUX_RAMDISK
   uint32_t adresseRamdisk = infoSysteme->adresseRamdiskHi * 65536
                         + infoSysteme->adresseRamdiskLo;
#endif

   interdireIRQ(IRQTimer);

   initialiserGDT();
   initialiserIDT();
   
   /* Initialisation de la console noyau */
   console = consoleInit();

   /* Initialisation du journal */
#ifdef MANUX_JOURNAL
   initialiserJournal(console);
#endif

   //basculerConsole();
   printk_debug(DBG_KERNEL_START, "32 bit ManuX running ...\n");

#ifdef MANUX_GESTION_MEMOIRE
   /* Affichage de la mémoire disponible */
   printk_debug(DBG_KERNEL_START, "Memoire : %d + %d Ko\n", infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);

   //   while(1){};
   /* Initialisation de la gestion mémoire */
   printk_debug(DBG_KERNEL_START, "Initialisation memoire ...\n");
   initialiserMemoire(infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);
   printk_debug(DBG_KERNEL_START, "Memoire initialisee\n");
#endif

   /* Initialisation de la pagination */
#ifdef MANUX_PAGINATION
   printk_debug(DBG_KERNEL_START, "Initialisation pagination ...\n");
   initialiserPagination(infoSysteme->memoireEtendue);
   printk_debug(DBG_KERNEL_START, "Paginiation initialisee\n");
#endif
   
   /* Initilisation des descripteurs de segments */
   printk_debug(DBG_KERNEL_START, "Chargement GDT, ...\n");
   //   initialiserGDT();
   printk_debug(DBG_KERNEL_START, "GDT chargee\n");

   /* Initialisation de la table des interruptions */
   printk_debug(DBG_KERNEL_START, "Chargement IDT ...\n");
   //   initialiserIDT();
   printk_debug(DBG_KERNEL_START, "IDT chargee\n");

   /* Initialisation de la table des appels système*/
#ifdef MANUX_APPELS_SYSTEME
   printk_debug(DBG_KERNEL_START, "Initialisation appels systeme ...\n");
   initialiserAppelsSysteme();
   printk_debug(DBG_KERNEL_START, "Appels systeme initialises\n");
#endif

   /* Initialisation de la gestion des systèmes de fichiers */
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

#ifdef MANUX_CLAVIER
   /* Initialisation du clavier */
   printk_debug(DBG_KERNEL_START, "Initialisation du clavier ...\n");
   initialiserClavier();
   printk_debug(DBG_KERNEL_START, "Clavier initalise\n");
#endif
   
   /* Initialisation de la fréquence du timer */
   setFrequenceTimer(MANUX_FREQUENCE_TIMER);
   autoriserIRQ(IRQTimer);

   /* Initialisation de la gestion des processus */

   printk_debug(DBG_KERNEL_START, "Console noyau = 0x%x\n", consoleNoyau());
   printk_debug(DBG_KERNEL_START, "A comparer a  = 0x%x\n", 0x000250a0);

#ifdef MANUX_TACHES
   printk_debug(DBG_KERNEL_START, "Initialisation du scheduler ...\n");
   initialiserScheduler();
   printk_debug(DBG_KERNEL_START, "Scheduler initialise\n"); 
#endif
   
   printk_debug(DBG_KERNEL_START, "Adresse de __start : 0x%x\n", _start);

   init();
}   /* _start */

