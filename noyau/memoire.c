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
#ifndef MANUX_AFFECTATION_PAGES
#   define MANUX_AFFECTATION_PAGES 0x1000
#endif

/*
 * Nombre de pages communes à toutes les tâches.
 */
int nombrePagesSysteme = MANUX_NOMBRE_PAGES_SYSTEME;

/*
 * Le propriétaire de chaque page mémoire est identifié dans le
 * tableau suivant (0 <=> page libre). WARNING, pour le moment,
 * c'est 1 ou 0 (utilisé ou libre).
 */
TacheID * proprietairePage = (TacheID *)MANUX_AFFECTATION_PAGES ;

/*
 * Le nombre global de pages dans le système
 */
static int nombrePages = 0;

/*
 * Marquer une page comme réservée. WARNING : il faut se protéger par un mutex
 */
static void inline reserverPage(uint32_t i)
{
   if (proprietairePage[i] != (TacheID)0) {
      paniqueNoyau("Page %d déjà prise\n", i);
   }
   proprietairePage[i] = (TacheID)1;
}

/*
 * Marquer une page comme libre (WARNING trouve un plus joli nom)
 */
static void inline demarquerPage(uint32_t i)
{
   proprietairePage[i] = (TacheID)0;
}

void initialiserMemoire(uint32_t tailleMemoireDeBase,
			uint32_t tailleMemoireEtendue,
			uint32_t adresseDebutManuX,
			uint32_t adresseFinManuX)
{
   uint32_t i;                  // Pour compter les pages initialisées 
   uint32_t tailleProprietaire; // Taille nécessaire pour les gérer
   uint32_t nbrePagesManuX;
   
   printk_debug(DBG_KERNEL_MEMOIRE, "base = %d, et = %d\n",
		tailleMemoireDeBase, tailleMemoireEtendue);

   /* Les tailles sont données en Ko */
   nombrePages = (tailleMemoireDeBase + tailleMemoireEtendue) / 4;

   printk_debug(DBG_KERNEL_MEMOIRE, "%d pages de %d octets\n",
		nombrePages, MANUX_TAILLE_PAGE);

   /* De combien de pages a-t-on besoin pour les gérer ? */
   /* Pour le moment, la gestion d'une page demande 4 octets (bientôt 1 bit !) */
   tailleProprietaire = 4 * nombrePages  / MANUX_TAILLE_PAGE; 
   
   printk_debug(DBG_KERNEL_MEMOIRE, "%d pages de gestion\n", tailleProprietaire);

   /* Le tableau d'allocation des pages ne doit pas télescoper le noyau !*/
   assert((void*)(proprietairePage + tailleProprietaire*MANUX_TAILLE_PAGE) < (void *)MANUX_KERNEL_START_ADDRESS);

#ifdef CETAITMIEUXAVANT
   /* On considère le 1er méga occupé par le noyau */
   for (i = 0; i < MANUX_DEBUT_MEMOIRE_ETENDUE/MANUX_TAILLE_PAGE; i++) {
      proprietairePage[i] = (TacheID) 1;
   }
   /* Tout le reste (au delà du premier méga) est libre */
   for (i = MANUX_DEBUT_MEMOIRE_ETENDUE/MANUX_TAILLE_PAGE; i < nombrePages; i++) {
      proprietairePage[i] = (TacheID) 0;
   }
#else

   // Par défaut tout est libre
   for (i = 0; i < nombrePages; i++) {
      demarquerPage(i);      
   }

   // Les handlers : WARNING est-ce utile ?
   reserverPage(0);

   // Le BIOS (indéboulonable !)
   for (i=0 ; i < MANUX_BIOS_NB_PAGES; i++) {
      reserverPage(ADDR_VERS_PAGE(MANUX_ADRESSE_BIOS)+i);
   }

   // Le noyau
   nbrePagesManuX = (adresseFinManuX-adresseDebutManuX)/MANUX_TAILLE_PAGE
     + (((adresseFinManuX-adresseDebutManuX)%MANUX_TAILLE_PAGE)?1:0);
   for (i=0 ; i < nbrePagesManuX; i++) {
      reserverPage(ADDR_VERS_PAGE(adresseDebutManuX)+i);
   }

   // La table d'allocation de la mémoire
   for (i=0 ; i < tailleProprietaire; i++){
     reserverPage(ADDR_VERS_PAGE(((uint32_t)proprietairePage))+i);
   }
   
   // La GDT
   for (i=0 ; i < MANUX_GDT_NB_PAGES; i++) {
      reserverPage(ADDR_VERS_PAGE(MANUX_ADRESSE_GDT)+i);
   }

   // La IDT
   for (i=0 ; i < MANUX_IDT_NB_PAGES; i++) {
      reserverPage(ADDR_VERS_PAGE(MANUX_ADRESSE_IDT)+i);
   }
#endif   
}

/*
void * reserverPageSysteme()
{
   void * pageAllouee = NULL;
   int    numeroPage;

   while (pageAllouee == NULL) {
      numeroPage = MANUX_DEBUT_MEMOIRE_ETENDUE/MANUX_TAILLE_PAGE;
      while (  (numeroPage < nombrePagesSysteme)
             &&(proprietairePage[numeroPage] != (TacheID) 0)) {
         numeroPage++;
      }
      // Si on trouve une page dispo 
      if (numeroPage < nombrePages) {
         // On essaie de la réserver 
         if (atomiqueTestInit((Atomique *)&(proprietairePage[numeroPage]),
                              1, 0)) {
            pageAllouee = (void *) (numeroPage * MANUX_TAILLE_PAGE);
	 }
      // Sinon inutile de continuer 
      } else {
         return NULL;
      }
   }
   //printk("return 0x%x\n", pageAllouee);
   return pageAllouee;
}
*/

void * allouerPage()
{
   void * pageAllouee = NULL;
   int    numeroPage = 0;

   // On cherche la première page libre
   while (  (numeroPage < nombrePages)
         &&(proprietairePage[numeroPage] != (TacheID) 0)) {
      numeroPage++;
   }

   /* Si on trouve une page dispo */
   if (numeroPage < nombrePages) {
      reserverPage(numeroPage);
      pageAllouee = (void *) (numeroPage * MANUX_TAILLE_PAGE);
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

#if defined(MANUX_APPELS_SYSTEME) && defined(MANUX_PAGINATION)
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
   tache->tailleMemoire += MANUX_TAILLE_PAGE;

   return 1;
}
#endif
