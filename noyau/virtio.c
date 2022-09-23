/**
 * @file virtio.c
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

#define min(a, b)   (((a)<(b))?(a):(b))

#include <manux/virtio.h>
#include <manux/debug.h>
#include <manux/memoire.h> // allouerPages
#include <manux/string.h>  // memcpy
#include <manux/i386.h>    // barriereMemoire
#include <manux/errno.h>
#include <manux/io.h>      // outb

/**
 * Création d'une file de communication.
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
int virtioCreerFileVirtuelle(VirtioFileVirtuelle * fileVirtuelle,
                             uint16_t tailleFile)
{
   uint32_t taillePartie1; // Buffers + disponibles
   uint32_t taillePartie2; // Utilisés
   uint32_t nbTotalPages;  // Combien de pages pour stocker ça ?
   char *   pointeur;      // Pour les allocations

   fileVirtuelle->taille = tailleFile;
   
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

   // En cas d'échec, on remonte l'erreur
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

   fileVirtuelle->prochainDescripteur = 0;

   // Pour savoir où on en est des récupérations de buffer
   fileVirtuelle->dernierIndiceUtilise = 0;
   
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
			       uint32_t masque)
{
   uint16_t adresseES;  // L'adresse d'E/S pour dialoguer avec lui
   uint16_t tailleFile; // Pour lire les tailles de files virtio
   uint8_t  etat;       // Etat du périphérique   
   int      numFile;    // Pour boucler sur les files
   
   // On va chercher un pinteur vers la description PCI
   vp->pciEquipement = PCIEquipementNumero(PCINum);

   // Un raccourci sur l'adresse pour alléger le code
   adresseES = vp->pciEquipement->adresseES;

   // On reset le périphérique
   outb(adresseES + VIRTIO_HIST_ETAT, VIRTIO_RESET);
   
   // On lui dit qu'on l'a vu
   outb(adresseES + VIRTIO_HIST_ETAT, VIRTIO_ACKNOWLEDGE);
   
   // On lui dit qu'on va le gérer
   outb(adresseES + VIRTIO_HIST_ETAT, VIRTIO_ACKNOWLEDGE|VIRTIO_DRIVER);

   // Lecture de ses caractéristiques
   inl(adresseES + VIRTIO_HIST_CAPA_EQUIP, vp->caracteristiques);

   // Un petit message de debug
   printk_debug(DBG_KERNEL_NET, "Caract : 0x%x\n",
		vp->caracteristiques);

   // C'est donc la phase de "négociation", on va supprimer ce qu'on
   // ne sait pas gérer
   masque |= VIRTIO_F_RING_EVENT_IDX;  //| VIRTIO_F_RING_INDIRECT_DESC
     
   printk_debug(DBG_KERNEL_NET, "Masque : 0x%x\n",
		masque);
   
   // On désélectionne les caractéristiques qui ne nous intéressent pas
   vp->caracteristiques &= ~masque ;

   printk_debug(DBG_KERNEL_NET, "Caract : 0x%x\n",
		vp->caracteristiques);

   // Négociation de ces caractéristiques
   outl(adresseES + VIRTIO_HIST_CAPA_PILOTE, vp->caracteristiques);

   // On acte définitivement ces caractéristiques
   outb(adresseES + VIRTIO_HIST_ETAT,
	VIRTIO_ACKNOWLEDGE | VIRTIO_DRIVER | VIRTIO_FEATURES_OK);

   // On vérifie qu'il est OK
   inb(adresseES + VIRTIO_HIST_ETAT, etat);

   if ((etat & VIRTIO_FEATURES_OK) == 0) {
      printk_debug(DBG_KERNEL_NET, "Le device virtio net refuse les caract. !\n");
      return EINVAL;
   }

   // Construction des files
   for (numFile = 0 ; numFile < MANUX_VIRTIO_NB_MAX_FILES; numFile++) {
      outw(adresseES + VIRTIO_HIST_FILE_SEL, numFile);
      inw(adresseES + VIRTIO_HIST_TAILLE_FILE, tailleFile);

      if (tailleFile) {
         // Création des fameuses files
         virtioCreerFileVirtuelle(&(vp->filesVirtuelles[numFile]),   
	                          tailleFile);

         // On donne l'adresse à l'équipement
         outw(adresseES + VIRTIO_HIST_FILE_SEL, numFile);
         outl(adresseES + VIRTIO_HIST_ADDRESS_FILE,
         ((uint32_t)vp->filesVirtuelles[numFile].tableDescripteurs)/MANUX_TAILLE_PAGE);   
      }     
   }
   
   // On dit au périphérique qu'on est bon, ...
   outb(adresseES + VIRTIO_HIST_ETAT,
	VIRTIO_ACKNOWLEDGE
	| VIRTIO_DRIVER
	| VIRTIO_FEATURES_OK
	| VIRTIO_DRIVER_OK);

   return ESUCCES;
}

/**
 * @brief Fourniture d'un buffer au périphérique.
 *
 * @param vp   le périphérique concerné
 * @param id   numéro de la file sur le périphérique
 * @param bu   le buffer (pointeur sur les données/la place)
 * @param lg   la taille du buffer
 * @param fl   lecture/écriture 
 *
 */
void virtioFournirBuffer(VirtioPeripherique * vp,
			 uint16_t id,
			 void * bu, int lg, int fl)
{
   virtioFournirBuffers(vp, id, &bu, &lg, 1, fl);
}

/**
 * @brief Fournir plusieurs buffers au périphérique.
 *
 * @param fv   la file sur laquelle placer ce buffer
 * @param id   index de la file
 * @param bu   les buffers (tableau de pointeurs sur les données/la place)
 * @param lg   la taille des buffers
 * @param nb   le nombre de buffers
 * @param fl   lecture/écriture
 * @return     le nombre de buffers réellement fournis
 * 
 * L'idée est de fournir une chaîne de buffers . WARNING pas d'erreur gérée !
 */
int virtioFournirBuffers(VirtioPeripherique * vp,
                         uint16_t id,
			 void * bu[], int lg[], int nb,
			 int fl)
{
   VirtioFileVirtuelle * fv = &(vp->filesVirtuelles[id]);
   VirtioDescripteurBuffer * vb;
   int prochainDesc = fv->prochainDescripteur;
   int n;  // Pour boucler sur les buffers

   for (n = 0 ; (n < nb) && (prochainDesc + n < fv->taille); n++) {
      // On va chercher un descripteur de buffer (2.4.1.1 (a))
      vb = &(fv->tableDescripteurs[prochainDesc + n]);

      // L'adresse des données (2.4.1.1 (b))
      vb->adresse = (uint64_t)bu[n];
      printk_debug(DBG_KERNEL_NET, "On envoie @ 0x%x\n", vb->adresse);
      // La longueur des données (2.4.1.1 (c))
      vb->longueur = lg[n]; 
   
      // On l'initialise en lecture
      vb->flags = fl;

      // On chaîne
      if (n < nb -1) {
         vb->flags |= VRING_DESC_F_NEXT;
         vb->suivant = fv->prochainDescripteur + n + 1;
  	 if (vb->suivant  >= fv->taille) {
	    vb->suivant = 0;
	 }
      } else {
         vb->suivant = 0;
      }
      
      // On le met dans les disponibles (2.4.1.2)
      fv->buffersDisponibles->indicesDesBuffer
	[(fv->buffersDisponibles->indice+n)%fv->taille] =
         prochainDesc;
   }

   barriereMemoire();  // WARNING, c'est là ?

   // On incremente le nombre de dispo (2.4.1.3)
   fv->buffersDisponibles->indice = (fv->buffersDisponibles->indice + n)%fv->taille;

   // On a utiliser n buffers
   fv->prochainDescripteur = (fv->prochainDescripteur + n) % fv->taille;
   
   barriereMemoire();
   
   // On prévient l'équipement
   outw(vp->pciEquipement->adresseES + VIRTIO_HIST_FILE_NOTIF, id);

   return n;
}

/**
 * @brief on va récupérer des buffers
 * 
 * @return Le nombre de pointeurs initialisés
 *
 * Voir [3] section 2.4.2 "Receiving Used Buffers From The Device"
 *
 * On fait au plus simple : on renvoie un(e chaîne de) buffer(s) tel
 * que fourni par le périphérique.
 */
int virtioFileRecupererBuffers(VirtioFileVirtuelle * fv, void * bu[], int lg[], int nb)
{
   uint16_t indiceBuffer;
   uint32_t lgTotale;       // Nombre d'octets significatifs
   int      result = 0;

   printk_debug(DBG_KERNEL_NET, "IN\n");
   printk_debug(DBG_KERNEL_NET, "Pouet %d/%d\n", fv->dernierIndiceUtilise, fv->buffersUtilises->indice);
   for (int n = 0 ; n < fv->buffersUtilises->indice; n++ ){
     printk_debug(DBG_KERNEL_NET, "I %d, a 0x%x, l %d\n", n , fv->buffersUtilises->elementsUtilises[n].indiceBuffer, fv->buffersUtilises->elementsUtilises[n].longueur);
   }
   // WARNING, c'est une première ébauche pour comprendre comment ça
   // marche. Il faut gérer les mb et la possibilité de plusieurs
   // chaînes. Voir [3] 2.4.2
   if (fv->dernierIndiceUtilise < fv->buffersUtilises->indice) {
      indiceBuffer = fv->buffersUtilises->elementsUtilises[fv->dernierIndiceUtilise].indiceBuffer;
      lgTotale = fv->buffersUtilises->elementsUtilises[fv->dernierIndiceUtilise].longueur;
      printk_debug(DBG_KERNEL_NET, "Indice %d, long : %d\n", indiceBuffer, fv->buffersUtilises->elementsUtilises[fv->dernierIndiceUtilise+1].longueur);
      fv->dernierIndiceUtilise++;
      // On traite cette chaîne
      do {
         bu[result] = (void *) fv->tableDescripteurs[indiceBuffer].adresse;
	 lg[result] = min(fv->tableDescripteurs[indiceBuffer].longueur, lgTotale);
	 printk_debug(DBG_KERNEL_NET, "Sur %d : %d(%d)/%d (@ 0x%x)\n", result, lg[result], fv->tableDescripteurs[indiceBuffer].longueur, lgTotale, bu[result]);
	 lgTotale -= lg[result];
	 indiceBuffer = fv->tableDescripteurs[indiceBuffer].suivant;
	 result++;  // Ca en fait un de plus !

	 // On continue tant qu'il y en a et qu'on a de la place pour
	 // les fournir
      } while ((result < nb)
	       && (fv->tableDescripteurs[indiceBuffer].flags & VRING_DESC_F_NEXT));
   }
   printk_debug(DBG_KERNEL_NET, "OUT\n");
   return result;
}

void virtioFileInterdireInteruption(VirtioFileVirtuelle * fv)
{
  fv->buffersUtilises->flags = 1;
}

void virtioFileAutoriserInteruption(VirtioFileVirtuelle * fv)
{
  fv->buffersUtilises->flags = 0;
}
