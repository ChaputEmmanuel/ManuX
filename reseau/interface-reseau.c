/**
 * @file reseau/interface-reseau.c
 * @brief Implantation des interfaces réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#include <manux/interface-reseau.h>

void reseauEnregistrerPilote(ReseauPilote * p)
{
   (void) p;
}

/**
 * @brief Initialisation d'une interface
 */
void interfaceReseauInitialiser(ReseauInterface * i)
{
   i->nbReceptions =0;
   i->nbEmissions = 0;
   i->receptionEnAttente = FALSE;
}
