/**
 * @file include/arp.h
   @brief Définition des fonctions ARP
                                                              (C) Chaput E. 2026
*/
#ifndef __ARP_DEF
#define __ARP_DEF
#include <manux/buffer-reseau.h>

/**
 * @brief Définition d'un entête ARP
 */
typedef struct _arpEntete {
   uint16_t typeLiaison;
   uint16_t typeReseau;
   uint8_t  lgLiaison;
   uint8_t  lgReseau;
   uint16_t operation;
} ArpEntete;
  
/**
 * @brief Réception d'une trame ARP (et réponse !)
 */
void arpRecevoir(BufferReseau * b);

#endif
