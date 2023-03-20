/*----------------------------------------------------------------------------*/
/*   Configuration générale du noyau                                          */ 
/*----------------------------------------------------------------------------*/

/*
 * La fréquence du timer
 */
#define MANUX_FREQUENCE_HORLOGE 100

/*
 * Doit-on activer les "assert" ? Si cette macro n'est pas définie,
 * les assert ne produisent aucun code.
 */
#define MANUX_ASSERT_ACTIVES

/*
 * Utilisation des outils de synchronisation (mutex, semaphore, ...)
 */
//#define MANUX_OUTILS_SYNCHRO

/*
 * Définition des appels système
 */
#define MANUX_APPELS_SYSTEME

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

