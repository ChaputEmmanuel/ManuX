/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>
#include <manux/clavier.h>
#include <manux/debug.h>
#include <manux/tache.h>
#include <manux/scheduler.h>
#include <manux/interruptions.h>
#include <manux/irq.h>
#include <manux/io.h>
#include <manux/segment.h>
#include <manux/memoire.h>
#include <manux/atomique.h>
#include <manux/appelsysteme.h>
#include <manux/ramdisk.h>
#include <manux/printk.h>
#include <manux/limites.h>
#include <manux/pagination.h>
#include <manux/string.h>        /* bcopy() */
#ifdef MANUX_JOURNAL
#   include <manux/journal.h>       /* initialiserJournal() */
#endif
#include <manux/fichier.h>

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
   uint32 adresseRamdisk = infoSysteme->adresseRamdiskHi * 65536
                         + infoSysteme->adresseRamdiskLo;

   //int i;

   interdireIRQ(IRQTimer);
   console = consoleInit();
   
   /* Initialisation du journal */
#ifdef MANUX_JOURNAL
   initialiserJournal(console);
#endif
   
   //basculerConsole();
   printk("32 bit ManuX running ...\n");

   /* Affichage de la mémoire disponible */
   printk("Memoire : %d + %d Ko\n", infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);

   /* Initialisation de la gestion mémoire */
   printk("Initialisation memoire : ");
   initialiserMemoire(infoSysteme->memoireEtendue);
   printk("OK\n");

   /* Initialisation de la pagination */
   printk("Initialisation pagination : ");
   initialiserPagination(infoSysteme->memoireEtendue);
   printk("OK\n");

   /* Initilisation des descripteurs de segments */
   printk("Chargement GDT : ");
   initialiserGDT();
   printk("OK\n");

   /* Initialisation de la table des interruptions */
   printk("Chargement IDT : ");
   initialiserIDT();
   printk("OK\n");

   /* Initialisation de la table des appels système*/
   printk("Initialisation appels systeme : ");
   initialiserAppelsSysteme();
   printk("OK\n");

   printk("Initialisation du systeme de fichiers : ");
   sfInitialiser();
   printk("OK\n");

   /* Initialisation du ramdisk */
   if (infoSysteme->tailleRamdisk > 0) {
      printk("Initialisation du ramdisk (a %d de taille %d Ko) : ",
	     adresseRamdisk, infoSysteme->tailleRamdisk);
      initialiserRamDisk(adresseRamdisk, infoSysteme->tailleRamdisk);
      printk("OK\n");
   }
     
   /* Initialisation du clavier */
   printk("Initialisation du clavier : ");
   initialiserClavier();
   printk("OK\n");

   /* Initialisation de la fréquence du timer */
   setFrequenceTimer(18);
   autoriserIRQ(IRQTimer);

   /* Initialisation de la gestion des processus */

   printk("Initialisation du scheduler : ");
   initialiserScheduler();
   printk("OK\n"); 

   sti();

   printk_debug(DBG_KERNEL_START, "C'est parti les amis ...\n");
   //   printk_debug(DBG_KERNEL_START, "Adresse de __start : %x\n", _start);

   init();
}   /* main */

