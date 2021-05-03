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
#include <manux/string.h>     /* bcopy */
#include <manux/segment.h>    /* setDescripteurSegment */
#include <manux/pagination.h> /* repertoirePaginationSysteme */

#include <manux/fichier.h>    /* pour créer stdout WARNING, à mettre ailleurs */
#include <manux/errno.h>      // Les codes d'erreur
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

   __asm__ __volatile__ ("ljmp %0"::"m" (*selecteur));
}

Tache * creerTache(CorpsTache corpsTache, Console * cons)
{
   void  * unePage;
   Tache * tache;

   /* On stoque les infos en zone système */
   unePage = allouerPageSysteme();
   if (unePage == NULL)
      printk_debug(DBG_KERNEL_TACHES, "plus de memoire disponible\n");
      return NULL;
   }

   tache = (Tache *) unePage;

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
   tache->tss.ESP = (uint32)unePage + 4092;  /* WARNING !! */
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
   tache->console = cons; // WARNING à virer (? ou pas, pour le moment non !)
   
   tache->fichiers[1].prive = (void*)cons;
   tache->fichiers[1].methodes = &consoleMethodesFichier;

   /* Zone mémoire utilisable */
   tache->tailleMemoire = (void *)(nombrePagesSysteme * TAILLE_PAGE);

   printk_debug(DBG_KERNEL_PAGIN, "Tache = 0x%x\n", tache);
   printk_debug(DBG_KERNEL_PAGIN, "Le CR3 avant = 0x%x\n", tache->tss.CR3);

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
   bcopy(gdtSysteme, tache->ldt, tailleGDTSysteme);

   /* Elle est prète à être exécutée */
   tache->etat = Tache_Prete;

   return tache;
}

TacheID sysFork()
{
   return -ENOMEM;   // WARNING : gestion des erreurs
}
