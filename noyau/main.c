/*----------------------------------------------------------------------------*/
/*      Un exemple pitoyable de début de noyau.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/errno.h>
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
#ifdef MANUX_STDLIB
#   include <manux/stdlib.h>
#endif
#ifdef MANUX_KMALLOC
#   include <manux/kmalloc.h>
#endif
#include <manux/atomique.h>
#include <manux/appelsysteme.h>
#ifdef MANUX_PCI
#   include <manux/pci.h>
#endif
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
#ifdef MANUX_RESEAU
#   ifdef MANUX_VIRTIO_NET
#      include <manux/virtio-net.h>
#   endif
#endif
#ifdef MANUX_VIRTIO_CONSOLE
#   include <manux/virtio-console.h>
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

#ifdef MANUX_VIRTIO_CONSOLE
INoeud iNoeudVirtioConsole;
Fichier fichierVirtioConsole;
#endif

/**
 * Configuration de la console
 */
INoeud  iNoeudConsole;  // Le INoeud qui décrit la console

#ifdef MANUX_KMALLOC
/* A mettre dans un fichier qui remplace init */
#define NB_ELEMENTS 20
#define NB_APPELS   500
void testerKmalloc()
{
   void * elements[NB_ELEMENTS] = {NULL,};
   int n, e;

   printk(PRINTK_DEBUGAGE ">>>>\n");
   for (e = 0; e<NB_ELEMENTS; e++){
      elements[e] = NULL;
   }
   kmallocAfficherStatistiques();
   for (n = 0; n < NB_APPELS; n++) {
      e = rand() % NB_ELEMENTS;
      /*  if (elements[e] == NULL) {*/
         elements[e] = kmalloc(rand() % 1024);
	 printk(PRINTK_DEBUGAGE"B %d (0x%x)\n", e, elements[e]);
	 /*      } else {
         printk(PRINTK_DEBUGAGE"A %d (0x%x)\n", e, elements[e]);
         kfree(elements[e]);
         elements[e] = NULL;
	 }*/
   }
   kmallocAfficherStatistiques();
   printk(PRINTK_DEBUGAGE "<<<<\n");
}

#endif // MANUX_KMALLOC

#ifdef MANUX_VIRTIO_CONSOLE
#define NB_LIGNES 2
void testerVirtioConsole()
{
   for (int n = 0; n < NB_LIGNES; n++) {
     printk(PRINTK_DEBUGAGE "[%3d] Une ligne de texte ...\n", n);
   }
}
#endif // MANUX_VIRTIO_CONSOLE

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

#ifdef MANUX_GESTION_MEMOIRE
   /* Affichage de la mémoire disponible */
   //   printk_debug(DBG_KERNEL_START, "Memoire : %d + %d Ko\n", infoSysteme->memoireDeBase, infoSysteme->memoireEtendue);

   /* Initialisation de la gestion mémoire */
   //   printk_debug(DBG_KERNEL_START, "Initialisation memoire ...\n");
   initialiserMemoire(infoSysteme->memoireDeBase,
		      infoSysteme->memoireEtendue,
		      adresseDebutManuX,
		      adresseFinManuX);
   //printk_debug(DBG_KERNEL_START, "Memoire initialisee\n");
#endif

   /* Initialisation de la console noyau */
   consoleInitialisation(&iNoeudConsole);

   i8259aInit(MANUX_INT_BASE_IRQ);

   /* Initilisation des descripteurs de segments */
   initialiserGDT();

   /* Initialisation de la table des interruptions */
   initialiserIDT();
   
   /* Initialisation du journal */
#ifdef MANUX_JOURNAL
    journalInitialiser(&iNoeudConsole);
#endif

   descriptionProcesseur(0, descriptionProc.registres);
   descriptionProc.caracteres[12] = 0;

   printk_debug(DBG_KERNEL_START, "32 bit ManuX running on a '%s' ...\n",
		descriptionProc.caracteres);

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
   
   /* Initialisation du bus PCI */
#ifdef MANUX_PCI
   printk_debug(DBG_KERNEL_START, "Initialisation du bus PCI ...\n");
   PCIEnumerationDesEquipements();
   printk_debug(DBG_KERNEL_START, "Bus PCI initialise...\n");
#endif

#ifdef MANUX_VIRTIO_CONSOLE
   printk_debug(DBG_KERNEL_START, "Initialisation de virtio console ...\n");
   if (virtioConsoleInitialisation(&iNoeudVirtioConsole) == ESUCCES) {
      ouvrirFichier(&iNoeudVirtioConsole, &fichierVirtioConsole);
      journalAffecterFichier(&fichierVirtioConsole);
   }
   printk_debug(DBG_KERNEL_START, "Virtio console initialise...\n");
#endif

   /* Initialisation du réseau */
#ifdef MANUX_RESEAU
   printk_debug(DBG_KERNEL_START, "Initialisation du reseau ...\n");
#   ifdef MANUX_VIRTIO_NET
   virtioNetInit();
#   endif
   printk_debug(DBG_KERNEL_START, "Reseau initialise\n");
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

//   printk_debug(DBG_KERNEL_START, "Initialisation de l'horloge ...\n");
   initialiserHorloge();
//   printk_debug(DBG_KERNEL_START, "Horloge initialisee\n");
   
   printk_debug(DBG_KERNEL_START, "Le noyau va de 0x%x a 0x%x\n",
   adresseDebutManuX, adresseFinManuX);

#ifdef MANUX_STDLIB_NON
   for (int n = 0; n < 50; n++)
     printk(PRINTK_DEBUGAGE "%d, ", rand());
#endif

#ifdef MANUX_VIRTIO_CONSOLE
   testerVirtioConsole();
#endif

#ifdef MANUX_KMALLOC
   printk(PRINTK_DEBUGAGE "Pouet\n");
   kmallocInitialisation();
   testerKmalloc();
#endif
   
   init();
}   /* _start */

