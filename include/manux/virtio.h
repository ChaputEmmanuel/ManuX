/**
 * @file virtio.h
 *
 * WARNING : je voudrais mettre toutes les définitions de types dans
 * le .c, mais le soucis c'est qu'elles doivent ensuite être utilisées
 * via des pointeurs (cf le premier champs de virtioReseauPeriph) et
 * comme pour le moment je n'ai pas de malloc, ...
 */
#ifndef VIRTIO_DEF
#define VIRTIO_DEF

#include <manux/pci.h>          // virtio est un système PCI
#include <manux/types.h>

/**
 * Toujours pour éviter de jouer avec des structures dynamiques, nous
 * allons limiter le nombre de files virtuelles de chaque équipement
 * de type virtio.
 * Tant qu'on ne fait que des périphériques réseau, on n'a besoin que
 * de deux files, ...
 */
#ifndef MANUX_VIRTIO_NB_MAX_FILES
#   define MANUX_VIRTIO_NB_MAX_FILES 2
#endif

#define PCI_VENDEUR_VIRTIO            0x1AF4
#define PCI_PERIPHERIQUE_VIRTIO_NET   0x1000

/**
 * Description d'un buffer (voir par exemple struct vring_desc dans [1]
 * Section 2.4.4 ou dans [3] annexe A)
 * 
 */
typedef struct __attribute__((__packed__)) _VirtioDescripteurBuffer {
   uint64_t adresse;
   uint32_t longueur;
   uint16_t flags;
   uint16_t suivant;
} VirtioDescripteurBuffer;

/**
 * Description d'un buffer "disponible" ([3] Section 2.3.4)
 */
typedef struct _VirtioBufferDisponible {
   uint16_t flags;
   uint16_t indice;
   uint16_t indicesDesBuffer[];
   //uint16_t evenementUtilise; // On ne peut pas le déclarer !
} VirtioBufferDisponible;

/**
 * Description des éléments utilisés
 */
typedef struct _VirtioElementUtilise {
   uint32_t indiceBuffer; // Dans la table des descripteurs
   uint32_t longueur;     // Le nombre d'octets utilisés
} VirtioElementUtilise;

/**
 * Description d'un buffer "utilisé"
 */
typedef struct _VirtioBufferUtilise {
   uint16_t              flags;
   uint16_t              indice;
   VirtioElementUtilise  elementsUtilises[];
   //uint16_t            evenementDisponible; // On ne peut pas le déclarer
} VirtioBufferUtilise;

typedef struct _VirtioFileVirtuelle {
  VirtioDescripteurBuffer * tableDescripteurs; // La table des
					       // descripteurs de
					       // buffer
  VirtioBufferDisponible  * buffersDisponibles ;
  VirtioBufferUtilise     * buffersUtilises;  
} VirtioFileVirtuelle;

/**
 * Description d'un périphérique virtio
 */
typedef struct _VirtioPeripherique {
   PCIEquipement       * pciEquipement;  // Pointeur sur les caractéristiques PCI
   uint32_t              caracteristiques;
   VirtioFileVirtuelle   filesVirtuelles[MANUX_VIRTIO_NB_MAX_FILES];    
} VirtioPeripherique;

/**
 * Initialisation d'un périphérique de type virtio accédé en PCI.
 * L'essentiel de ce qui suit est conforme à la v0.9.5 a priori,
 * pas à la v1.0
 *
 * @param vp      pointeur sur une structure déjà allouée (ou statique)
 * @param PCINum  l'identifiant PCI
 * @param masque  les caractéristiques à *ne pas* utiliser
 */
int virtioInitPeripheriquePCI(VirtioPeripherique * vp,
     		       int PCINum,
  			       uint32_t masque);

/**
 * Fournir un buffer au périphérique.
 * @param fv   la file sur laquelle placer ce buffer
 * @param bu   le buffer (pointeur sur les données/la place)
 * @param lg   la taille du buffer
 * @param fl   lecture/écriture
 * @param id   index de la file
 *
 */
void virtioFournirBuffer(VirtioPeripherique * vp,
			 void * bu, int lg, int fl,
			 uint16_t id);

#endif
