/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de base du mode utilisateur de ManuX.        */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#ifndef _UNISTD_H
#define _UNISTD_H

#include <manux/types.h>

int creerNouvelleTache(void (corps()), booleen shareCons);
/*
 * Création d"une nouvelle tâche 
 *   corps : fonction à exécuter
 *   shareCons : partage-t-on la console de la tâche actuelle ?
 *   retour : le numéro de la tâche créée.
 */

int basculerTache();
/*
 * Basculer vers une autre tâche. On rend simplement la main, c'est du
 * collaboratif.
 */

int appelSystemeInutile();
/*
 * Tout est dit !
 */

int tube(int * fd);
/*
int numeroTache();
int obtenirPages(int n);
*/

#endif
