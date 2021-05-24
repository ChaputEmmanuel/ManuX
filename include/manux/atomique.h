/*----------------------------------------------------------------------------*/
/*      Définition des opérations atomiques de ManuX.                         */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef ATOMIQUE_DEF
#define ATOMIQUE_DEF

#include <manux/types.h>
#include <manux/tache.h>
#include <manux/listetaches.h>  /* Pour les listes de tâches en attente */
#include <manux/scheduler.h>    /* tacheEnCours */

/*
 * Type des données manipulées de façon atomique
 */
typedef uint32_t Atomique;

#define atomiqueInit(atom, val)\
   *(atom) = (val);

#define atomiqueLire(atom) \
   (atom)

static __inline__ booleen atomiqueTestInit(Atomique * atom, uint32_t val, uint32_t cond)
/*
 * La valeur de l'Atomique est comparée à la valeur cond ; en cas d'égalité,
 * l'Atomique prend la valeur val et le retour est 1. Sinon rien n'est fait
 * et le retour est 0.
 */
{
   uint8_t resultat;

   __asm__ ("cmpxchg %3, %2 \n\t"
            "sete %0 \n\t"
            : "=m"(resultat)
            : "a"(cond), "m"(*(volatile Atomique *)atom), "r"(val));
   return resultat;
}

/*
 * Gestion des exclusions mutuelles
 */
typedef struct _ExclusionMutuelle {
   Atomique   verrou;
   ListeTache tachesEnAttente;
} ExclusionMutuelle;

#define initialiserExclusionMutuelle(em)                  \
   atomiqueInit(&(em)->verrou, 0);                        \
   initialiserListeTache(&(em)->tachesEnAttente);

#define entrerExclusionMutuelle(em)                                  \
   while (!atomiqueTestInit(&((em)->verrou), 1, 0)) {                \
      tacheEnCours->etat = Tache_Bloquee;                            \
      insererCelluleTache(&(em)->tachesEnAttente,                    \
                          tacheEnCours,                              \
                          (CelluleTache*)tacheEnCours+sizeof(Tache));\
      basculerTache();                                               \
   }

#define sortirExclusionMutuelle(em)                            \
{                                                              \
   Tache * ta;                                                 \
   atomiqueInit(&(em)->verrou, 0);                             \
   if ((ta = extraireTache(&(em)->tachesEnAttente)) != NULL) { \
      ta->etat = Tache_Prete;                                  \
}

/*
#define sortirExclusionMutuelle(em)                            \
{                                                              \
   Tache * ta;                                                 \
   atomiqueInit(&(em)->verrou, 0);                             \
   if ((ta = extraireTache(&(em)->tachesEnAttente)) != NULL) { \
      ta->etat = Tache_Prete;                                  \
      insererCelluleTache(tachesPretes,                        \
                          tacheEnCours,                        \
                          (CelluleTache*)ta+sizeof(Tache));    \
   }                                                           \
}
*/
  
#endif
