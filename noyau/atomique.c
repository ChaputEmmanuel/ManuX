/*----------------------------------------------------------------------------*/
/*      Définition des opérations atomiques de ManuX.                         */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/atomique.h>
#include <manux/memoire.h>   // NULL
#include <manux/debug.h>     // assert
#if defined(MANUX_ATOMIQUE_AUDIT) && defined(MANUX_KMALLOC)
#   include <manux/kmalloc.h>
#endif

#if defined(MANUX_ATOMIQUE_AUDIT) && defined(MANUX_KMALLOC)

typedef struct _CelluleCondition {
   Condition                * condition;
   struct _CelluleCondition * suivant;
} CelluleCondition;
  
typedef struct _ListeConditions {
   CelluleCondition * tete;
   CelluleCondition * queue;
} ListeConditions;

ListeConditions * listeConditionsCreer()
{
   ListeConditions * result;

   result = (ListeConditions *) kmalloc(sizeof(ListeConditions));

   result->tete = NULL;
   result->queue = NULL;

   return result;
}

/**
 * @brief Insertion d'une condition dans une liste (au début)
 *
 */
void listeConditionsInserer(ListeConditions * l, Condition * c)
{
   CelluleCondition * cell = (CelluleCondition *) kmalloc(sizeof(CelluleCondition));
   
   assert(l != NULL);
   assert(c != NULL);

   if (cell != NULL) {
      cell->condition = c;
      cell->suivant = NULL;
      if (l->queue != NULL) {
         l->queue->suivant = cell;
      }
      l->queue = cell;
      if (l->tete  == NULL) {
	l->tete = cell;
      }
   }
}

/** 
 * @brief  Liste des conditions définies sur le système
 */
static ListeConditions * listeConditions = NULL;

#endif //  defined(MANUX_ATOMIQUE_AUDIT) && defined(MANUX_KMALLOC)

/**
 * @brief Entrée en exclusion mutuelle.
 * 
 * La tâche appelante est éventuellement mise en attente dans une
 * file spécifique. Elle n'en sera extraite que par la sortie d'une
 * tâche qui est dans la zone d'exclusion.
 */
void exclusionMutuelleEntrer(ExclusionMutuelle * em)
{
   // Tant que je n'acquière pas le verrou ...
   while (compareEtEchange(&(em->verrou), 0, tacheEnCours->numero)) {

      // ... je me mets en attente. Pour cela, j'interdis la
      // préeemption de sorte à ne pas me retrouvée dans l'état
      // bloquées mais référencée dans aucune liste d'attente !
      tacheEnCoursInterdirePreemption();  
      
      tacheEnCours->etat = Tache_Bloquee;
      insererCelluleTache(&(em->tachesEnAttente),
                          tacheEnCours,
                          (CelluleTache*)tacheEnCours+sizeof(Tache));
      
      tacheEnCoursAutoriserPreemption();

      // Puisque je suis bloquée, je rends la main
      ordonnanceur();   
   };

   // Je peux tranquilement modifier le compteur !  
   em->nbEntrees++;
}

void exclusionMutuelleSortir(ExclusionMutuelle * em)
{
   Tache * ta;

   // Si on trouve une tâche en attente de cette exclusion mutuelle,
   // on la réveille.
   if ((ta = extraireTache(&(em->tachesEnAttente))) != NULL) {
      ta->etat = Tache_Prete;
      insererCelluleTache(&listeTachesPretes,
                          ta,
                          (CelluleTache*)ta+sizeof(Tache));
   }
   // Le verrou est encore à nous, on peut modifier les compteurs sans
   // crainte 
   (em)->nbSorties++ ;

   // On déverouille
   atomiqueInit(&(em)->verrou, 0);
}

/**
 * @brief Initialisation d'une condition
 */
void conditionInitialiser(Condition * cond)
{
   initialiserListeTache((&cond->tachesEnAttente));

#if defined(MANUX_ATOMIQUE_AUDIT)
   cond->nbSignaler = 0;
   cond->nbDiffuser = 0;

   if (listeConditions == NULL) {
      listeConditions = listeConditionsCreer();
   }
   if (listeConditions != NULL) {
      listeConditionsInserer(listeConditions, cond);
   }
#endif
}

/**
 * @brief Attente de la prochaine occurence d'une condition
 *
 * La tâche appelante doit être dans l'exclusion mutuelle
 * (qui sera automatiquement libérée le temps de l'attente puis
 * reprise avant que cette fonction ne rende la main)
 */
void conditionAttendre(Condition * cond, ExclusionMutuelle * em)
{
   // On se place dans la liste des tâches en attente de la condition
   // que l'on peut manipuler sans risque car on est protégé par
   // l'exclusion mutuelle 
   insererCelluleTache(&(cond->tachesEnAttente),
                       tacheEnCours,
                       (CelluleTache*)tacheEnCours+sizeof(Tache));
   
   // On doit libérer l'exclusion mutuelle et se placer dans l'état
   // bloqué, on va se rendre non préemptible pour faire ça afin de ne
   // pas se retrouvée bloquée après avoir été préemptée par une tâche
   // qui signale la condition !
   tacheEnCoursInterdirePreemption();
   
   exclusionMutuelleSortir(em);
   tacheEnCours->etat = Tache_Bloquee;
   
   tacheEnCoursAutoriserPreemption();

   // On est bloquée et préemptible, on peut rendre la main
   ordonnanceur();

   // Il nous faut ré-acquérir l'exclusion mutuelle
   exclusionMutuelleEntrer(em);
}

/**
 * @brief Signaler une occurence d'une condition
 *
 * Attention, doit être utilisée sous la protection de l'exclusion
 * mutuelle détenue par les tâches en attente.
 * Elle est donc inutilisable dans un handler d'interruption.
 */
void conditionSignaler(Condition * cond)
{
   Tache * ta;

   // Si on trouve une tâche en attente de cette condition
   // on la réveille.
   if ((ta = extraireTache(&(cond->tachesEnAttente))) != NULL) {
      ta->etat = Tache_Prete;
      insererCelluleTache(&listeTachesPretes,
                          ta,
                          (CelluleTache*)ta+sizeof(Tache));
   }
#if defined(MANUX_ATOMIQUE_AUDIT)
   cond->nbSignaler++;
#endif
}

/**
 * @brief Signaler une occurence d'une condition à toutes les tâches
 * en attente
 *
 * Attention, doit être utilisée sous la protection de l'exclusion
 * mutuelle détenue par les tâches en attente.
 * Ell est donc inutilisable dans un handler d'interruption.
 */
void conditionDiffuser(Condition * cond)
{
   Tache * ta;

   // Si on trouve une tâche en attente de cette condition
   // on la réveille.
   while ((ta = extraireTache(&(cond->tachesEnAttente))) != NULL) {
      ta->etat = Tache_Prete;
      insererCelluleTache(&listeTachesPretes,
                          ta,
                          (CelluleTache*)ta+sizeof(Tache));
   }
#if defined(MANUX_ATOMIQUE_AUDIT)
   cond->nbDiffuser++;
#endif
}

#if defined(MANUX_ATOMIQUE_AUDIT)
/**
 * @brief Affichage de l'état des variables condition
 */
void condtionsAfficherEtat()
{
   CelluleCondition * cell;

   printk("Cond  s  d\n");
   for (cell = listeConditions->tete; cell != NULL; cell = cell->suivant){
     printk("0x%x  %d %d\n", cell->condition, cell->condition->nbSignaler, cell->condition->nbDiffuser);
   }
}

#endif



