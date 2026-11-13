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

/**
 * @brief Entête Ehernet
 */
typedef struct ethernetEntete {
   uint8_t adresseDest[6];
   uint8_t adresseSrc[6];
   uint16_t etherType;
} EthernetEntete;

/**
 * @brief Traitement d'une trame reçue par Ethernet
 */
void ethernetRecevoirTrame(BufferReseau * buffer);

/**
 * void Initialisation d'une interface de type Ethernet
 */
void ethernetInitialiserInterface(ReseauInterface * ire, uint8_t a[6]);
#endif
