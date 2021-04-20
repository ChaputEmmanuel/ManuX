/*----------------------------------------------------------------------------*/
/*      Définition des sous-programmes de manipulation de la mémoire sous     */
/*   ManuX.                                                                   */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef MEMOIRE_DEF
#define MEMOIRE_DEF

#include <manux/appelsysteme.h> /* ParametreAS */

#ifndef NULL
#   define NULL 0
#endif

#define DEBUT_MEMOIRE_ETENDUE 0x100000

/*
 * Nombre de pages communes à toutes les tâches.
 */
extern int nombrePagesSysteme;

void initialiserMemoire(unsigned int tailleMemoireEtendue);
/*
 * Initialisation de la mémoire. Nécessaire avant toute
 * demande d'allocation.
 */

void * allouerPage();
/*
 * Réservation d'une page (de 4 Ko) au delà de la zone système.
 *
 * Retour
 *    première adresse dispo si la pages a pu être allouée,
 *    NULL sinon
 */

void * allouerPageSysteme();
/*
 * Réservation d'une page (de 4 Ko) dans la zone système.
 *
 * Retour
 *    première adresse dispo si la pages a pu être allouée,
 *    NULL sinon
 */

void * allouerPages(unsigned int nombre);
/*
 * Réservation d'un nombre choisi de pages (de 4 Ko) contigues.
 *
 * Retour
 *    première adresse dispo si les pages ont pu être allouées,
 *    NULL sinon
 * WARNING à mettre en appel système ?
 */

void libererPage(void * pageLiberee);
/*
 * Libération d'une page préalablement allouée. Attention, aucune
 * vérification n'est effectuée.
 */

int AS_obtenirPages(ParametreAS p, int nbPages);
/*
 * Demande d'accroissement de la zone mémoire disponible à
 * la tâche.
 * Retour : nombre de pages obtenues.
 */

#endif
