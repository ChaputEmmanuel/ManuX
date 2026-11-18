/**
 * @file
 * @brief
 *                                                                  (C) Manu Chaput 2025
 */
#include <manux/printk.h>
#include <manux/buffer-reseau.h>
#include <manux/debug.h>
#include <manux/string.h>    // memcopy
#include <manux/kmalloc.h>

#define MANUX_RESEAU_AUDIT

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

typedef struct _CelluleBufferReseau {
   BufferReseau                * br;
   struct _CelluleBufferReseau * suivant;
} CelluleBufferReseau;
  
typedef struct _ListeBufferReseau {
   CelluleBufferReseau * premier;
   CelluleBufferReseau * dernier;
#ifdef MANUX_RESEAU_AUDIT
   uint32_t nbBuffersCumul;
#endif 
} ListeBufferReseau;

/**
 * @brief Combien reste-t-il d'octets utilisable en fin de buffer ?
 */
uint16_t bufferReseauPlaceLibreFin(BufferReseau * b)
{
  return b->base + MANUX_BUFFER_RESEAU_TAILLE_MAX - (b->donnees + b->taille);
}

/**
 * @brief Création allocation et mise à zéro d'un buffer réseau.
 *
 * Cette fonction crée un buffer vide dont les champs devront ensuite
 * être initialisé au grès de la traversée des couches protocolaires.
 *
 * WARNNG : ne pas utiliser kmalloc !! mettre 2 paquets dans une page
 * avec un allocateur spécifique.
 */
BufferReseau * bufferReseauCreer()
{
   BufferReseau * result = (BufferReseau *)kmalloc(sizeof(BufferReseau));

   if (result) {
      printk_debug(DBG_KERNEL_NET, "Creation d'un buffer reseau de taille %d\n",
		   MANUX_BUFFER_RESEAU_TAILLE_MAX);
      result->base = kmalloc(MANUX_BUFFER_RESEAU_TAILLE_MAX);
      if (result->base == NULL) {
         kfree(result);
         result = NULL;
      }
      result->donnees = result->base + MANUX_BUFFER_RESEAU_DEBUT;
      result->taille = 0;
   }
   
   return result;
}

/**
 * @brief Création et initialisation d'un buffer avec des données
 *
 * Cette fonction utilise bufferReseauCreer() pour créer un buffer
 * vide, puis elle copie les données dans ce buffer. Elle ne peut pas
 * analyser le contenu pour initialiser les champs de la structure,
 * c'est donc à l'appelant de s'en charger (par exemple une fonction
 * de réception d'une trame pourra l'utiliser puis initialiser
 * enteteLiaison, protocole, ...
 */
BufferReseau * bufferReseauInitialiser(void * donnees, int taille)
{
   BufferReseau * result = bufferReseauCreer();
   taille = MIN(taille, bufferReseauPlaceLibreFin(result));
   
   if (result != NULL) {
      memcpy(result->donnees, donnees, taille);
      result->taille = taille;
   }
   
   return result;
}

/**
 * @brief Destruction d'un buffer
 */
void bufferReseauLiberer(BufferReseau * b)
{
   printk_debug(DBG_KERNEL_A_FAIRE, "Fuite memoire a corriger !\n");
   kfree(b);
}

/**
 * @brief Étendre la mémoire par le début (pour ajouter un nouvel
 * entête)
 */
void * bufferReseauEtendreDebut(BufferReseau * b, uint16_t taille)
{
   b->donnees = MAX(b->base, b->donnees - taille);

   return b->donnees;
}

/**
 * @brief Création d'une liste vide de buffers réseau
 */
ListeBufferReseau * listeBufferReseauCreer(void)
{
   ListeBufferReseau * result = (ListeBufferReseau *)kmalloc(sizeof(ListeBufferReseau));

   if (result) {
      printk_debug(DBG_KERNEL_NET, "Creation d'une liste de buffers reseau\n");
      result->premier = NULL;
      result->dernier = NULL;
   };
   
   return result;
}

/**
 * @brief Une liste de buffers est-elle vide ?
 */
int listeBufferReseauVide(ListeBufferReseau * l)
{
   return ((l == NULL) || (l->premier == NULL));
}

/**
 * @brief Insertion d'un buffer réseau à la fin d'une liste
 */
int listeBufferReseauInserer(ListeBufferReseau * l, BufferReseau * b)
{ 
   printk_debug(DBG_KERNEL_NET, "Insertion d'un nouveau buffer\n");

   assert (l != NULL);
   
   CelluleBufferReseau * c = (CelluleBufferReseau *)kmalloc(sizeof(CelluleBufferReseau));

   if (c) {
      c->br = b;
      c->suivant = NULL;
      if (l->dernier != NULL) {
         l->dernier->suivant = c;
      }
      l->dernier = c;
      if (l->premier == NULL) {
         l->premier = c;
      }
#ifdef MANUX_RESEAU_AUDIT
      l->nbBuffersCumul++;
#endif 
      return 1;
   } else {
      return 0;
   }
}

/**
 * @brief Extraction d'un buffer réseau en tête d'une liste
 *
 * NULL si liste inexistante ou vide
 */
BufferReseau * listeBufferReseauExtraire(ListeBufferReseau * l)
{
   CelluleBufferReseau * cellResult = NULL;
   BufferReseau * result = NULL;

   printk_debug(DBG_KERNEL_NET, "Extraction d'un buffer\n");
   
   if ((l != NULL) && (l->premier != NULL)) {
      cellResult = l->premier;
      l->premier = cellResult->suivant;
      if (l->dernier == cellResult) {
         l->dernier = NULL;
      }
      result = cellResult->br;
      kfree(cellResult);
   }
   return result;
}

/**
 * @brief Affichage de la liste de buffers avec printkc
 */
void listeBufferReseauAfficher(ListeBufferReseau * l)
{
   int n = 0;
   CelluleBufferReseau * cell;
   uint8_t * data;
   
   if (l != NULL) {
      n = listeBufferReseauTaille(l);
      printkc("Il y a %d paquet(s) dans la file :\n", n);
#ifdef MANUX_RESEAU_AUDIT
      printkc("Il y en a eu %d en tout\n", l->nbBuffersCumul);
#endif 
      for (cell = l->premier, n=0; (cell != NULL) && (n<20); cell = cell->suivant, n++) {
         data = (uint8_t*)cell->br->donnees;
         printkc("l=%4d : ", cell->br->taille);
         for (int i=0; i< 16; i++) {
	    printkc("%2x ", data[i]);
	 };
	 printkc("\n");
      }
   } else {
      printkc("File reseau pas encore initialisee ...\n");
   }
}

/**
 * @brief Nombre de buffers dans la liste
 */
int listeBufferReseauTaille(ListeBufferReseau * l)
{
   int result = 0;
   CelluleBufferReseau * cell;

   if (l != NULL) {
      for (cell = l->premier; cell != NULL; cell = cell->suivant) {
         result++;
      }
   }
   return result;
}
