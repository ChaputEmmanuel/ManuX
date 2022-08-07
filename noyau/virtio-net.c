/**
 * @file virtio-net.c
 *
 * WARNING : A terme, si un jour ça marche, il faudra mettre dans
 * virtio.h et virtio.c ce qui est général à virtio, ... Ca pourra
 * servir pour un autre type d'équipement.
 *
 * WARNING : on utilise ici en fait les spécifications v0.9.5,
 * qualifiées de "legacy" dans la v1.0. J'utiliserai autant que
 * possible des macros/variables du genre VIRTIO_HIST... pour faire la
 * différence chaque fois que possible. Attention !! Il y a des
 * petites différences entre la v0.9.5 et la version dite legacy dans
 * la v1 !! Apparemment une typo dans la v1 sur l'ordre des champs de
 * contrôle ...
 *
 * Sources intéressantes 
 *   [1] http://docs.oasis-open.org/virtio/virtio/v1.0/csprd01/virtio-v1.0-csprd01.html 
 *   [2] https://www.redhat.com/en/blog/virtqueues-and-virtio-ring-how-data-travels    
 *   [3] http://ozlabs.org/~rusty/virtio-spec/virtio-0.9.5.pdf
 */
#include <manux/virtio-net.h>
#include <manux/debug.h>
#include <manux/pci.h>
#include <manux/io.h>      // outb
#include <manux/string.h>  // memcpy
#include <manux/memoire.h> // allouerPages
#include <manux/errno.h>

#define VIRTIO_ACKNOWLEDGE 0x01
#define VIRTIO_DRIVER      0x02
#define VIRTIO_DRIVER_OK   0x04
#define VIRTIO_FEATURES_OK 0x08
/**
 * Les caractéristiques
 */
#define VIRTIO_NET_F_CSUM                   0x00000001
#define VIRTIO_NET_F_GUEST_CSUM             0x00000002
#define VIRTIO_NET_F_CTRL_GUEST_OFFLOADS    0x00000004
#define VIRTIO_NET_F_MAC                    0x00000020
#define VIRTIO_NET_F_GUEST_TSO4             0x00000080
#define VIRTIO_NET_F_GUEST_TSO6             0x00000100
#define VIRTIO_NET_F_GUEST_ECN              0x00000200
#define VIRTIO_NET_F_GUEST_UFO              0x00000400
#define VIRTIO_NET_F_HOST_TSO4              0x00000800
#define VIRTIO_NET_F_HOST_TSO6              0x00001000
#define VIRTIO_NET_F_HOST_ECN               0x00002000
#define VIRTIO_NET_F_HOST_UFO               0x00004000
// Le pilote sait fusionner les buffers
#define VIRTIO_NET_F_MRG_RXBUF              0x00008000
#define VIRTIO_NET_F_STATUS                 0x00010000
#define VIRTIO_NET_F_CTRL_VQ                0x00020000
#define VIRTIO_NET_F_CTRL_RX                0x00040000
#define VIRTIO_NET_F_CTRL_VLAN              0x00080000
#define VIRTIO_NET_F_GUEST_ANNOUNCE         0x00200000

#define VIRTIO_F_RING_INDIRECT_DESC         0x10000000
#define VIRTIO_F_RING_EVENT_IDX             0x20000000

/**
 * La position des champs principaux de configuration d'une interface
 * conforme à la spécification 0.9.5 [3] (dite "legacy" dans [1] mais
 * avec une orgnisation différente!)
 */
#define VIRTIO_HIST_CAPA_EQUIP     0x00  // Les capacités de l'équipement
#define VIRTIO_HIST_CAPA_PILOTE    0x04  // Les capacités du pilote
#define VIRTIO_HIST_ADDRESS_FILE   0x08  // Numéro de page de la file
					 // sélectionnée
#define VIRTIO_HIST_TAILLE_FILE    0x0C  // Taille de la file
#define VIRTIO_HIST_FILE_SEL       0x0E  // Sélection de la file 
#define VIRTIO_HIST_FILE_NOTIF     0x10  // Notification de la file
#define VIRTIO_HIST_ETAT           0x12  // Etat du périph et du pilote

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
 * Description d'un périphérique virtio net
 */
typedef struct VirtioReseau_t {
   PCIEquipement       * pciEquipement;  // Pointeur sur les caractéristiques PCI
   uint32_t              caracteristiques;
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
 * WARNING : Une affreuse trame ARP requete
 */
   uint8_t requeteARP[] = {
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff,
      0xee, 0xde, 0xad, 0xbe, 0x08, 0x06, 0x00, 0x01,
      0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0xc0, 0xff,
      0xee, 0xde, 0xad, 0xbe, 0x0a, 0x00, 0x00, 0x02,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,
      0x00, 0x01
   };

/**
 * Création des files de communication.
 *
 * D'après [3] (section 2.3), une file est constituée d'une zone
 * mémoire contigüe contenant 
 *   . Une table des descripteurs de buffer (alignée sur une page)
 *   . Une structure de description des buffers disponibles
 *   . Une structure de description des buffers usagers (alignée sur
 * une page)
 * 
 * On va donc calculer la taille de chacune de ces deux parties
 * (alignées sur une page).
 */
int virtioNetCreerFileVirtuelle(VirtioFileVirtuelle * fileVirtuelle,
                       uint16_t tailleFile)
{
   uint32_t taillePartie1; // Buffers + disponibles
   uint32_t taillePartie2; // Utilisés
   uint32_t nbTotalPages;  // Combien de pages pour stocker ça ?
   char *   pointeur;      // Pour les allocations
   
    // La première partie est composée des descripteurs de buffer
   taillePartie1 = tailleFile * sizeof(VirtioDescripteurBuffer);

   // Suivis des descripteurs de buffers disponibles
   taillePartie1 += 3 * sizeof(uint16_t) + tailleFile * sizeof(uint16_t);

   // La seconde partie est composée des buffers utilisés
   taillePartie2 =  3 * sizeof(uint16_t) + tailleFile * sizeof(VirtioElementUtilise);

   // Combien faut-il de pages avec les contraintes d'alignement ?
   nbTotalPages = NB_PAGES(taillePartie1) + NB_PAGES(taillePartie2);
   
   // Allocation des pages nécessaires
   pointeur = allouerPages(nbTotalPages);

   if (pointeur == NULL) {
      return ENOMEM;
   }

   // La spécification insiste sur une initialisation à 0
   memset(pointeur, 0, nbTotalPages * MANUX_TAILLE_PAGE);
   
   // On peut enfin initialiser les pointeurs
   fileVirtuelle->tableDescripteurs = (VirtioDescripteurBuffer *)pointeur;

   // Les buffers disponibles sont collés derrière le dernire buffer
   fileVirtuelle->buffersDisponibles =
     (VirtioBufferDisponible *)&(fileVirtuelle->tableDescripteurs[tailleFile]);

   // Les buffers utilisés sont au début de la page suivante
   fileVirtuelle->buffersUtilises =
     (VirtioBufferUtilise*) (pointeur + NB_PAGES(taillePartie1) * MANUX_TAILLE_PAGE);
   
   printk_debug(DBG_KERNEL_NET, "%d elts, %d+%d octets, %d pages, desc=0x%x, dis=0x%x, uti=0x%x,\n",
		tailleFile,
		taillePartie1,
		taillePartie2,
		nbTotalPages,
		fileVirtuelle->tableDescripteurs,
		fileVirtuelle->buffersDisponibles,
		fileVirtuelle->buffersUtilises);

   return ESUCCES;
}

/**
 * Émission d'un buffer sur une file virtuelle
 */
void virtioEmission(VirtioFileVirtuelle * fileVirtuelle, VirtioDescripteurBuffer * b)
{
   printk_debug(DBG_KERNEL_NET, "IN\n");
   printk_debug(DBG_KERNEL_NET, "OUT\n");
}

void virtioNetGestionInteruption(uint32_t itNum, TousRegistres registres,
                                 uint32_t eip, uint32_t cs, uint32_t eFlags)
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
   PCIEquipement * pciEqu = PCIEquipementNumero(PCINumeroPeripherique);
   uint32_t        caracteristiques;
   uint8_t         adresseMAC[6];
   uint8_t         etat;
   uint16_t        tailleFile;   // Pour lire les tailles de files
				 // virtio
   /**
    * L'essentiel de ce qui suit est conforme à la v0.9.5 a priori,
    * pas à la v1.0
    */
   // On reset le périphérique
   outb(pciEqu->adresseES + VIRTIO_HIST_ETAT, 0x00);
   
   // On lui dit qu'on l'a vu
   outb(pciEqu->adresseES + VIRTIO_HIST_ETAT, VIRTIO_ACKNOWLEDGE);
   
   // On lui dit qu'on va le gérer
   outb(pciEqu->adresseES + VIRTIO_HIST_ETAT, VIRTIO_ACKNOWLEDGE|VIRTIO_DRIVER);

   // Lecture de ses caractéristiques
   inl(pciEqu->adresseES + VIRTIO_HIST_CAPA_EQUIP, caracteristiques);

   // On sélectionne les caractéristiques qui nous intéressent
   caracteristiques = caracteristiques
     & (~(
	  VIRTIO_F_RING_INDIRECT_DESC
	| VIRTIO_NET_F_CTRL_VQ
	| VIRTIO_NET_F_GUEST_TSO4
	| VIRTIO_NET_F_GUEST_CSUM
	| VIRTIO_NET_F_GUEST_TSO4
	| VIRTIO_NET_F_GUEST_TSO6
	| VIRTIO_NET_F_GUEST_UFO
	| VIRTIO_NET_F_MRG_RXBUF
	| VIRTIO_NET_F_GUEST_ANNOUNCE
	  ));

   // Négociation de ces caractéristiques
   outl(pciEqu->adresseES + VIRTIO_HIST_CAPA_PILOTE, caracteristiques);

   // On acte définitivement ces caractéristiques
   outb(pciEqu->adresseES + VIRTIO_HIST_ETAT, VIRTIO_ACKNOWLEDGE
	                        | VIRTIO_DRIVER
	                        | VIRTIO_FEATURES_OK);

   // On vérifie qu'il est OK
   inb(pciEqu->adresseES + VIRTIO_HIST_ETAT, etat);

   if ((etat & VIRTIO_FEATURES_OK) == 0) {
      printk_debug(DBG_KERNEL_NET, "Le device virtio net refuse les caract. !\n");
      return -1;
   }
   
   // Lecture des tailles de file. On n'a que deux files puisqu'on ne
   // gère pas le CTRL_VQ
   outw(pciEqu->adresseES + VIRTIO_HIST_FILE_SEL, 0);
   inw(pciEqu->adresseES + VIRTIO_HIST_TAILLE_FILE, tailleFile);
   virtioReseau.tailleFileReception = tailleFile;
   
   outw(pciEqu->adresseES + VIRTIO_HIST_FILE_SEL, 1);
   inw(pciEqu->adresseES + VIRTIO_HIST_TAILLE_FILE, tailleFile);
   virtioReseau.tailleFileEmission = tailleFile;

   // Création des fameuses files
   virtioNetCreerFileVirtuelle(&virtioReseau.fileReception,
			       virtioReseau.tailleFileReception);
   virtioNetCreerFileVirtuelle(&virtioReseau.fileEmission,
			       virtioReseau.tailleFileEmission);

  // On donne l'adresse à l'équipement
   outw(pciEqu->adresseES + VIRTIO_HIST_FILE_SEL, 0);
   outl(pciEqu->adresseES + VIRTIO_HIST_ADDRESS_FILE,
	((uint32_t)virtioReseau.fileReception.tableDescripteurs)/MANUX_TAILLE_PAGE);   
   outw(pciEqu->adresseES + VIRTIO_HIST_FILE_SEL, 1);
   outl(pciEqu->adresseES + VIRTIO_HIST_ADDRESS_FILE,
	((uint32_t)virtioReseau.fileEmission.tableDescripteurs)/MANUX_TAILLE_PAGE);   
     
   // On définit notre fonction de gestion des interuptions
   definirFonctionGestionInteruption(32 + pciEqu->interruption,
				     virtioNetGestionInteruption);
   
   // Lecture de l'adresse IEEE
   for (int i = 0; i < 6; i++){
      inb(pciEqu->adresseES + 0x14 + i, etat);
      adresseMAC[i] = etat;
   }

   // On dit au périphérique qu'on est bon, ...
   outb(pciEqu->adresseES + VIRTIO_HIST_ETAT,
	VIRTIO_ACKNOWLEDGE
	| VIRTIO_DRIVER
	| VIRTIO_FEATURES_OK
	| VIRTIO_DRIVER_OK);

   // Un petit message de debug
   printk_debug(DBG_KERNEL_NET, "Dev %d (irq %d, @ 0x%x, mac %x:%x:%x:%x:%x:%x) : 0x%x\n",
		PCINumeroPeripherique,
		pciEqu->interruption,
		pciEqu->adresseES,
		adresseMAC[0],
		adresseMAC[1],
		adresseMAC[2],
		adresseMAC[3],
		adresseMAC[4],
		adresseMAC[5],
		caracteristiques);

   return 0;
}

/**
 * Émission d'une trame via une interface ...
 */
void virtioNetEmettre(VirtioReseau * vr, uint8_t * trame)
{
   // WARNING Comment sait-on quels descripteurs de buffer sont utilisés?
   int prochainDescripteurAUtiliser = 0;
   int    attentionCestPasZero = 0;

      printk_debug(DBG_KERNEL_NET, "IN\n");

   // On utiliser la file d'émission
   VirtioFileVirtuelle * fr = &(vr->fileEmission);

   // On va chercher un descripteur de buffer
   VirtioDescripteurBuffer * vb = &(fr->tableDescripteurs[prochainDescripteurAUtiliser]);

   // On l'initalise en lecture
   vb->flags = 0 ; // Read-Only a priori
   vb->adresse = (uint64_t)requeteARP;
   vb->longueur = 42; // Non non, ce n'est pas une blague !

   // On le met dans les disponibles
   fr->buffersDisponibles->indicesDesBuffer[attentionCestPasZero] =
     prochainDescripteurAUtiliser;

   fr->buffersDisponibles->indice++;

   // On prévient Jean-Pierre
   outw(vr->pciEquipement->adresseES + VIRTIO_HIST_FILE_NOTIF, 1);
   
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

   PCINumeroPeripherique = PCIObtenirProchainEquipement(PCI_VENDEUR_VIRTIO,
							PCI_PERIPHERIQUE_VIRTIO_NET, -1);
   printk_debug(DBG_KERNEL_NET, "Peripherque PCI %d\n", PCINumeroPeripherique);

   // Initialisation de l'unique périphérique pour le moment
   if (virtioNetInitPeripherique(PCINumeroPeripherique)== 0) {
      printk_debug(DBG_KERNEL_NET, "Peripherique initialise !\n");
   }

   // Test d'émission
   virtioNetTestEmission(&virtioReseau);

   return ESUCCES;
}
