/*----------------------------------------------------------------------------*/
/*      Définition des sous-programme de manipulation de la mémoire au niveau */
/*   d'une tache sous ManuX. Ces sous-programmes devront faire partie de la   */
/*   librairie, et non du système.                                            */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_MALLOC_DEF
#define MANUX_MALLOC_DEF

#ifndef NULL
#   define NULL ((void *)0)
#endif

void * malloc(int taille);
/*
 * Allocation d'une zone mémoire de taille voulue.
 *
 * Retour
 *    adresse de la zone si possible
 *    0 si impossible
 *
 * Attention la taille doit être <= 4092 octets
 */

void free(void * pointeur);
/*
 * Libération d'une zone mémoire précédemment allouée.
 */

int initialiserMalloc();
/*
 * Initialisation du gestionnaire de mémoire. A appeler une fois et une
 * seule par tâche. Idéalement cet appel sera caché dans la phase
 * d'initialisation du processus.
 *
 * Retour
 *
 *    0       si tout va bien
 *    ERREUR  sinon
 */

#endif
