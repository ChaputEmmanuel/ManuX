/*----------------------------------------------------------------------------*/
/*      Implantation des appels système de Manux.                             */
/*                                                                            */
/*      Attention, il s'agit donc bien ici de la partie noyau !               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef APPEL_SYSTEME_DEF
#define APPEL_SYSTEME_DEF

#include <manux/appelsystemenum.h>  /* Les numéros des AS définis */
#include <manux/types.h>

/*
 * Définition du nombre maximal d'appels système
 */
#ifndef NB_MAX_APPELS_SYSTEME
#   define NB_MAX_APPELS_SYSTEME 10
#endif

/*
 * Tableau des appels systeme
 */
extern void * vecteurAppelsSysteme[NB_MAX_APPELS_SYSTEME];

/*
 * Type du premier paramètre de chaque appel système (dû au
 * séquencement des appels). WARNING a expliquer.
 */
typedef struct _ParametreAS {
   uint32 ecx;
   uint32 edx;
   uint32 ebx;
   uint32 esp;
   uint32 ebp;
   uint32 esi;
   uint32 edi;
   uint32 adresseRetourInterruption; // retour DE l'IT
   uint32 eflags;
   uint32 csretour;                  // retour DE l'interface
   uint32 adresseRetourInterface;    // retour DE l'interface
} ParametreAS;


int definirAppelSysteme(int num, void * appel);
/*
 * Définition de la fonction appel comme appel système de numéro num.
 * Code de retour:
 *    0         succés
 *    EINVAL    num n'est pas dans l'intervalle correct
 */

void initialiserAppelsSysteme();
/*
 * Initialisation des appels systèmes prédéfinis.
 */

#endif
