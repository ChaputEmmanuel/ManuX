/*----------------------------------------------------------------------------*/
/*      Définition du scheduler de Manux. Version trés minimale à revoir.     */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef SCHEDULER_DEF
#define SCHEDULER_DEF

#include <manux/types.h>
#include <manux/tache.h>
#include <manux/listetaches.h>
#include <manux/atomique.h>

/*
 * Pour paramétrer le scheduler
 */
#define KHZ * 1000

/*
 * Pour permettre la gestion des consoles virtuelles. WARNING n'a rien
 * à faire là
 */
extern booleen basculeConsoleDemandee;

extern booleen afficheEtatSystemeDemande;

/*
 * L'identification de la tâche en cours
 */
extern Tache * tacheEnCours;

/*
 * Le verrou suivant nous permet de garantir qu'un seul processus
 * est en mode noyau à un instant donné.
 */
//ExclusionMutuelle verrouNoyau;

void setFrequenceTimer(int freqHz);
/*
 * Positionne la fréquence du timer 0
 */

void initialiserScheduler();
/*
 * Déclencher le scheduler avant toute création de tache
 */

void ordonnancerTache(CorpsTache corpsTache, booleen nouvelleConsole);
/*
 * Création d'une nouvelle tâche et mise de celle-ci dans
 * la liste du scheduler. Si nouvelleConsole est vrai, on crée une
 * nouvelle console pour cette tâche. Sinon elle hérite de celle de
 * son créateur.
 */

void basculerTache();
/*
 * Basculer vers la prochaine tache à exécuter
 */

int AS_numeroTache();
/*
 * Obtention du numero de la tache en cours. WARNING ça n'a rien
 * à faire là ...
 */

uint32 AS_console();
/*
 * Obtention de la console de la tache en cours. WARNING ça n'a rien
 * à faire là ...
 */

#endif
