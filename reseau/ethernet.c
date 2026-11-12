/**
 * @file reseau/ethernet.c
 * @brief Implantation des fonctions liées à Ethernet
 *
 *                                                             (C) Manu Chaput 2025-2026
 */
#include "manux/debug.h"
#include <manux/ethernet.h>
#include <manux/buffer-reseau.h>

/**
 * @brief Traitement d'une trame reçue par Ethernet
 *
 * Pour le moment, on ne fait que déterminer le protocole véhiculé
 */
void ethernetRecevoirTrame(BufferReseau * buffer) {
   EthernetEntete * eh;

   printk_debug(DBG_KERNEL_NET, "ON traite une trame Ethernet ...\n");
   // On a tout mis au début lors de la réception
   buffer->enteteLiaison = buffer->donnees;

   // On va chercher le protocole véhiculé
   eh = (EthernetEntete *)buffer->enteteLiaison;
   buffer->protocole = eh->etherType;

   printk_debug(DBG_KERNEL_NET, "Elle contient du 0x%d\n", buffer->protocole);

   // Le protocole véhiculé est juste derrière
   buffer->enteteProtocole = buffer->enteteLiaison + sizeof(EthernetEntete);
}
