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

#include <manux/console.h>
#include <manux/io.h>
#include <manux/irq.h>           /* autoriserIRQ(IRQTimer) */
#include <manux/memoire.h>       /* NULL, allouerPage */
#include <manux/atomique.h>      /* Pour le verrou sur le scheduler */
#include <manux/printk.h>        /* printk() */
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

void basculerTache()
{
   Tache * tachePrecedente = tacheEnCours;

   do {
      if (tacheEnCours != NULL) { /* WARNING on doit pouvoir s'en passer */
#ifdef DEBUG_MANUX_SCHEDULER
         printk("[SCHED] On quitte la tache %d de TSS %x, ...\n",
		tacheEnCours->numero, tacheEnCours->indiceTSSDescriptor);
#endif
         insererCelluleTache(&listeTaches,
	                     tacheEnCours,
                             (CelluleTache*)tacheEnCours+sizeof(Tache));
#ifdef DEBUG_MANUX_SCHEDULER
      } else {
         printk("[SCHED] Pas de tache a quitter !\n");
#endif
      }

      tacheEnCours = extraireTache(&listeTaches);
   } while ((tacheEnCours == NULL) || (tacheEnCours->etat != Tache_Prete));
      //   } while ((tacheEnCours != NULL) && (tacheEnCours->etat != Tache_Prete));

#ifdef DEBUG_MANUX_SCHEDULER
   printk("[SCHED] ... pour la tache %d de TSS %x\n",
		tacheEnCours->numero, tacheEnCours->indiceTSSDescriptor);
#endif
   //     while (1){};

   /* WARNING on doit pouvoir se passer du test suivant */
   if ((tacheEnCours != NULL) && (tacheEnCours != tachePrecedente)){
#ifdef DEBUG_MANUX_SCHEDULER
      printk("[SCHED] On passe a la tache %d de TSS %x (cs = %x)\n",
	     tacheEnCours->numero,
	     tacheEnCours->indiceTSSDescriptor,
	     tacheEnCours->tss.CS);
#endif

      basculerVersTache(tacheEnCours);

#ifdef DEBUG_MANUX_SCHEDULER
   } else {
      printk("[SCHED] Pas de nouvelle tache !\n");
#endif
   }
}

void scheduler()
/*
 * Le "main" du scheduler
 */
{
   CelluleTache * celluleTache;

   printk("Scheduler le mal nomme, ...\n");

   while(1) {
      if (afficheEtatSystemeDemande) {
	 printk("\n-------------------------<SCHEDULER t = %d>----------------------------\n", nbTicks);
         printk("Nombre de taches : %d\n", numeroProchaineTache);
         afficheEtatSystemeDemande = FALSE;
	 for (celluleTache = listeTaches.tete;
	      celluleTache != NULL;
	      celluleTache = celluleTache->suivant){
	   printk("[%d]  %s\n",
		  celluleTache->tache->numero,
		  (celluleTache->tache->etat == Tache_En_Cours)?"c":((celluleTache->tache->etat == Tache_Prete)?"p":"b"));
	 }
         printk("\n-------------------------------------------------------------------------------\n");

      }

#ifdef CONSOLES_VIRTUELLES
      /* Basculement entre les consoles virtuelles */
      /* WARNING, c'est sûrement pas le meilleur endroit ... */
      if (basculeConsoleDemandee) {
         basculeConsoleDemandee = FALSE;
         printk("\n\nBascule console ...\n\n");
         basculerVersConsoleSuivante();
      }
#endif
      basculerTache();
   }
}

void initialiserScheduler()
{
   numeroProchaineTache = 0;
   
   /* Initialisation de la liste des taches en cours */
   initialiserListeTache(&listeTaches);
   
   /* Création d'une tâche pour le fil actuel */
   ordonnancerTache(NULL, TRUE);

   /* Initialisation de la tache scheduler */
   ordonnancerTache(scheduler, FALSE);

   //while(1){};
}

void ordonnancerTache(CorpsTache corpsTache, booleen nouvelleConsole)
{
   Tache   * tache;
   
   Console * cons = NULL;
   void    * page;

#ifdef CONSOLES_VIRTUELLES
   //Nouvelle console ? 
   if (nouvelleConsole) {
      page = allouerPageSysteme();  // WARNING, gérer erreur
      cons = (Console *)page;
      initialiserConsole(cons, page + sizeof(Console)); // WARNING ! il faut que ça tienne !
      printk("Console 0x%x creee\n", cons);
   }
#endif
   
   /* Création de la tache */
   tache = creerTache(corpsTache, cons);
      printk("Tache %d creee de TSSdesc 0x%x et tss=0x%x\n",
	  tache->numero,
	  tache->indiceTSSDescriptor,
	  &(tache->tss));
   
   /* On insère la nouvelle tache à la fin de la liste */
   if (corpsTache) {
      insererCelluleTache(&listeTaches,
                          tache,
                          (CelluleTache*)tache+sizeof(Tache));
      printk("Tache inseree\n");

   } else {
      /* Cas particulier de la première tâche : */
      /*   . on charge son task register ;      */
      ltr(tache->indiceTSSDescriptor);
      /*   . et on la déclare comme en cours.   */
      tacheEnCours = tache;
   }
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
