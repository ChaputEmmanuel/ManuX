/*----------------------------------------------------------------------------*/
/*      Définition des éléments liés à la gestion de l'horloge matérielle.    */
/*                                                                            */
/*                                                (C) Manu Chaput 2000 - 2021 */
/*----------------------------------------------------------------------------*/
#ifndef HORLOGE_DEF
#define HORLOGE_DEF

#include <manux/types.h>   // Temps

/*
 * Le compteur incrémenté par l'horloge matérielle
 */
extern Temps nbTopHorloge;

/*
 * Initialisation de la gestion de l'horloge matérielle
 */
void initialiserHorloge();

#endif
