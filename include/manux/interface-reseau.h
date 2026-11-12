/**
 * @file reseau/reseau.c
 * @brief Implantation des éléments principaux du sous système réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#ifndef MANUX_RESEAU_INTERFACE
#define MANUX_RESEAU_INTERFACE

#include <manux/buffer-reseau.h>

/**
 * @brief Description d'un pilote d'interface réseau
 */
typedef struct _ReseauPilote {
   char * nom;
} ReseauPilote;

typedef struct _ReseauInterface {
   void (* recevoirTrame)(BufferReseau * buffer); //<! Traitement d'une trame reçue
   ReseauPilote * pilote;
   void * prive;
} ReseauInterface;

/**
 * @brief
 */
void reseauEnregistrerPilote(ReseauPilote * p);

/**
 * @brief
 */
void reseauAjouterInterface(ReseauInterface * i);


#endif
