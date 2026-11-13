/**
 * @file reseau/arp.c
   @brief Implantation des fonctions ARP
                                                              (C) Chaput E. 2026
*/
#include <manux/printk.h>
#include <manux/arp.h>

/**
 * @brief Réception d'une trame ARP (et réponse !)
 */
void arpRecevoir(BufferReseau * b)
{
  (void)b;
   printkc("Reception requete ARP\n");
}

