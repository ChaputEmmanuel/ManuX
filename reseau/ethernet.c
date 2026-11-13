/**
 * @file reseau/ethernet.c
 * @brief Implantation des fonctions liées à Ethernet
 *
 *                                                             (C) Manu Chaput 2025-2026
 */
#include <manux/debug.h>
#include <manux/ethernet.h>
#include <manux/buffer-reseau.h>
#include <manux/interface-reseau.h>
#include <arpa/inet.h>
#include <manux/string.h>

/**
 * @brief Traitement d'une trame reçue par Ethernet
 *
 * Pour le moment, on ne fait que déterminer le protocole véhiculé
 */
void ethernetRecevoirTrame(BufferReseau * buffer) {
   EthernetEntete * eh;

   // On a tout mis au début lors de la réception
   buffer->enteteLiaison = buffer->donnees;

   // On va chercher le protocole véhiculé
   eh = (EthernetEntete *)buffer->enteteLiaison;
   buffer->protocole = htons(eh->etherType);

   printk_debug(DBG_KERNEL_NET, "On traite une trame Ethernet contenant 0x%x\n", buffer->protocole);

   // Le protocole véhiculé est juste derrière
   buffer->enteteProtocole = buffer->enteteLiaison + sizeof(EthernetEntete);
}

/**
 * @brief Initialisation d'une interface de type Ethernet
 */
void ethernetInitialiserInterface(ReseauInterface * ire, uint8_t a[6])
{
   ire->recevoirTrame = ethernetRecevoirTrame;
   memcpy(ire->adresse, a, 6);
   ire->lgAdresse = 6;
}
