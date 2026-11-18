/**
 * @file virtio-net.h
 *                                                       (C) E. Chaput 2022-2026
 */
#ifndef VIRTIO_NET_DEF
#define VIRTIO_NET_DEF

#include <manux/virtio.h>

/**
 * @brief Initialisation des périphériques
 *
 * Cette fonction est sensée chercher toutes les interfaces réseaux
 * virtio et les configurer. Pour le moment elle n'en gère qu'une !
 */
int virtioNetInit(void);

/**
 * @brief Cette fonction de test est appelée à disparaître
 */
void virtioNetTestDeuxiemeEmission(void);

void virtioReseauPoll(void);

#endif
