/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/console.h>
#include <manux/clavier.h>
#include <manux/tache.h>
#include <manux/horloge.h>        // initialiserHorloge
#include <manux/scheduler.h>
#include <manux/interruptions.h>
#include <manux/intel-8259a.h>
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

void _start(InfoSysteme * infoSysteme,
	    uint32_t adresseDebutManuX,
	    uint32_t adresseFinManuX)
{
#ifdef MANUX_RAMDISK
   uint32_t adresseRamdisk = infoSysteme->adresseRamdiskHi * 65536
                         + infoSysteme->adresseRamdiskLo;
#endif
   union {
      uint32_t registres[3];
      char     caracteres[13];
   } descriptionProc;

   i8259aInit(MANUX_INT_BASE_IRQ);

   /* Initialisation de la console noyau */
   console = consoleInit();

   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();

   /* Initialisation du journal */
#ifdef MANUX_JOURNAL
   initialiserJournal(console);
#endif

   //basculerConsole();
   descriptionProcesseur(0, descriptionProc.registres);
   descriptionProc.caracteres[12] = 0;

   printk_debug(DBG_KERNEL_START, "32 bit ManuX running on a '%s' ...\n",
		descriptionProc.caracteres);

#ifdef MANUX_GESTION_MEMOIRE
   /* Affichage de la mémoire disponible */
   printk_debug(DBG_KERNEL_START, "Memoire : %d + %d Ko\n", infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);

   //   while(1){};
   /* Initialisation de la gestion mémoire */
   printk_debug(DBG_KERNEL_START, "Initialisation memoire ...\n");
   initialiserMemoire(infoSysteme->memoireDeBase,
		      infoSysteme->memoireEtendue,
		      adresseDebutManuX,
		      adresseFinManuX);
   printk_debug(DBG_KERNEL_START, "Memoire initialisee\n");
#endif

   /* Initialisation de la pagination */
#ifdef MANUX_PAGINATION
   printk_debug(DBG_KERNEL_START, "Initialisation pagination ...\n");
   initialiserPagination(infoSysteme->memoireEtendue);
   printk_debug(DBG_KERNEL_START, "Paginiation initialisee\n");
#endif
   
   /* Initialisation de la table des appels système*/
#ifdef MANUX_APPELS_SYSTEME
   printk_debug(DBG_KERNEL_START, "Initialisation appels systeme ...\n");
   initialiserAppelsSysteme();
   printk_debug(DBG_KERNEL_START, "Appels systeme initialises\n");
#endif

   /* Initialisation de la gestion des systèmes de fichiers */
#ifdef MANUX_FS
   printk_debug(DBG_KERNEL_START, "Initialisation du systeme de fichiers ...\n");
   sfInitialiser();
   printk_debug(DBG_KERNEL_START, "Systeme de fichiers initialise\n");
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

   /*
   printk_debug(DBG_KERNEL_START, "Console noyau = 0x%x\n", consoleNoyau());
   printk_debug(DBG_KERNEL_START, "Adresse de _start : 0x%x\n", _start);
   */
   
   /* Initialisation de la gestion des processus */
#ifdef MANUX_TACHES
   printk_debug(DBG_KERNEL_START, "Initialisation du scheduler ...\n");
   initialiserScheduler();
   printk_debug(DBG_KERNEL_START, "Scheduler initialise\n"); 
#endif

   printk_debug(DBG_KERNEL_START, "Initialisation de l'horloge ...\n");
   initialiserHorloge();
   printk_debug(DBG_KERNEL_START, "Horloge initialisee\n");
   
   printk_debug(DBG_KERNEL_START, "Le noyau va de 0x%x a 0x%x\n",
		adresseDebutManuX, adresseFinManuX);

   /*
   //afficher le masque des PIC ?
   uint8_t c, m;
   inb(0x20, c);
   inb(0x21, m);
   printk_debug(DBG_KERNEL_START, "Les masques tombent : 0x%x - 0x%x\n", c, m);
   inb(0xa0, c);
   inb(0xa1, m);
   printk_debug(DBG_KERNEL_START, "Les masques tombent : 0x%x - 0x%x\n", c, m);
   // __asm__ ("int %0\n" : :"N"(28));
   */
   init();
}   /* _start */

