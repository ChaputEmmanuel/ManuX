/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de gestion des taches.               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/tache.h>
#include <manux/scheduler.h>  /* tacheEnCours */
#include <manux/memoire.h>
#include <manux/segment.h>
#ifdef MANUX_TACHE_CONSOLE
#   include <manux/console.h>
#endif
#include <manux/atomique.h>
#include <manux/segment.h>    /* setDescripteurSegment */
#include <manux/pagination.h> /* repertoirePaginationSysteme */

#include <manux/fichier.h>    /* pour créer stdout WARNING, à mettre ailleurs */
#include <manux/errno.h>      // Les codes d'erreur
#include <manux/string.h>     // memcpy
#include <manux/printk.h>
#include <manux/debug.h>

/*
 * Le numero de la prochaine tache (WARNING : et si on cycle ?) 
 */
TacheID numeroProchaineTache = 0;

unsigned int nbActivations = 0; //  Nombre d'appels à activerTache

void basculerVersTache(Tache * tache)
/*
 * Activation d'une tâche
 */
{
   volatile uint32_t selecteur[2] = {0 , tache->indiceTSSDescriptor};

   __asm__ __volatile__ ("ljmp %0"::"m" (*selecteur));

}

/*#ifdef MANUX_TACHE_CONSOLE
Tache * creerTache(CorpsTache corpsTache, struct _Console * cons)
#else
Tache * creerTache(CorpsTache corpsTache)
#endif
*/
Tache * tacheCreer(CorpsTache corpsTache)
{
   void  * unePage;
   Tache * tache;
   void  * pile;      // Elle a sa propre pile

   /* On stoque les infos en zone système */
   unePage = allouerPage();
   if (unePage == NULL) {
      printk_debug(DBG_KERNEL_TACHE, "plus de memoire disponible\n");
      return NULL;
   }

   tache = (Tache *) unePage;

   pile = (void*) allouerPage();

   if (pile == NULL) {
      printk_debug(DBG_KERNEL_TACHE, "plus de memoire disponible\n");
      // WARNING : libérer la page de la tache
      return NULL;
   }
   
   /* Initialisation du descripteur de tache */
   tache->tss.Reserve1 = (uint16_t) 0;
   tache->tss.Reserve2 = (uint16_t) 0;
   tache->tss.Reserve3 = (uint16_t) 0;
   tache->tss.Reserve4 = (uint16_t) 0;
   tache->tss.Reserve5 = (uint16_t) 0;
   tache->tss.Reserve6 = (uint16_t) 0;
   tache->tss.Reserve7 = (uint16_t) 0;
   tache->tss.Reserve8 = (uint16_t) 0;
   tache->tss.Reserve9 = (uint16_t) 0;
   tache->tss.Reserve10 = (uint16_t) 0;
   tache->tss.Reserve11 = (uint16_t) 0;
   tache->tss.Reserve12 = (uint16_t) 0;
   tache->tss.CS = 0x08;  /* WARNING, hardcodé pas beau ! */
   tache->tss.DS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.ES = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.FS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.GS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.SS = 0x18;  /* WARNING, hardcodé pas beau ! */
   tache->tss.ESP = (uint32_t)pile + 4092;  /* WARNING !! */
   tache->tss.EIP = (uint32_t)corpsTache;
   tache->tss.EFLAGS = (uint32_t)0x200;

   /* Ajout de la tâche dans la GDT */
   tache->indiceTSSDescriptor = ajouterDescTSS(gdtSysteme,
					       &tache->tss,
					       0x67, FALSE);

   /* On recharge la GDT (nécessaire suite changement de taille ?) */
   chargerGDT(gdtSysteme);

   /* On lui affecte son numero */
   tache->numero = numeroProchaineTache++;

#ifdef MANUX_FICHIER
   // Pas de fichier ouvert pour le moment
   for (int i = 0; i < MANUX_NB_MAX_FICHIERS; i++){
     tache->fichiers[i] = NULL;
   }
   tache->nbFichiersOuverts = 0;
#endif
   
#ifdef MANUX_TACHE_CONSOLE   
   // Pas de console pour le moment
   tache->console = NULL;
#endif
   
   /* Elle n'a pas encore été activée */
   tache->nbActivations = 0;
   tache->tempsExecution = (Temps)0;
   
   /* Zone mémoire utilisable */
   tache->tailleMemoire = (void *)(nombrePagesSysteme * MANUX_TAILLE_PAGE);

#ifdef MANUX_PAGINATION
   /* On lui affecte son PDBR */
   creerTablePagination((PageDirectory *)&(tache->tss.CR3));

   /* On ajoute la page décrivant la tâche en début de mémoire spécifique */
   ajouterPage((PageDirectory *)&tache->tss.CR3,
	       tache,
	       tache->tailleMemoire);
   tache->tailleMemoire += MANUX_TAILLE_PAGE;
#endif
   // printk("hhhhh\n");

   /* On lui affecte sa LDT */
   //   tache->ldt = (DescriptorTable *)(unePage + sizeof(Tache));
   /*   tache->ldt = (DescriptorTable *)allouerPage();
   tache->tss.LDT = (uint16_t)setDescripteurSegment(gdtSysteme,
					  (uint32_t)&(tache->ldt->taille),
					  LDT_NB_BYTES,
                                        0x82, 0xC0);
   */
   tache->ldt = NULL;
   tache->tss.LDT = NULL;
   //   printk("iiiii\n");
   
   /* On recharge la GDT */
   //   chargerGDT(gdtSysteme);
   //  printk("jjjjj\n");

   /* Copie de la LDT, maintenant qu'elle est complète */
   //memcpy(tache->ldt, gdtSysteme, tailleGDTSysteme);

   //  printk("kkkkk\n");

   /* Elle est prète à être exécutée */
   tache->etat = Tache_Prete;
   //  printk("lllll\n");

   /* On affiche quelques infos */
   printk_debug(DBG_KERNEL_TACHE, "Tache[%d] = 0x%x\n", tache->numero, tache);
#ifdef MANUX_TACHE_CONSOLE
   printk_debug(DBG_KERNEL_TACHE, "cons = 0x%x, tss=0x%x, ldt=0x%x\n", tache->console, tache->tss, tache->ldt);
#else
   printk_debug(DBG_KERNEL_TACHE, "tss=0x%x, ldt=0x%x\n", tache->tss, tache->ldt);
#endif
   return tache;
}

#ifdef MANUX_TACHE_CONSOLE   
/**
 * @brief Affectation d'une console à une tâche
 */
void tacheSetConsole(Tache * tache, struct _Console * cons)
{
   tache->console = cons;

#   ifdef MANUX_FICHIER   // et sinon !?
   Fichier * f;
   INoeud * i;
   
   i = consoleCreerINoeud(cons);
   f = fichierCreer(i);
   
   tache->fichiers[0] =  f;
   
#   endif
}

#ifdef MANUX_FICHIER
/**
 * @brief Recherche du premier numéro de fichier ouvert disponible
 *
 * Attention : doit être protégé par un mutex !
 */
static int inline tacheNumFichierLibre(Tache * tache)
{
   int result = 0;

   while ((result < MANUX_NB_MAX_FICHIERS) && (tache->fichiers[result])) {
      result++;
   }
   return result;
}

/**
 * @brief Ajout de n fichiers dans la table des descripteurs de
 * fichier d'une tâche.
 *
 * On les affecte tous ou aucun. A priori, n = 1 la plupart du temps,
 * et éventuellement 2 pour les tubes, ...
 */
int tacheAjouterFichiers(Tache * tache, int n, Fichier * fichiers[], int * fds)
{
   int nbLibres;
   int result = 0;
   int fd;
   
   // WARNING à protéger par un mutex lock

   // A-t-on la place ?
   nbLibres = MANUX_NB_MAX_FICHIERS - tache->nbFichiersOuverts;
   
   // Si oui on affecte
   if (nbLibres >= n) {
      for (int i = 0; i < n; i++) {
         fd = tacheNumFichierLibre(tache);
	 assert(fd >= 0);
	 assert(fd < MANUX_NB_MAX_FICHIERS);
	 fds[i] = fd;
	 tache->fichiers[fd] = fichiers[i];
	 tache->nbFichiersOuverts ++;
	 result++;
      }
   }

   return result;
}
#endif

#endif
