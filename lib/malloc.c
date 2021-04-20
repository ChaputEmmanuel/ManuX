/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de manipulation de la mémoire au     */
/*   niveau d'une tache sous ManuX.                                           */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#include <manux/malloc.h>

#include <manux/config.h>
#include <manux/memoire.h>      /* TAILLE_PAGE  */
#include <manux/scheduler.h>    /* tacheEnCours */
#include <manux/appelsysteme.h> /* numeroTache  */

/*
 * Informations relatives à la gestion de la mémoire par malloc/free.
 * Ici, on gère les blocs via une liste.
 */
typedef struct _InfoGestionMemoire {
   void        * prochainBlocLibre;
} InfoGestionMemoire;

/*
 * Informations permettant de gérer un bloc de mémoire libre.
 */
typedef struct _BlocLibre {
   unsigned int       taille;   /* Taille utilisable, en octets */ 
   InfoGestionMemoire igm;
} BlocLibre;

/*
 * Définition de la vision d'un bloc de mémoire lorsqu'il a été alloué à la
 * tâche. Naturellement, c'est le pointeur sur la zone utilisateur qui est
 * fourni.
 */
typedef struct _BlocAlloue {
   unsigned int taille;   /* En octets */
   char         utilisateur[sizeof(InfoGestionMemoire)];
} BlocAlloue;

/*
 * On conserve l'adresse de départ de gestion des infos. On les place dans
 * le premier segment, juste aprés les infos de la tâche.
 */
InfoGestionMemoire * infoMalloc
   = (InfoGestionMemoire * )(NOMBRE_PAGES_SYSTEME * TAILLE_PAGE + sizeof(Tache));

void * malloc(int taille)
{
   BlocLibre * blocLibre;

   /* Pas plus d'une page (moins la place des infos) */
   if (taille > TAILLE_PAGE - sizeof(BlocLibre) + sizeof(InfoGestionMemoire)) {
      return NULL;
   }

   /* Si la tâche ne veut rien, elle n'a rien :-) */
   if (taille <= 0) {
      return NULL;
   }

   /* Si rien n'est plus disponible, il nous faut une page */
   if (infoMalloc->prochainBlocLibre == NULL) {
      blocLibre = tacheCourante->tailleMemoire;
      if (obtenirPages(1) != 1) {
         return NULL;
      }
      blocLibre->taille = TAILLE_PAGE - sizeof(BlocLibre) + sizeof(InfoGestionMemoire);
      blocLibre->igm.prochainBlocLibre = NULL;
   }

   /* */
   /* Si on en est là, c'est un échec */
   return NULL;
}

void free(void * pointeur)
{
}

int initialiserMalloc()
{
  //   Page debutTas;

   /* Il nous faut au moins une page pour gérer les infos */
   infoMalloc->prochainBlocLibre = NULL;

   return 0;
}
