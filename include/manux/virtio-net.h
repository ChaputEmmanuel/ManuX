/**
 * @file virtio-net.h
 */
#ifndef VIRTIO_NET_DEF
#define VIRTIO_NET_DEF

#define PCI_VENDEUR_VIRTIO            0x1AF4
#define PCI_PERIPHERIQUE_VIRTIO_NET   0x1000

/**
 * @brief Initialisation des périphériques
 */
int virtioNetInit();

#endif
