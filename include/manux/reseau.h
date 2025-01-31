/**
 * @file reseau/reseau.c
 * @brief Implantation des éléments principaux du sous système réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#ifndef MANUX_RESEAU_COMMUN
#define MANUX_RESEAU_COMMUN

#include <manux/buffer-reseau.h>

/**
 * @brief La liste des buffers qui ont été reçus mais non encore
 * traités 
 */
extern ListeBufferReseau * listeBuffersRecus;

/**
 * @brief Initialisation du sous système réseau
 */
void reseauInitialiser();

/**
 * @brief
 */
void reseauEnregistrerPilote(ReseauPilote * p);

/**
 * @brief
 */
void reseauAjouterInterface(ReseauPilote * p, void * d);

#endif
