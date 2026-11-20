/**
 * @file reseau/reseau.c
 * @brief Implantation des éléments principaux du sous système réseau
 *
 *                                                                  (C) Manu Chaput 2025
 */
#include <manux/interface-reseau.h>
#include <manux/reseau.h>
#include <manux/buffer-reseau.h>
#include <manux/debug.h>
#include <manux/arp.h>

#ifdef MANUX_TACHES
#   include <manux/tache.h>
#endif

#ifndef MANUX_RESEAU_NBMAX_INTERFACES 
#   define  MANUX_RESEAU_NBMAX_INTERFACES 8
#endif

/**
 * @brief Les interfaces réseau du système
 */
static ReseauInterface * interfacesReseau[MANUX_RESEAU_NBMAX_INTERFACES];
static int nbInterfacesReseau = 0;

/**
 * @brief Réception d'un paquet sur le système
 */
void reseauRecevoirPaquet(BufferReseau *b)
{
   switch (b->protocole) {
      case PROTO_ARP :
         arpRecevoir(b);
      break;
      /*
      case PROTO_IPv4 :
         ipv4Recevoir(b);
      break;
      case PROTO_IPv6 :
         ipv6Recevoir(b);
      break;
      */
      default :
         bufferReseauLiberer(b);
      break;
	
   }
}

/**
 * @brief La liste des buffers qui ont été reçus mais non encore
 * traités 
 */
ListeBufferReseau * listeBuffersRecus = NULL;

/**
 * @brief La fonction de traitement dun buffer qui a été reçu

static void reseauAguillerBuffer(BufferReseau *b) {
   ReseauInterface * ir;
  
   assert(b != NULL);

   ir = b->interfaceReception;

   // On invoque la fonction de réception de l'interface qui va
   // déterminer le champ protocole en fonction de son format
   ir->recevoirTrame(ir);

   // On peut maintenant passer le contenu au protocole correspondant
   reseauRecevoirPaquet(b);
}
 */
/**
 * @brief Exécution des parties basses de réception de trames
 */
static void reseauTraiterPartiesBasses(void) {
   int n;
   ReseauInterface * i;

   for (n = 0; n < nbInterfacesReseau; n++) {
      i = interfacesReseau[n];
      if (i->receptionEnAttente) {
         i->recevoirTrame(i);
      }
   }
}

/**
 * @brief La fonction en charge de recevoir et aiguiller tous les
 * buffers reçus sur le système
 * WARNING on traite aussi les parties basses et on boucle bêtement !
 * Il faudra peut-être plutôt faire une attente passive.
 */
static
void reseauTraiterReceptions(void)
{
   BufferReseau * br;

   printk_debug(DBG_KERNEL_NET, "Traitement de la file de reception reseau ...\n");

   while (1) {
      reseauTraiterPartiesBasses();
      if (!listeBufferReseauVide(listeBuffersRecus)) {
	 br = listeBufferReseauExtraire(listeBuffersRecus);
         reseauRecevoirPaquet(br);
      } else {
	ordonnanceur();    // WARNING, on devrait dormir en attendant
      }
   }
}

/**
 * @brief Initialisation du sous-système réseau
 */
void reseauInitialiser(void)
{
   printk_debug(DBG_KERNEL_NET, "Initialisation du sous systeme reseau\n");

   listeBuffersRecus = listeBufferReseauCreer();
   
#ifdef MANUX_TACHES
   // Lancement d'une tâche de traitement des trames reçues
   printk_debug(DBG_KERNEL_NET, "Lancement de la tache de reception\n");
   ordonnanceurCreerTacheNoyau(reseauTraiterReceptions);

   printk_debug(DBG_KERNEL_NET, "Tache de traitement des receptions lancee\n");
#else    
   printk_debug(DBG_KERNEL_NET, "Pas de gestion des taches, ...\n");
#endif
}

/**
 * @brief Ajout d'une nouvelle interface réseau
 */
void reseauAjouterInterface(ReseauInterface * i)
{
   interfacesReseau[nbInterfacesReseau++] = i;
}

/**
 * @brief Affichage de la liste de buffers de réception
 */
void reseauAfficherListeReception()
{
   listeBufferReseauAfficher(listeBuffersRecus);
}
