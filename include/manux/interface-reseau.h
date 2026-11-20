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
   ReseauPilote * pilote;
   void * prive;

   // La fonction suivante constitue la partie basse de la réception
   // d'une trame sur l'interface
   void (* recevoirTrame)(ReseauInterface *);
   booleen receptionEnAttente;
  
   // Une interface peut avoir une adresse (eg Ethernet) de longueur
   // lgAdresse 
   uint8_t adresse[MANUX_ADRESSE_MAC_LG_MAX];
   int lgAdresse;  // 0 si pas d'adresse

   // S'il y a une adresse (lgAdresse > 0), il put y avoir une adresse
   // de diffusion
   uint8_t adresseDiffusion[MANUX_ADRESSE_MAC_LG_MAX];

   // Quelques stats
   uint32_t nbReceptions;
   uint32_t nbEmissions;
} ReseauInterface;

/**
 * @brief
 */
void reseauEnregistrerPilote(ReseauPilote * p);

/**
 * @brief Initialisation d'une interface
 */
void interfaceReseauInitialiser(ReseauInterface * i);

/**
 * @brief Émission d'une trame via une interface réseau
 *
 * Le buffer b doit être initialisé et contenir tout ce concerne les
 * couches supérieures à la couche liaison. L'adresse du destinataire
 * est passée en paramètre.
 */
int reseauInterfaceEmettre(ReseauInterface * i,
			   BufferReseau * b,
			   uint8_t * adresse);

#endif
