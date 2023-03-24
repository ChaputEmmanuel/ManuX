/*----------------------------------------------------------------------------*/
/*   Configuration générale du noyau                                          */ 
/*----------------------------------------------------------------------------*/


/*
 * Doit-on activer les "assert" ? Si cette macro n'est pas définie,
 * les assert ne produisent aucun code.
 */
#define MANUX_ASSERT_ACTIVES

/*
 * Utilisation des outils de synchronisation (mutex, semaphore, ...)
 */
//#define MANUX_OUTILS_SYNCHRO


/*----------------------------------------------------------------------------*/
/*   Gestion des tâches et ordonnancement.                                    */
/*----------------------------------------------------------------------------*/
/*
 * Implantation des tâches ? 
 */
#define MANUX_TACHES

/*
 * Ordonnancement préemptif ?
 */
#define MANUX_PREEMPTIF

