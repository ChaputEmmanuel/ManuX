/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de gestion des taches.               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/tache.h>
#include <manux/scheduler.h>  /* tacheEnCours */
#include <manux/memoire.h>
#include <manux/segment.h>
#include <manux/console.h>
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
TacheID numeroProchaineTache;

/*
 * Chaque tâche pourra voir les infos la concernant à cette adresse
 */
Tache * tacheCourante = (Tache*)(TAILLE_PAGE * NOMBRE_PAGES_SYSTEME);

unsigned int nbActivations = 0; //  Nombre d'appels à activerTache

void basculerVersTache(Tache * tache)
/*
 * Activation d'une tâche
 */
{
   volatile uint32 selecteur[2] = {0 , tache->indiceTSSDescriptor};

   printk_debug(DBG_KERNEL_ORDON, "t 0x%x %d\n", tache, tache->nbActivations);
   __asm__ __volatile__ ("ljmp %0"::"m" (*selecteur));

}

Tache * creerTache(CorpsTache corpsTache, Console * cons)
{
   void  * unePage;
   Tache * tache;
   void  * pile;      // Elle a sa propre pile
   
   /* On stoque les infos en zone système */
   unePage = allouerPageSysteme();
   if (unePage == NULL) {
      printk_debug(DBG_KERNEL_TACHE, "plus de memoire disponible\n");
      return NULL;
   }

   tache = (Tache *) unePage;

   pile = (void*) allouerPageSysteme();
   if (pile == NULL) {
      printk_debug(DBG_KERNEL_TACHE, "plus de memoire disponible\n");
      // WARNING : libérer unePage
      return NULL;
   }
   
   /* Initialisation du descripteur de tache */
   tache->tss.Reserve1 = (uint16) 0;
   tache->tss.Reserve2 = (uint16) 0;
   tache->tss.Reserve3 = (uint16) 0;
   tache->tss.Reserve4 = (uint16) 0;
   tache->tss.Reserve5 = (uint16) 0;
   tache->tss.Reserve6 = (uint16) 0;
   tache->tss.Reserve7 = (uint16) 0;
   tache->tss.Reserve8 = (uint16) 0;
   tache->tss.Reserve9 = (uint16) 0;
   tache->tss.Reserve10 = (uint16) 0;
   tache->tss.Reserve11 = (uint16) 0;
   tache->tss.Reserve12 = (uint16) 0;
   tache->tss.CS = 0x08;  /* WARNING, hardcodé pas beau ! */
   tache->tss.DS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.ES = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.FS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.GS = 0x10;  /* WARNING, hardcodé pas beau ! */
   tache->tss.SS = 0x18;  /* WARNING, hardcodé pas beau ! */
   tache->tss.ESP = (uint32)pile + 4092;  /* WARNING !! */
   tache->tss.EIP = (uint32)corpsTache;
   tache->tss.EFLAGS = (uint32)0x200;

   /* Ajout de la tâche dans la GDT */
   tache->indiceTSSDescriptor = ajouterDescTSS(gdtSysteme,
					       &tache->tss,
					       0x67, FALSE);

   /* On recharge la GDT */
   chargerGDT(gdtSysteme);

   /* On lui affecte son numero */
   tache->numero = numeroProchaineTache++;

   /* On lui affecte sa console */
   assert(cons != NULL);
   tache->console = cons;

#ifdef MANUX_FS   
   tache->fichiers[1].prive = (void*)cons;
   tache->fichiers[1].methodes = &consoleMethodesFichier;
#endif

   /* Elle n'a pas encore été activée */
   tache->nbActivations = 0;
   tache->tempsExecution = (Temps)0;
   
   /* Zone mémoire utilisable */
   tache->tailleMemoire = (void *)(nombrePagesSysteme * TAILLE_PAGE);

   /* On lui affecte son PDBR */
   creerTablePagination((PageDirectory *)&(tache->tss.CR3));

   /* On ajoute la page décrivant la tâche en début de mémoire spécifique */
   ajouterPage((PageDirectory *)&tache->tss.CR3,
	       tache,
	       tache->tailleMemoire);
   tache->tailleMemoire += TAILLE_PAGE;

   /* On lui affecte sa LDT */
   tache->ldt = (DescriptorTable *)(unePage + sizeof(Tache));
   tache->tss.LDT = (uint16)setDescripteurSegment(gdtSysteme,
					  (uint32)&(tache->ldt->taille),
					  LDT_NB_BYTES,
                                          0x82, 0xC0);

   /* On recharge la GDT */
   chargerGDT(gdtSysteme);

   /* Copie de la LDT, maintenant qu'elle est complète */
   memcpy(tache->ldt, gdtSysteme, tailleGDTSysteme);

   /* Elle est prète à être exécutée */
   tache->etat = Tache_Prete;

   /* On affiche quelques infos */
   printk_debug(DBG_KERNEL_TACHE, "Tache[%d] = 0x%x\n", tache->numero, tache);
   printk_debug(DBG_KERNEL_TACHE, "cons = 0x%x, tss=0x%x, ldt=0x%x\n", tache->console, tache->tss, tache->ldt);

   return tache;
}

