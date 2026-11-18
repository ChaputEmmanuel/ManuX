/**
 * @file include/manux/buffer-reseau.h
 * @brief Définition des buffers réseau et les fonctions associcées
 *
 *                                                             (C) Manu Chaput 2025-2026
 */
#ifndef BUFFER_RESEAU
#define BUFFER_RESEAU

//#include <manux/interface-reseau.h>
#include <manux/types.h>

/**
 * @brief Taille réservée en tête d'un buffer réseau
 */
#ifndef MANUX_BUFFER_RESEAU_DEBUT
#   define MANUX_BUFFER_RESEAU_DEBUT 64
#endif

#ifndef MANUX_BUFFER_RESEAU_TAILLE_MAX
#   define MANUX_BUFFER_RESEAU_TAILLE_MAX 2048
#endif

/**
 * @brief Défnition d'un buffer réseau
 *
 * Le champ protocole désigne le protocole véhiculé dans la trame. Ce
 * sera donc par exemple IPv4, IPv6 mais aussi peut-être ARP, ...
 * C'est ce champ qui déterminera ensuite l'acheminement vers la bonne
 * fonction de réception correspondante.
 */
#define PROTO_IPv4     0x0800
#define PROTO_ARP      0x0806
#define PROTO_IPv6     0x86DD

typedef struct _ReseauInterface ReseauInterface;

/**
 * @brief La structure générale permettant de manipuler les messages
 * réseau
 *
 * On a une zone générale avec des données dans laquelle on a les
 * entêtes qui se succèdent. C'est cette même structure qui est
 * manipulée par les différentes couches réseau, de sorte à éviter les
 * copies successives. Du coup, on a des pointeurs pour savoir où
 * chaque protocole commence.
 * On suppose ici qu'il n'y a pas d'enqueue donc pour chaque couche le
 * message se termine a donnees+taille
 */
typedef struct _bufferReseau {
   // Interface sur laquelle on l'a reçu (ou NULL)
   ReseauInterface * interfaceReception;
  
   // L'intégralité des données 
   void            * base;     // Premier octet alloué 
   void            * donnees;  // Premier octet utilisable
   int               taille;   // Taille totale utilisée

   // Couche liaison
   void            * enteteLiaison;    // Pointeur sur le premier octet

   // Couche réseau (ou ARP, ...)
   uint16_t          protocole;       // protocole véhiculé
   void            * enteteProtocole; // Pointeur sur l'entête

} BufferReseau;

/**
 * @brief Un liste de buffers réseau
 *
 */
typedef struct _ListeBufferReseau ListeBufferReseau;


/**
 * @brief Création allocation et mise à zéro d'un buffer réseau.
 *
 * Cette fonction crée un buffer vide dont les champs devront ensuite
 * être initialisé au grès de la traversée des couches protocolaires.
 */
BufferReseau * bufferReseauCreer(void);

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
BufferReseau * bufferReseauInitialiser(void * donnees, int taille);

/**
 * @brief Destruction d'un buffer
 */
void bufferReseauLiberer(BufferReseau * b);

/**
 * @brief Création d'une liste vide de buffers réseau
 */
ListeBufferReseau * listeBufferReseauCreer(void);

/**
 * @brief Combien reste-t-il d'octets utilisable en fin de buffer ?
 */
uint16_t bufferReseauPlaceLibreFin(BufferReseau * b);

/**
 * @brief Étendre la mémoire par le début (pour ajouter un nouvel
 * entête)
 */
void * bufferReseauEtendreDebut(BufferReseau * b, uint16_t taille);

/**
 * @brief Une liste de buffers est-elle vide ?
 */
int listeBufferReseauVide(ListeBufferReseau * l);

/**
 * @brief Insertion d'un buffer réseau à la fin d'une liste
 */
int listeBufferReseauInserer(ListeBufferReseau * l, BufferReseau * b);

/**
 * @brief Extraction d'un buffer réseau d'une liste
 *
 * NULL si liste inexistante ou vide
 */
BufferReseau * listeBufferReseauExtraire(ListeBufferReseau * l);

/**
 * @brief Nombre de buffers dans la liste
 */
int listeBufferReseauTaille(ListeBufferReseau * l);

/**
 * @brief Affichage de la liste de buffers
 */
void listeBufferReseauAfficher(ListeBufferReseau * l);

#endif
