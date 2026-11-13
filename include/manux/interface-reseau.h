/**
 * @file reseau/reseau.c
 * @brief Implantation des éléments principaux du sous système réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#ifndef MANUX_RESEAU_INTERFACE
#define MANUX_RESEAU_INTERFACE

#include <manux/buffer-reseau.h>

#define MANUX_ADRESSE_MAC_LG_MAX 6

/**
 * @brief Description d'un pilote d'interface réseau
 */
typedef struct _ReseauPilote {
   char * nom;
} ReseauPilote;

/**
 * @brief Définition d'une interface réseau générale
 */
typedef struct _ReseauInterface {
   void (* recevoirTrame)(BufferReseau * buffer); //<! Traitement d'une trame reçue
   ReseauPilote * pilote;
   void * prive;

   // Une interface peut avoir une adresse (eg Ethernet) de longueur
   // lgAdresse 
   uint8_t adresse[MANUX_ADRESSE_MAC_LG_MAX];
   int lgAdresse;  // 0 si pas d'adresse
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
