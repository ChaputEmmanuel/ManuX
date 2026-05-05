/**
 * @file noyau/dummy-task.c
 * @brief Implantation d'une tâche de fond du noyau, surtout utile
 * pour de l'affichage  pour le moment.
 *                                                     (C) Manu Chaput 2000-2026
 */
#include "manux/printk.h"
#include <manux/dummy-task.h>
#include <manux/debug.h>   // printk_debug
#ifdef MANUX_TACHES
#   include <manux/tache.h>
#endif // MANUX_TACHES
#include <manux/scheduler.h>
#ifdef MANUX_CONSOLE
#   include <manux/console.h>
#endif
#ifdef MANUX_CLAVIER_CONSOLE
#   include <manux/clavier.h>
#endif
#ifdef MANUX_AS_AUDIT
#   include <manux/appelsysteme.h>
#endif
#ifdef MANUX_KMALLOC_STAT
#   include <manux/kmalloc.h>    // kmallocAfficherStatistiques
#endif
#ifdef MANUX_VIRTIO_CONSOLE
#   include <manux/virtio-console.h> // A virer
#endif
#ifdef MANUX_VIRTIO_NET
#   include <manux/virtio-net.h> // A virer
#endif
#if defined(MANUX_EXCLUSION_MUTUELLE)
#   include <manux/exclusion-mutuelle.h> // exclusionsMutuellesAfficherEtat
#endif
#if defined(MANUX_CONDITION)
#   include <manux/condition.h>  // exclusionsMutuellesAfficherEtat
#endif

#ifdef MANUX_DEBUGMASK_VAR
void debugMasqueAfficher()
{
   printk("masqueDebugageConsole = 0x%x\n", masqueDebugageConsole);
   printk("masqueDebugageFichier = 0x%x\n", masqueDebugageFichier);
}
#endif // MANUX_DEBUGMASK_VAR

#ifdef MANUX_AS_AUDIT
/**
 * @brief Affichage sur la console des AS de chaque tâche
 */
void appelsSystemeAfficher()
{
   CelluleTache * celluleTache;

   printk("\nTache  | Appels Systeme (num:in/out)\n");
   printk("-------+----------------------------------------\n");
   for (celluleTache = listeToutesLesTaches.tete;
      celluleTache != NULL;
      celluleTache = celluleTache->suivant){
      printk("%3d    | ", celluleTache->tache->numero);

      for (int i=0; i < NB_MAX_APPELS_SYSTEME; i++) {
         if (celluleTache->tache->nbAppelsSystemeIn[i]) {
	    printk("%d:%d/%d ", i,
		   celluleTache->tache->nbAppelsSystemeIn[i],
		   celluleTache->tache->nbAppelsSystemeOut[i]);
         }
      }
      printk("\n");
   }
}
#endif  // MANUX_AS_AUDIT

#if  defined(MANUX_TACHES) \
 &&  defined(MANUX_SYNCHRONISATION) \
 &&  defined(MANUX_EXCLUSION_MUTUELLE) \
 && !defined(MANUX_REENTRANT)
/**
 * @brief Etat du verrou général
 */
void afficherEtatMutex()
{
   printk("\n-- Tache dans le noyau : %d \n-- Taches en attente : ", tacheDansLeNoyau);
   for (CelluleTache * celluleTache = verrouGeneralDuNoyau.tachesEnAttente.tete;
        celluleTache != NULL;
	celluleTache = celluleTache->suivant){
      printk("%d ", celluleTache->tache->numero);
   }
#ifdef MANUX_EXCLUSION_MUTUELLE_AUDIT
   printk("\n-- %d ent / %d sor\n", verrouGeneralDuNoyau.nbEntrees, verrouGeneralDuNoyau.nbSorties);
#endif // MANUX_EXCLUSION_MUTUELLE_AUDIT
}
#endif

#ifdef MANUX_CLAVIER_CONSOLE
/**
 * @brief Gestion du clavier pour la dummy
 */
void dummyTraiterClavier()
{
   Console * cons

#ifdef MANUX_CONSOLES_VIRTUELLES
     = tacheEnCours->console;
#else
     = consoleNoyau();     
#endif
   
   char c[1] ;
   int i;
   
   while (cons->nbCarAttente) {
      c[0] = 0;
      consoleLire(cons, c, 1);
      switch (c[0]) {
#ifdef MANUX_AS_AUDIT
         case 'a' :
            appelsSystemeAfficher();
         break;
#endif
         case 'c' :
	    for (i = 0; i < 24; i++)
	       printk("\n");
         break;
#ifdef MANUX_DEBUGMASK_VAR
         case 'd' :
            debugMasqueAfficher();
	 break;
#endif  // MANUX_DEBUGMASK_VAR
         case 'h' :
	   printk("c(lear screen)\nh(elp)\np(rocessus)\nm(emoire)\ni(nterruptions)\ns(ynchronisation)\n");
	 break;
         case 'i' :
            interruptionAfficher();
	 break;
         case 'p' :
            afficherEtatTaches();
	 break;
#if defined(MANUX_EXCLUSION_MUTUELLE_AUDIT) || defined(MANUX_CONDITION_AUDIT)
         case 's' :
#if defined(MANUX_EXCLUSION_MUTUELLE_AUDIT)
            exclusionsMutuellesAfficherEtat();
#endif
#if defined(MANUX_CONDITION_AUDIT)
            conditionsAfficherEtat();
#endif
	    break;
#endif
#if  defined(MANUX_TACHES) \
 &&  defined(MANUX_SYNCHRONISATION) \
 &&  defined(MANUX_EXCLUSION_MUTUELLE) \
 && !defined(MANUX_REENTRANT)
         case 'x' :
            afficherEtatMutex();
	 break;
#endif
         case 'm' :
#ifdef MANUX_KMALLOC_STAT
            kmallocAfficherStatistiques("");
#else
            printk(" Memoire : %d / %d pages allouees\n",
	    nombrePagesAllouees(), nombrePagesTotal());
#endif
	 break;
         default :
	   //            printk("Unknown [0x%x] pressed\n", c[0]);
         break;
      }
   }
}
#endif // MANUX_CLAVIER_CONSOLE

/**
 * @brief Le corps d'une tâche à exécuter lorsqu'on n'a que ça à faire, ...
 *
 * Cette tâche ne fait rien de très important, si ce n'est récupérer
 * les événements clavier sur la console noyau. Grâce à ça c'est elle
 * qui permet d'afficher quelques informations sur le système
 */
void aDummyKernelTask()
{
   while(1) {

#if defined(MANUX_EXCLUSION_MUTUELLE) && !defined(MANUX_REENTRANT)
      // Cette tâche passe sa vie dans le noyau, elle doit donc
      // acquérir le verrou si le noyau n'est pas réentrant.
      exclusionMutuelleEntrer(&verrouGeneralDuNoyau);
      assert(tacheDansLeNoyau == 0);
      tacheDansLeNoyau = tacheEnCours->numero;
#endif

      printk_debug(DBG_KERNEL_ORDON, "aDummyKernelTask running\n");

#ifdef MANUX_CLAVIER_CONSOLE
      dummyTraiterClavier();
#endif

#if defined(MANUX_EXCLUSION_MUTUELLE) && !defined(MANUX_REENTRANT)
      // Cette tâche passe sa vie dans le noyau, elle doit donc
      // rendre le verrou si le noyau n'est pas réentrant.
      tacheDansLeNoyau = 0;
      exclusionMutuelleSortir(&verrouGeneralDuNoyau);
#endif

#ifdef MANUX_TACHES
      ordonnanceur();
#endif
   }
}



