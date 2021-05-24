/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programme de manipulation de la mémoire sous    */
/*   ManuX.                                                                   */
/*      On fait pour le moment un truc hyper minimaliste, sans aucune forme   */
/*   de vérification, le but étant juste de fournir un service élémentaire    */
/*   aux autres parties du noyau afin d'en avancer le développement.          */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/types.h>      /* TacheID */
#include <manux/debug.h>      /* assert */
#include <manux/memoire.h>
#include <manux/atomique.h>
#include <manux/pagination.h> /* ajouterPage() */
#include <manux/console.h>

/*
 * Début de la zone mémoire dans laquelle on mémorise l'occupation
 * des pages
 */
#ifndef AFFECTATION_PAGES
#   define AFFECTATION_PAGES 0x1000
#endif

/*
 * Nombre de pages communes à toutes les tâches.
 */
int nombrePagesSysteme = NOMBRE_PAGES_SYSTEME;

/*
 * Le propriétaire de chaque page mémoire est identifié dans le
 * tableau suivant (0 <=> page libre). WARNING, pour le moment,
 * c'est 1 ou 0 (utilisé ou libre).
 */
TacheID * proprietairePage = (TacheID *)AFFECTATION_PAGES ;

/*
 * Le nombre global de pages dans le système
 */
static int nombrePages = 0;

void initialiserMemoire(uint32_t tailleMemoireDeBase,
			uint32_t tailleMemoireEtendue)
{
   int i;                      // Pour compter les pages initialisées 
   uint32_t taillePropietaire; // Taille nécessaire pour les gérer
   
   /* Les tailles sont données en Ko */
   nombrePages = (tailleMemoireDeBase + tailleMemoireEtendue) / 4;

   /* De combien de pages a-t-on besoin pour les gérer ? */
   taillePropietaire = (nombrePages + TAILLE_PAGE - 1) / TAILLE_PAGE; 
   
#ifdef DEBUG_MEMOIRE
   printk("%d pages de %d octets : %d pages de gestion\n",  nombrePages, TAILLE_PAGE, tailleProprietaire);
#endif

   /* Le tableau d'allocation des pages ne doit pas télescoper le noyau !*/
   assert((void*)(proprietairePage + taillePropietaire*TAILLE_PAGE) < (void *)KERNEL_START_ADDRESS);
   
   /* On considère le 1er méga occupé par le noyau */
   for (i = 0; i < DEBUT_MEMOIRE_ETENDUE/TAILLE_PAGE; i++) {
      proprietairePage[i] = (TacheID) 1;
   }

   /* Tout le reste est libre */
   for (i = DEBUT_MEMOIRE_ETENDUE/TAILLE_PAGE; i < nombrePages; i++) {
      proprietairePage[i] = (TacheID) 0;
   }
}

void * allouerPageSysteme()
{
   void * pageAllouee = NULL;
   int    numeroPage;

   while (pageAllouee == NULL) {
      numeroPage = DEBUT_MEMOIRE_ETENDUE/TAILLE_PAGE;
      while (  (numeroPage < nombrePagesSysteme)
             &&(proprietairePage[numeroPage] != (TacheID) 0)) {
         numeroPage++;
      }
      /* Si on trouve une page dispo */
      if (numeroPage < nombrePages) {
         /* On essaie de la réserver */
         if (atomiqueTestInit((Atomique *)&(proprietairePage[numeroPage]),
                              1, 0)) {
            pageAllouee = (void *) (numeroPage * TAILLE_PAGE);
	 }
      /* Sinon inutile de continuer */
      } else {
         return NULL;
      }
   }
   return pageAllouee;
}

void * allouerPage()
{
   void * pageAllouee = NULL;
   int    numeroPage;

   while (pageAllouee == NULL) {
      numeroPage = nombrePagesSysteme;
      while (  (numeroPage < nombrePages)
             &&(proprietairePage[numeroPage] != (TacheID) 0)) {
         numeroPage++;
      }

      /* Si on trouve une page dispo */
      if (numeroPage < nombrePages) {
         /* On essaie de la réserver */
         if (atomiqueTestInit((Atomique *)&(proprietairePage[numeroPage]),
                              1, 0)) {
            pageAllouee = (void *) (numeroPage * TAILLE_PAGE);
	 }
      /* Sinon inutile de continuer */
      } else {
         return NULL;
      }
   }
   return pageAllouee;
}

void * allouerPages(unsigned int nombre)
{
   return NULL;
}

void libererPage(void * pageLiberee)
{
}

int AS_obtenirPages(ParametreAS p, int nbPages)
{
   Page unePage;
   Tache * tache = tacheEnCours;

   /* Pour le moment, on ne sait faire qu'une à la fois */
   if (nbPages != 1) {
      return 0;
   }

   /* Obtention de la page */
   unePage = allouerPage();

   /* Ajout à la fin de la mémoire virtuelle de la tâche */
   ajouterPage((PageDirectory *)&tache->tss.CR3,
               unePage,
               tache->tailleMemoire);

   /* On note qu'on a davantage de mémoire */
   tache->tailleMemoire += TAILLE_PAGE;

   return 1;
}
