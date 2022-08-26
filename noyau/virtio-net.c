/**
 * @file virtio-net.c
 *
 */
#include <manux/virtio-net.h>
#include <manux/intel-8259a.h>  // pour les interruptions 
#include <manux/pci.h>          // Pour aller récupérer l'irq, on doit
				// pouvoir s'en passer avec une
				// fonction dan virtio
#include <manux/io.h>      // outb
#include <manux/debug.h>
#include <manux/errno.h>

/**
 * Les caractéristiques sp&cifiques au réseau
 */
#define VIRTIO_NET_F_CSUM                   0x00000001   //  0
#define VIRTIO_NET_F_GUEST_CSUM             0x00000002   //  1
#define VIRTIO_NET_F_CTRL_GUEST_OFFLOADS    0x00000004   //  2
#define VIRTIO_NET_F_MAC                    0x00000020   //  5
#define VIRTIO_NET_F_GUEST_TSO4             0x00000080   //  7
#define VIRTIO_NET_F_GUEST_TSO6             0x00000100   //  8
#define VIRTIO_NET_F_GUEST_ECN              0x00000200   //  9
#define VIRTIO_NET_F_GUEST_UFO              0x00000400   // 10
#define VIRTIO_NET_F_HOST_TSO4              0x00000800   // 11
#define VIRTIO_NET_F_HOST_TSO6              0x00001000   // 12
#define VIRTIO_NET_F_HOST_ECN               0x00002000   // 13
#define VIRTIO_NET_F_HOST_UFO               0x00004000   // 14
// Le pilote sait fusionner les buffers
#define VIRTIO_NET_F_MRG_RXBUF              0x00008000   // 15
#define VIRTIO_NET_F_STATUS                 0x00010000   // 16
#define VIRTIO_NET_F_CTRL_VQ                0x00020000   // 17
#define VIRTIO_NET_F_CTRL_RX                0x00040000   // 18
#define VIRTIO_NET_F_CTRL_VLAN              0x00080000   // 19 
#define VIRTIO_NET_F_GUEST_ANNOUNCE         0x00200000

/**
 * Description d'un périphérique virtio net
 */
typedef struct VirtioReseau_t {
   VirtioPeripherique    virtioPeripherique; 
   // Les éléments ci dessous sont sûrement à mettre dans une
   // structure liée au réseau
   uint8_t               adresseMAC[6];

   // Les éléments liés à virtio, à mettre dans une  structure
   // spécifique 
   uint16_t              tailleFileEmission;
   uint16_t              tailleFileReception;
   VirtioFileVirtuelle   fileEmission;
   VirtioFileVirtuelle   fileReception;
} VirtioReseau;

/**
 * WARNING : pour le moment on crée un unique périphérique
 */
VirtioReseau virtioReseau; 

/**
 * A faire : le réseau utilise un entête supplémentaire de 10 octets,
 * il faut le prendre en compte
 */


/**
 * WARNING : Une affreuse trame ARP requete
 */
uint8_t requeteARP[] = { 0,0,0,0,0,0,0,0,0,0,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff,
      0xee, 0xde, 0xad, 0xbe, 0x08, 0x06, 0x00, 0x01,
      0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xc0, 0xff,
      0xee, 0xde, 0xad, 0xbe, 0x0a, 0x00, 0x00, 0x02,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,
      0x00, 0x01
   };

/**
 * WARNING une horreur
 */
uint8_t unBufferALaCon[4096] ={0};

void virtioNetGestionInteruption(uint32_t itNum, TousRegistres registres,
                                 uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   printk_debug(DBG_KERNEL_NET, "Interuption !!!\n");
}

void virtioNetGestionInt(void * vr)
{
   printk_debug(DBG_KERNEL_NET, "Interuption !!!\n");
}

/**
 * @brief Initialisation d'un périphérique réseau virtio
 *
 * Séquence d'après [3] section 2.2.1 (voir aussi [1] section 3 mais
 * en restant prudent !)
 */
int virtioNetInitPeripherique(int PCINumeroPeripherique)
{
   PCIEquipement * pciEquip = PCIEquipementNumero(PCINumeroPeripherique);

   uint8_t addr;
   
   // On renseigne la structure
   virtioInitPeripheriquePCI(&(virtioReseau.virtioPeripherique),
			  PCINumeroPeripherique,
			  VIRTIO_NET_F_GUEST_TSO4
			  | VIRTIO_NET_F_GUEST_TSO6
			  | VIRTIO_NET_F_GUEST_UFO
			  | VIRTIO_NET_F_MRG_RXBUF
			  | VIRTIO_NET_F_CTRL_VQ);

   // Lecture de l'adresse IEEE
   for (int i = 0; i < 6; i++){
      inb(pciEquip->adresseES + 0x14 + i, addr);
      virtioReseau.adresseMAC[i] = addr;
   }

   // On définit notre fonction de gestion des interuptions
   /*definirFonctionGestionInteruption(virtioReseau.pciEquipement->interruption,
 				     virtioNetGestionInteruption);
  */
   i8259aAjouterHandler(pciEquip->interruption,
			virtioNetGestionInt,
			&virtioReseau);
   i8259aAutoriserIRQ(pciEquip->interruption);

   return 0;
}

/**
 * Émission d'une trame via une interface ...
 */
void virtioNetEmettre(VirtioReseau * vr, uint8_t * trame)
{
   printk_debug(DBG_KERNEL_NET, "IN\n");

   virtioFournirBuffer(&(vr->virtioPeripherique),
          	      (void*)unBufferALaCon,
                       4096,
		       1, // pour recevoir
		       0); // File réception

   virtioFournirBuffer(&(vr->virtioPeripherique),
		       (void*)requeteARP,
                       42+10, // Non non, ce n'est pas une blague !
		       0, // Read-Only a priori
		       1); // File émission
   
   printk_debug(DBG_KERNEL_NET, "OUT\n");
} 

/**
 * Fonction de test d'une émission. On tente naïvement d'envyer une
 * requête ARP.
 *
 * WARNING cettefonction a vocation à disparaître
 *
 */
void virtioNetTestEmission(VirtioReseau * vr)
{
   printk_debug(DBG_KERNEL_NET, "IN\n");

   virtioNetEmettre(vr, requeteARP);
   
   printk_debug(DBG_KERNEL_NET, "OUT\n");
}

/**
 * @brief Initialisation des périphériques
 */
int virtioNetInit()
{
   int PCINumeroPeripherique;
  
   printk_debug(DBG_KERNEL_NET, "IN\n");

   // On va chercher un peripherique virtio net
   PCINumeroPeripherique = PCIObtenirProchainEquipement(PCI_VENDEUR_VIRTIO,
							PCI_PERIPHERIQUE_VIRTIO_NET, -1);
   printk_debug(DBG_KERNEL_NET, "Peripherique PCI %d\n", PCINumeroPeripherique);

   // Initialisation de l'unique périphérique pour le moment
   if (virtioNetInitPeripherique(PCINumeroPeripherique)== 0) {
      printk_debug(DBG_KERNEL_NET, "Peripherique initialise !\n");
   }

   // Test d'émission
   virtioNetTestEmission(&virtioReseau);

   return ESUCCES;
}
