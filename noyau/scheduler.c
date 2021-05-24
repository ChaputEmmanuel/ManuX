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
#include <manux/interruptions.h> /* nbTopHorloge */
#include <manux/i386.h>          /* ltr */
#include <manux/appelsysteme.h>  /* console() */
#include <manux/temps.h>         /* secondesDansTemps */

extern void lanceTacheInitiale();
extern TacheID numeroProchaineTache ;

/*
 * Une variable globale permettant d'identifier la tache du scheduler
 */
Tache * tacheScheduler;

Temps dateDernierOrdonnancement;  // En nbTopHorloge
Atomique schedulerEnCours = 0;

/*
 * Ce qui suit n'est pas trés joli, mais ça ne devrait pas rester.
 */
booleen basculeConsoleDemandee = FALSE;
booleen afficheEtatSystemeDemande = FALSE;

booleen basculerTacheDemande = TRUE; // WARNING à virer ? C'est pour
				     // faire du "pas à pas"

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

void setFrequenceTimer(uint16_t freqHz)
{
   uint16_t decompte;

   decompte = 1193200 / freqHz;

   /* On initialise la fréquence du timer 0 WARNING a rendre plus propre */
   outb(0x43, 0x34); // wAS 36
   outb(0x40, decompte & 0xFF);
   outb(0x40, (decompte >> 8) & 0xFF);
}

/*
 * Le coeur de l'ordonnanceur. C'est cette fonction qui détermine la
 * prochaine tâche à exécuter.
 */
void ordonnanceur()
{
   Tache * tachePrecedente = tacheEnCours;

#ifdef CONSOLES_VIRTUELLES
   /* Basculement entre les consoles virtuelles */
   if (basculeConsoleDemandee) {
      basculeConsoleDemandee = FALSE;
      basculerVersConsoleSuivante();
   }
#endif

   /* (1) On suspend la tâche en cours */
   assert(tacheEnCours != NULL);
   insererCelluleTache(&listeTaches,
                       tacheEnCours,
                       (CelluleTache*)tacheEnCours+sizeof(Tache));

   /* On cumule le temps d'écution dont elle vient de profiter */
   tacheEnCours->tempsExecution += (nbTopHorloge - dateDernierOrdonnancement);

   tacheEnCours->etat = Tache_Prete;    // On n'est pas là volontairement
                                        // WARNING : il faudra faire gaffe en cas de pause

   /* (2) on cherche la tâche suivante */
   /* On prend la première tâche prête, il y en a au moins une : la dummy */
   do {
      tacheEnCours = extraireTache(&listeTaches);
   } while (tacheEnCours->etat != Tache_Prete); 

   tacheEnCours->etat = Tache_En_Cours;
 
   if (tacheEnCours != tachePrecedente){
      printk_debug(DBG_KERNEL_ORDON, "On passe a la tache %d de TSS 0x%x \n",
	     tacheEnCours->numero,
	     tacheEnCours->indiceTSSDescriptor);

      /* Une activation de plus pour elle */
      tacheEnCours->nbActivations++;

      /* On note la date pour pouvoir mesurer le temps dont elle va profiter */
      dateDernierOrdonnancement = nbTopHorloge;
      basculerVersTache(tacheEnCours);
   }
}

void afficherEtatUneTache(Tache * tache)
{
   printk("[  %d]  %s   %4d  %2d:%2d  0x%x   0x%x  0x%x \n",
       tache->numero,
         (tache->etat == Tache_En_Cours)?"c":((tache->etat == Tache_Prete)?"p":"b"),
          tache->nbActivations,
	  totalMinutesDansTemps(tache->tempsExecution),
	  secondesDansTemps(tache->tempsExecution),
          tache,
          tache->console,
          tache->ldt);
}

void afficherEtatTaches()
{
   CelluleTache * celluleTache;

   printk("\n-------------------------<SCHEDULER t = %d:%d (%d)>----------------------------\n",
	  totalMinutesDansTemps(nbTopHorloge),
	  secondesDansTemps(nbTopHorloge),
	  nbTopHorloge);
   printk("Num prochaine tache : %d\n", numeroProchaineTache);
   afficheEtatSystemeDemande = FALSE;
   printk("[num] et   nbAc  tpsEx     tache    console       ldt\n");
   afficherEtatUneTache(tacheEnCours);
   for (celluleTache = listeTaches.tete;
      celluleTache != NULL;
      celluleTache = celluleTache->suivant){
        afficherEtatUneTache(celluleTache->tache);
   }
   printk("\n-------------------------------------------------------------------------------\n");
}

void aDummyKernelTask()
/*
 * Le corps d'une tâche à exéctuer lorsqu'on n'a que ça à faire, ...
 */
{
   printk_debug(DBG_KERNEL_ORDON, "aDummyKernelTask running\n");

   while(1) {
      if (afficheEtatSystemeDemande) {
         afficherEtatTaches();
      }
 for (int i = 0; i<10000000; i+=1){asm("");};
 //      ordonnanceur();
   }
}

void initialiserScheduler()
{
   /* Les valeurs initiales */
   numeroProchaineTache = 1;
   tacheEnCours = NULL;
   dateDernierOrdonnancement = nbTopHorloge;
   
   /* Initialisation de la liste des taches en cours */
   initialiserListeTache(&listeTaches);

   /* Création d'une tâche pour le fil actuel */
   if (ordonnancerTache(NULL, TRUE) < 0) {
      paniqueNoyau("impossible de creer la premiere tache !\n");
   }

   /* Initialisation de la tache "aDummyKernelTask" */
   if (ordonnancerTache(aDummyKernelTask, FALSE)  < 0) {
      paniqueNoyau("impossible de creer la seconde tache !\n");
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
uint32_t AS_console()
{
   if (schedulerEnCours) {
      return (uint32_t)tacheScheduler->console;
   } else if (tacheEnCours) {
      return (uint32_t)tacheEnCours->console;
   } else {
      return (uint32_t) NULL;
   }
}

/*
 * Implantation de l'appel système d'invocation de l'ordonnanceur
 */
int sys_basculerTache(ParametreAS as)
{
   assert(tacheEnCours != NULL);
   ordonnanceur();

   return 0;
}

TacheID sys_creerTache(ParametreAS as, CorpsTache corpsTache, booleen shareConsole)
{
   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "corpsTache = 0x%x, share=%d\n", corpsTache, shareConsole);
   return ordonnancerTache(corpsTache, !shareConsole);
}

