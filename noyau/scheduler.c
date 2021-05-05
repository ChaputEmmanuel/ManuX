/*----------------------------------------------------------------------------*/
/*      Implantation des fonctions de base du scheduler.                      */
/*                                                                            */
/* A voir : le paramètre "nouvelleConsole" n'a pas de sens s'il n'y a pas de  */
/* consoles virtuelles, à supprimer dans ce cas ? Ca rendra le code moins     */
/* lisible.                                                                   */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/scheduler.h>

#define DEBUG_MANUX_SCHEDULER

#include <manux/errno.h>
#include <manux/console.h>
#include <manux/io.h>
#include <manux/irq.h>           /* autoriserIRQ(IRQTimer) */
#include <manux/memoire.h>       /* NULL, allouerPage */
#include <manux/atomique.h>      /* Pour le verrou sur le scheduler */
#include <manux/printk.h>        /* printk() */
#include <manux/debug.h>         /* debug() paniqueNoyau() */
#include <manux/interruptions.h> /* nbTicks */
#include <manux/i386.h>          /* ltr */
#include <manux/appelsysteme.h>  /* console() */

extern void lanceTacheInitiale();
extern TacheID numeroProchaineTache ;
/*
 * Lancement d'une tâche dont l'index du TSS est passé en paramètre
 */

/*
 * Une variable globale permettant d'identifier la tache du scheduler
 */
Tache * tacheScheduler;

Atomique schedulerEnCours = 0;

/*
 * Ce qui suit n'est pas trés joli, mais ça ne devrait pas rester.
 */
booleen basculeConsoleDemandee = FALSE;
booleen afficheEtatSystemeDemande = TRUE;

/*
 * Le scheduler est-il en cours d'exécution ?
 */
Atomique verrouScheduler;

/*
 * La liste des tâches en attente sur le système
 */
ListeTache listeTaches;

/*
 * La tache en cours
 */
Tache * tacheEnCours = NULL;

void setFrequenceTimer(int freqHz)
{
   uint16 decompte;

   decompte = 1193200 / freqHz;

   /* On initialise la fréquence du timer 0 WARNING a rendre plus propre */
   outb(0x43, 0x36);
   outb(0x40, decompte & 0xFF);
   outb(0x40, (decompte >> 8) & 0xFF);
}

void ordonnanceur()
{
   Tache * tachePrecedente = tacheEnCours;

   do {
      if (tacheEnCours != NULL) { /* WARNING on doit pouvoir s'en passer */
         printk_debug(DBG_KERNEL_ORDON, "On quitte la tache %d de TSS %x, ...\n",
		tacheEnCours->numero, tacheEnCours->indiceTSSDescriptor);
         insererCelluleTache(&listeTaches,
	                     tacheEnCours,
                             (CelluleTache*)tacheEnCours+sizeof(Tache));
      } else {
         printk_debug(DBG_KERNEL_ORDON, "Pas de tache a quitter !\n");
      }

      tacheEnCours = extraireTache(&listeTaches);
   } while ((tacheEnCours == NULL) || (tacheEnCours->etat != Tache_Prete));

   /* WARNING on doit pouvoir se passer du test suivant */
   if ((tacheEnCours != NULL) && (tacheEnCours != tachePrecedente)){
      printk_debug(DBG_KERNEL_ORDON, "On passe a la tache %d de TSS 0x%x \n",
	     tacheEnCours->numero,
	     tacheEnCours->indiceTSSDescriptor);

      basculerVersTache(tacheEnCours);

   } else {
      printk_debug(DBG_KERNEL_ORDON, "Pas de nouvelle tache !\n");
   }
}

void scheduler()
/*
 * Le "main" du scheduler
 */
{
   CelluleTache * celluleTache;

   printk("Pouet\n");
   printk_debug(DBG_KERNEL_ORDON, "Scheduler le mal nomme, ...\n");

   while(1) {
      if (afficheEtatSystemeDemande) {
	 printk("\n-------------------------<SCHEDULER t = %d>----------------------------\n", nbTicks);
         printk("Num prochaine tache : %d\n", numeroProchaineTache);
         afficheEtatSystemeDemande = FALSE;
	    printk("[num]  etat   tache      console   tss     ldt\n");
	 for (celluleTache = listeTaches.tete;
	      celluleTache != NULL;
	      celluleTache = celluleTache->suivant){
            printk("[  %d]  %s      0x%x   0x%x  0x%x     0x%x\n",
		  celluleTache->tache->numero,
		   (celluleTache->tache->etat == Tache_En_Cours)?"c":((celluleTache->tache->etat == Tache_Prete)?"p":"b"),
 		  celluleTache->tache,
                  celluleTache->tache->console,
		   celluleTache->tache->tss,
		   celluleTache->tache->ldt);
	 }
         printk("\n-------------------------------------------------------------------------------\n");
      }

#ifdef CONSOLES_VIRTUELLES
      /* Basculement entre les consoles virtuelles */
      /* WARNING, c'est sûrement pas le meilleur endroit ... */
      if (basculeConsoleDemandee) {
         basculeConsoleDemandee = FALSE;
         basculerVersConsoleSuivante();
      }
#endif
      ordonnanceur();
   }
}

void initialiserScheduler()
{
  /* Les valeurs initiales */
  
   numeroProchaineTache = 1;
   tacheEnCours = NULL;
   
   /* Initialisation de la liste des taches en cours */
   initialiserListeTache(&listeTaches);

   /* Initialisation de la tache "scheduler" */
   if (ordonnancerTache(scheduler, FALSE)  < 0) {
      paniqueNoyau("impossible de creer la seconde tache !\n");
   }
   
   /* Création d'une tâche pour le fil actuel */
   if (ordonnancerTache(NULL, TRUE) < 0) {
      paniqueNoyau("impossible de creer la premiere tache !\n");
   }
}

/*
 * Insertion d'une nouvelle tâche dans l'ordonnanceur. La valeur
 * retournée est l'id de la tâche ou un code d'erreur.
 */
TacheID ordonnancerTache(CorpsTache corpsTache, booleen nouvelleConsole)
{
   Tache   * tache;

   Console * cons = consoleNoyau(); // En l'absence de consoles virtuelles

#ifdef CONSOLES_VIRTUELLES
   void    * page;

   //Nouvelle console ? 
   if (nouvelleConsole) {
      page = allouerPageSysteme();  // WARNING, gérer erreur
      cons = (Console *)page;
      if (page != NULL) {
         initialiserConsole(cons, page + sizeof(Console)); // WARNING ! il faut que ça tienne !
      } else {
  	 printk_debug(DBG_KERNEL_ERREUR, "Impossible de creer une nouvelle console\n");
         assert(tacheEnCours != NULL);
         cons = tacheEnCours->console;
      }
   } else {
      if (tacheEnCours != NULL) {
         cons = tacheEnCours->console;
      } else { // Pour la première a priori
         cons = consoleNoyau();
      }
   }
#endif
   
   /* Création de la tache */
   tache = creerTache(corpsTache, cons);
   if (tache == NULL) {
      return -ENOMEM;
   }
   
   /* On insère la nouvelle tache à la fin de la liste */
   if (corpsTache) {
      insererCelluleTache(&listeTaches,
                          tache,
                          (CelluleTache*)tache+sizeof(Tache));
      printk_debug(DBG_KERNEL_TACHE, "Tache inseree\n");

   } else {
      /* Cas particulier de la première tâche : */
      /*   . on charge son task register ;      */
      ltr(tache->indiceTSSDescriptor);
      /*   . et on la déclare comme en cours.   */
      tacheEnCours = tache;
   }
   return tache->numero;
}

int AS_numeroTache()
{
   return (int)tacheEnCours->numero;
}

/*
 * Le nom de la fonction suivante est probablement à changer, comme celui
 * de la précédente d'ailleurs.
 */
uint32 AS_console()
{
   if (schedulerEnCours) {
      return (uint32)tacheScheduler->console;
   } else if (tacheEnCours) {
      return (uint32)tacheEnCours->console;
   } else {
      return (uint32) NULL;
   }
}

/*
 * Implantation de l'appel système d'invocation de l'ordonnanceur
 */
int sys_basculerTache(ParametreAS as)
{
   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "Num %d, tache = 0x%x\n", tacheEnCours->numero, tacheEnCours);
   ordonnanceur();
   printk_debug(DBG_KERNEL_ORDON, "Retour d'ordo vers %d, tache = 0x%x\n", tacheEnCours->numero, tacheEnCours);
   return 0;
}

TacheID sys_creerTache(ParametreAS as, CorpsTache corpsTache, booleen shareConsole)
{
   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "corpsTache = 0x%x, share=%d\n", corpsTache, shareConsole);
   return ordonnancerTache(corpsTache, !shareConsole);
}

