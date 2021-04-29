/*----------------------------------------------------------------------------*/
/*      Définition des appels système de Manux.                               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef APPEL_SYSTEME_DEF
#define APPEL_SYSTEME_DEF

#include <manux/types.h>

/*
 * Définition de l'interruption utilisée pour les appels système
 */
#ifndef MANUX_AS_INT
#   define MANUX_AS_INT 0x80
#endif

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
 * Liste des appels système prédéfinis
 */
#define NBAS_NUMERO_TACHE   0
#define NBAS_CONSOLE        1
#define NBAS_OBTENIR_PAGES  2
#define NBAS_ECRIRE         3
#define NBAS_FORK           4
/*
 * Définition des appels système. WARNING à dispatcher ...
 */
int numeroTache();
int obtenirPages(int n);

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

/*
 * Une macro permettant de définir l'interface d'un appel système
 * sans argument.
 */
#define appelSysteme0(numero, typeRetour, nom) \
   typeRetour nom()                            \
   {                                           \
      typeRetour resultat;                     \
      __asm__ __volatile__ ("int %2"                         \
              : "=a"(resultat)                 \
              : "0" (numero),                  \
                "N" (MANUX_AS_INT));           \
      return resultat;                         \
   }

/*
 * Une macro permettant de définir l'interface d'un appel système
 * à un argument.
 */
#define appelSysteme1(numero, typeRetour, nom, typeArgument) \
   typeRetour nom(typeArgument argument)                     \
   {                                                         \
      typeRetour resultat;                                   \
      __asm__ __volatile__ ("int %2"                                       \
              : "=a"(resultat)                               \
              : "0" (numero),                                \
                "N" (MANUX_AS_INT));                         \
      return resultat;                                       \
   }

/*
 * Une macro permettant de définir l'interface d'un appel système
 * à deux arguments.
 */
#define appelSysteme2(numero, typeRetour, nom, typeArgument1, typeArgument2) \
   typeRetour nom(typeArgument1 arg1, typeArgument2 arg2)                    \
   {                                                        \
      typeRetour resultat;                                  \
      __asm__ __volatile__ ("int %2"                                      \
              : "=a"(resultat)                              \
              : "0" (numero),                               \
                "N" (MANUX_AS_INT));                        \
      return resultat;                                      \
   }

/*
 * Une macro permettant de définir l'interface d'un appel système
 * à trois arguments.
 */
#define appelSysteme3(numero, typeRetour, nom, typeArgument1, typeArgument2, typeArgument3) \
   typeRetour nom(typeArgument1 arg1, typeArgument2 arg2, typeArgument3 arg3)                    \
   {                                                        \
      typeRetour resultat;                                  \
      __asm__ __volatile__ ("int %2"                                      \
              : "=a"(resultat)                              \
              : "0" (numero),                               \
                "N" (MANUX_AS_INT));                        \
      return resultat;                                      \
   }

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
