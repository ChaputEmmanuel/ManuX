/*----------------------------------------------------------------------------*/
/*      Implantation des fonctions de base du scheduler.                      */
/*                                                                            */
/* A voir : le paramètre "nouvelleConsole" n'a pas de sens s'il n'y a pas de  */
/* consoles virtuelles, à supprimer dans ce cas ? Ca rendra le code moins     */
/* lisible.                                                                   */
/*                                                  (C) Manu Chaput 2000-2025 */
/*----------------------------------------------------------------------------*/
#include <manux/scheduler.h>

#define DEBUG_MANUX_SCHEDULER
#define MANUX_DUMMY_TASK  // WARNING, à mettre ailleurs

#include <manux/errno.h>
#include <manux/console.h>
#include <manux/io.h>
#include <manux/memoire.h>       /* NULL, allouerPage */
#if defined(MANUX_SYNCHRONISATION)
#   include <manux/atomique.h>      /* Pour le verrou sur le scheduler */
#   if defined(MANUX_EXCLUSION_MUTUELLE)
#      include <manux/exclusion-mutuelle.h>
#   endif
#   if defined(MANUX_CONDITION)
#      include <manux/condition.h>
#   endif
#endif
#include <manux/printk.h>        /* printk() */
#include <manux/debug.h>         /* debug() paniqueNoyau() */
#include <manux/interruptions.h> /* nbTopHorloge */
#include <manux/i386.h>          /* ltr */
#include <manux/appelsysteme.h>  /* console() */
#include <manux/temps.h>         /* secondesDansTemps */
#ifdef MANUX_KMALLOC_STAT
#   include <manux/kmalloc.h>    // kmallocAfficherStatistiques
#endif
#ifdef MANUX_VIRTIO_CONSOLE
#   include <manux/virtio-console.h> // A virer
#endif
#ifdef MANUX_VIRTIO_NET
#   include <manux/virtio-net.h> // A virer
#endif

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

booleen basculerTacheDemande = TRUE; // WARNING à virer ? C'est pour
				     // faire du "pas à pas"

/**
 * @brief Identifiant de la tâche actuellement en cours d'exécution
 * dans le noyau (0 si aucune)
 *
 * C'est une façon d'assurer le fonctionnement d'un noyau non
 * réentrant.
 */
#if defined(MANUX_TACHES) && defined(MANUX_SYNCHRONISATION) && !defined(MANUX_REENTRANT)
ExclusionMutuelle verrouGeneralDuNoyau;
TacheID tacheDansLeNoyau = 0;
#endif

/**
 * @brief : La liste des tâches prêtes sur le système
 *
 * Ce sont les tâches prêtes à être exécutées. N'y figurent donc pas
 * la tâche en cours d'exécution ni les tâches en attente sur un outil
 * de synchronisation.
 */
ListeTache listeTachesPretes;

/**
 * @brief : La liste de toutes les tâches existant sur le système.
 */
ListeTache listeToutesLesTaches;

/**
 * @brief : La liste des tâches achevées.
 */
ListeTache listeTachesTerminees;

/**
 * @brief : La tâche en cours
 */
Tache * tacheEnCours = NULL;

/**
 * @brief Le coeur de l'ordonnanceur.
 *
 * C'est cette fonction qui détermine la prochaine tâche à exécuter.
 */
void ordonnanceur()
{
   Tache * tachePrecedente = tacheEnCours;

   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "in (de tache %d)\n", tachePrecedente->numero);
   
#ifdef MANUX_CONSOLES_VIRTUELLES
   // Basculement entre les consoles virtuelles WARNING pourquoi ici
   // !? A faire faire par dummyTask
   if (basculeConsoleDemandee) {
      basculeConsoleDemandee = FALSE;
      basculerVersConsoleSuivante();
   }
#endif

   // Attention, si la tâche en cours n'est pas préemptible
   if (tacheEnCours->nonPreemptible) {
      printk_debug(DBG_KERNEL_ORDON, "abandon (tache %d non preemptible)\n", tachePrecedente->numero);
      return;
   }
   
   // (1) On s'occupe de la tâche en cours
  
   // On cumule le temps d'écution dont elle vient de profiter
   tacheEnCours->tempsExecution += (nbTopHorloge - dateDernierOrdonnancement);

   // Si on n'est pas là spontanément, on se considère en cours, mais
   // c'est fini pour le moment !
   if (tacheEnCours->etat == Tache_En_Cours) {
      tacheEnCours->etat = Tache_Prete;
   
      insererCelluleTache(&listeTachesPretes,
                          tacheEnCours,
                          (CelluleTache*)tacheEnCours+sizeof(Tache));
   }

   // Dans les autres cas, la tâche est sensée être dans une file
   // correspondant à son état
   
   // (2) on cherche la tâche suivante
   // On prend la première tâche prête, il y en a au moins une : la dummy
   printk_debug(DBG_KERNEL_ORDON, "on cherche la suivante ...\n");   
   do {
      tacheEnCours = extraireTache(&listeTachesPretes);
   } while (tacheEnCours->etat != Tache_Prete); 
   printk_debug(DBG_KERNEL_ORDON, "... ce sera la %d\n", tacheEnCours->numero);   

   tacheEnCours->etat = Tache_En_Cours;
 
   /* On note la date pour pouvoir mesurer le temps dont elle va profiter */
   dateDernierOrdonnancement = nbTopHorloge;

   if (tacheEnCours != tachePrecedente){
      printk_debug(DBG_KERNEL_ORDON, "On passe a la tache %d de TSS 0x%x \n",
	     tacheEnCours->numero,
	     tacheEnCours->indiceTSSDescriptor);

      /* Une activation de plus pour elle */
      tacheEnCours->nbActivations++;

      printk_debug(DBG_KERNEL_ORDON, "out (vers tache %d)\n", tacheEnCours->numero);
      basculerVersTache(tacheEnCours);
   }
   printk_debug(DBG_KERNEL_ORDON, "back (vers tache %d)\n", tacheEnCours->numero);
}

void afficherEtatUneTache(Tache * tache)
{
  printk(" [  %d]  %s   %4d  %2d:%2d  0x%x   0x%x  0x%x\n",
       tache->numero,
         (tache->etat == Tache_En_Cours)?"c":(((tache->etat == Tache_Prete)?"p":((tache->etat == Tache_Terminee)?"t":"b"))),
          tache->nbActivations,
	  totalMinutesDansTemps(tache->tempsExecution),
	  secondesDansTemps(tache->tempsExecution),
          tache,
#ifdef MANUX_TACHE_CONSOLE
          tache->console,
#else
          0x00, // WARNING : bof
#endif
          tache->ldt);
}

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
#endif

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
#endif
}
#endif

/**
 * @brief Affichage des tâches
 */
void afficherEtatTaches()
{
   CelluleTache * celluleTache;

   printk("\n ------------------------<SCHEDULER t = %d:%d (%d)>----------------------------\n",
	  totalMinutesDansTemps(nbTopHorloge),
	  secondesDansTemps(nbTopHorloge),
	  nbTopHorloge);

   printk("\n Num prochaine tache : %d\n", numeroProchaineTache);
   printk(" [num] et   nbAc  tpsEx    tache   console       ldt\n");
   for (celluleTache = listeToutesLesTaches.tete;
      celluleTache != NULL;
      celluleTache = celluleTache->suivant){
        afficherEtatUneTache(celluleTache->tache);
   }
   printk("\n------------------------------------------------------------------------------\n");
}

/**
 * @brief Affichage des IT reçues
 *
 * Le but est de présenter un écran synthétique avec le nombre
 * d'occurences de chacune des interruptions.
 */
void interruptionAfficher()
{
   int i;

   printk("----[ Exceptions ]--------------------------\n");
   for (i = 0; i < MANUX_NB_EXCEPTIONS ; i ++) {
      if (nbItRecues[i]) {
         printk(" [ %3x : %5d ]", i, nbItRecues[i]); 
      }
   }
   printk("\n");
   
   printk("----[ IRQ ]---------------------------------\n");
   for (i = MANUX_NB_EXCEPTIONS; i < MANUX_NB_EXCEPTIONS + MANUX_NB_IRQ ; i ++) {
      if (nbItRecues[i]) {
         printk(" [ %3x : %5d ]", i, nbItRecues[i]); 
      }
   }
   printk("\n");
   
   printk("----[ Interruptions ]-----------------------\n");
   for (i = MANUX_NB_EXCEPTIONS + MANUX_NB_IRQ; i < MANUX_NB_INTERRUPTIONS ; i ++) {
      if (nbItRecues[i]) {
         printk(" [ %3x : %5d ]", i, nbItRecues[i]); 
      }
   }
   printk("\n");
}

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
#if defined(MANUX_TACHES) && !defined(MANUX_REENTRANT)
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

#if defined(MANUX_TACHES)  // A virer non ? Scheduler sans tache, ...

#if defined(MANUX_DUMMY_TASK)
/**
 * Le corps d'une tâche à exécuter lorsqu'on n'a que ça à faire, ...
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

      ordonnanceur();
   }
}
#endif // MANUX_DUMMY_TASK
#endif // MANUX_TACHES
/**
 * @brief Ajout d'une tâche dans l'ordonnanceur
 *
 * On se contente de l'insérer dans la liste des tâches de l'ordonnanceur. 
 */
void ordonnanceurAddTache(Tache * tache)
{
   insererCelluleTache(&listeTachesPretes,
		       tache,
		       (CelluleTache*)tache+sizeof(Tache));
}

/**
 * @brief Iniitalisation de l'ordonnanceur
 */
void initialiserScheduler()
{
   Tache * t1; // Le fil d'exécution en cours, qui deviendra init
#ifdef MANUX_DUMMY_TASK
   Tache * t2; // Ce sera la "dummy" task
#endif // MANUX_DUMMY_TASK

   dateDernierOrdonnancement = nbTopHorloge;

   printk_debug(DBG_KERNEL_ORDON, "initialisation du scheduler ...\n");

   // Initialisation de la liste (vide) des tâches en cours
   initialiserListeTache(&listeTachesPretes);

   // Initialisation de la liste (vide) de toutes les tâches
   initialiserListeTache(&listeToutesLesTaches);

   // Initialisation de la liste (vide) des tâches terminées
   initialiserListeTache(&listeTachesTerminees);

   // Création d'une tâche pour le fil actuel (premier numéro)
   t1 = tacheCreer(NULL);
   if (t1 == NULL) {
      paniqueNoyau("impossible de creer la premiere tache !\n");
   }
#ifdef MANUX_TACHE_CONSOLE   
   tacheSetConsole(t1, consoleNoyau());
#endif
   /* Cas particulier de la première tâche : */
   /*   . on la déclare comme en cours.   */
   tacheEnCours = t1;
   t1->etat = Tache_En_Cours;
   
   /*   . on charge son task register ;      */
   ltr(t1->indiceTSSDescriptor);
			
   // Avant de permettre à une deuxième tâche d'entrer en concurrence,
   // il faut s'assurer qu'on a la main sur le noyau.
#if !defined(MANUX_REENTRANT) && defined(MANUX_EXCLUSION_MUTUELLE)
   printk_debug(DBG_KERNEL_ORDON, "on verouille le verrou\n");
   
   exclusionMutuelleEntrer(&verrouGeneralDuNoyau);
   assert(tacheDansLeNoyau == 0);
   tacheDansLeNoyau = tacheEnCours->numero;
#endif

#ifdef MANUX_DUMMY_TASK
   printk_debug(DBG_KERNEL_ORDON, "creons la dummy\n");

   // Initialisation de la tache "aDummyKernelTask" (numéro 2)
   t2 = tacheCreer(aDummyKernelTask);
   
   if (t2 == NULL) {
      paniqueNoyau("impossible de creer la seconde tache !\n");
   }

   printk_debug(DBG_KERNEL_ORDON, "la dummy est la ...\n");

#ifdef MANUX_TACHE_CONSOLE   
   tacheSetConsole(t2, consoleNoyau());
#endif

   printk_debug(DBG_KERNEL_ORDON, "on ajoute la t2 dans l'ordo\n");
   
   // A partir de maintenant, nous ne sommes plus seuls
   ordonnanceurAddTache(t2);

#endif // MANUX_DUMMY_TASK
   
   printk_debug(DBG_KERNEL_ORDON, "scheduler is done\n");
}

#ifdef MANUX_APPELS_SYSTEME

/**
 * @brief Implantation de l'AS d'obtention de l'identifiant
 */
int sys_identifiantTache()
{
   return (int)tacheEnCours->numero;
}

#ifdef MANUX_TACHE_CONSOLE
/**
 * @brief l'AS permettant d'obtenir la Console de la tâche
 *
 * Le nom de cette fonction est à changer
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
#endif // MANUX_TACHE_CONSOLE

/**
 * @brief Implantation de l'appel système d'invocation de l'ordonnanceur
 */
int sys_basculerTache(ParametreAS as)
{
   assert(tacheEnCours != NULL);
   ordonnanceur();

   return 0;
}

/**
 * @brief Implantation de l'appel système de création d'une nouvelle
 * tâche
 * @param as les paramètrers d'un appel système
 * @param corpsTache un pointeur vers la fonction a exécuter
 * @param shareConsole pour partager la console de la tâche en cours
 * (ou en crééer une nouvelle sinon)
 */
TacheID sys_creerTache(ParametreAS as, CorpsTache corpsTache, booleen shareConsole)
{
   Tache   * tache;
#ifdef MANUX_TACHE_CONSOLE
   Console * console;
#endif
   
   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "corpsTache = 0x%x, share=%d\n", corpsTache, shareConsole);

   // Création de la tâche
   tache = tacheCreer(corpsTache);
   if (tache == NULL) {
      return -ENOENT;
   }

#ifdef MANUX_TACHE_CONSOLE
   // Affectation de la console
#   ifdef MANUX_CONSOLES_VIRTUELLES    // WARNING : MANUX_TACHE_CONSOLE plutôt ?
   if (shareConsole) {
      console = tacheEnCours->console;
   } else {
      console = creerConsoleVirtuelle();
   }
#   else
   console = consoleNoyau();
#   endif // MANUX_CONSOLES_VIRTUELLES
   tacheSetConsole(tache, console);
#endif

   ordonnanceurAddTache(tache);
   
   return tache->numero;
}
#endif // MANUX_APPELS_SYSTEME

/**
 * @brief Création et ordonnancement d'une tâche noyau
 */
TacheID ordonnanceurCreerTacheNoyau(CorpsTache corpsTache)
{
   Tache   * tache;
   
   assert(tacheEnCours != NULL);

   printk_debug(DBG_KERNEL_ORDON, "corpsTache = 0x%x\n", corpsTache);

   // Création de la tâche
   tache = tacheCreer(corpsTache);
   if (tache == NULL) {
      return -ENOENT;
   }

#ifdef MANUX_TACHE_CONSOLE
   // Affectation de la console
   tacheSetConsole(tache,  consoleNoyau());
#endif

   ordonnanceurAddTache(tache);
   
   return tache->numero;
}











#ifdef SUPPRIME
/*
 * Insertion d'une nouvelle tâche dans l'ordonnanceur. La valeur
 * retournée est l'id de la tâche ou un code d'erreur.
 * WARNING a virer dès que la précédente est OK
 */
TacheID ordonnancerTache(CorpsTache corpsTache, console * cons)
{
   Tache   * tache;

#ifdef MANUX_TACHE_CONSOLE   
   Console * cons;
#   ifdef MANUX_CONSOLES_VIRTUELLES
   if (nouvelleConsole) {
      cons = creerConsoleVirtuelle();
   } else {
      if (tacheEnCours != NULL) {
         cons = tacheEnCours->console;
      } else { // Pour la première a priori
         cons = NULL;//consoleNoyau();
      }
   }   
#   endif // MANUX_CONSOLES_VIRTUELLES
#endif // MANUX_TACHE_CONSOLE
   
   /* Création de la tache */
#ifdef MANUX_TACHE_CONSOLE
   tache = creerTache(corpsTache, cons);
#else
   tache = creerTache(corpsTache);
#endif
   if (tache == NULL) {
      return -ENOMEM;
   }

   /* On insère la nouvelle tâche à la fin de la liste */
   if (corpsTache) {
      insererCelluleTache(&listeTachesPretes,
                          tache,
                          (CelluleTache*)tache+sizeof(Tache));
      printk_debug(DBG_KERNEL_TACHE, "Tache inseree\n");

   } else {
     //printk("99999\n");
      /* Cas particulier de la première tâche : */
      /*   . on charge son task register ;      */
      ltr(tache->indiceTSSDescriptor);
      /*   . et on la déclare comme en cours.   */
      tacheEnCours = tache;
   }
   //printk("00000\n");
   
   return tache->numero;
}
#endif
