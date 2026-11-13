/**
 * @file include/arp.h
   @brief Définition des fonciotns ARP
                                                              (C) Chaput E. 2026
*/
#ifndef __ARP_DEF
#define __ARP_DEF
#include <manux/buffer-reseau.h>

/**
 * @brief Réception d'une trame ARP (et réponse !)
 */
void arpRecevoir(BufferReseau * b);

#endif
