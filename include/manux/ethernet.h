/**
 * @file include/manux/ethernet.h
 * @brief Définition des fonctions Ethernet
 *
 *                                                             (C) Manu Chaput 2025-2026
 */
#ifndef ETHERNET_DEF
#define ETHERNET_DEF

#include <manux/buffer-reseau.h>
#include <manux/types.h>

#define ETHERNET_ADDR_LG 6

/**
 * @brief Entête Ehernet
 */
typedef struct ethernetEntete {
   uint8_t adresseDest[ETHERNET_ADDR_LG];
   uint8_t adresseSrc[ETHERNET_ADDR_LG];
   uint16_t etherType;
} EthernetEntete;

/**
 * @brief Traitement d'une trame reçue par Ethernet
 */
void ethernetRecevoirTrame(BufferReseau * buffer);

/**
 * void Initialisation d'une interface de type Ethernet
 */
void ethernetInitialiserInterface(ReseauInterface * ire, uint8_t a[ETHERNET_ADDR_LG]);

/**
 * @brief Initialisation de l'entête Ethernet d'un buffer
 */
void ethernetInitialiserEntete(ReseauInterface * i, BufferReseau * b, uint8_t * a);

#endif
