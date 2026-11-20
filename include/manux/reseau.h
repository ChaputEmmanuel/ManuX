/**
 * @file reseau/reseau.c
 * @brief Implantation des éléments principaux du sous système réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#ifndef MANUX_RESEAU_COMMUN
#define MANUX_RESEAU_COMMUN

#include <manux/buffer-reseau.h>
#include <manux/interface-reseau.h>

/**
 * @brief La liste des buffers qui ont été reçus mais non encore
 * traités 
 */
extern ListeBufferReseau * listeBuffersRecus;

/**
 * @brief Initialisation du sous système réseau
 */
void reseauInitialiser(void);

/**
 * @brief Ajout d'une interface dans le sytème
 */
void reseauAjouterInterface(ReseauInterface * i);

/**
 * @brief Affichage de la liste de buffers de réception
 */
void reseauAfficherListeReception(void);

/**
 * @brief Réception d'un paquet sur le système
 */
void reseauRecevoirPaquet(BufferReseau *b);

#endif
