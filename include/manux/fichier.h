/*----------------------------------------------------------------------------*/
/*      Définition des fichiers de Manux.                                     */
/*                                                                            */
/* Pour le moment très simplifié : on ne se soucis pas d'ouvrir ou de fermer  */
/* par exemple                                                                */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#ifndef FICHIER_DEF
#define FICHIER_DEF

#include <manux/appelsysteme.h> // ParametreAS

typedef struct _Fichier Fichier;
typedef struct _INoeud INoeud;

/**
 * @brief : Définition des opérations réalisables sur un fichier
 */
typedef struct _MethodesFichier {
   int (*ouvrir) (INoeud * iNoeud, Fichier * f);
   size_t (*ecrire) (Fichier * f, void * buffer, size_t nbOctets);
   size_t (*lire) (Fichier * f, void * buffer, size_t nbOctets);
} MethodesFichier;

/**
 * @brief : Qu'est-ce qu'un fichier ouvert du point de vue du noyau ?
 */
typedef struct _Fichier {
   void            * prive;   // Données privées au fichier
   MethodesFichier * methodes;
} Fichier;

/**
 * @brief : Définition du type d'un périphérique
 * Pour le moment il sera représenté de façon assez classique par une
 * numérotation à deux champs : un majeur et un mineur.
 */
typedef struct _TypePeripherique {
   uint16_t majeur;
   uint16_t mineur;
} TypePeripherique;

/**
 * @brief : Un INoeud représente un fichier, d'un point de vue statique
 * Il s'agit donc de la description d'un fichier, même spécial, sur le disque.
 */
typedef struct _INoeud {
   TypePeripherique   typePeripherique;
   void             * prive;           // Données spécifiques à la nature
   MethodesFichier  * methodesFichier; // Les fonctions applicables à ce fichier
} INoeud;

int fichierEcrire(Fichier * f, void * buffer, int nbOctets);

#ifdef MANUX_APPELS_SYSTEME
int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets);
/**
 * L'appel système write
 */

int sys_lire(ParametreAS as, int fd, void * buffer, int nbOctets);
/**
 * L'appel système read
 */
#endif

void sfInitialiser();
/* 
 * Initialisation de tout ce qui est lié au SF
 */

/**
 * @brief : Ouverture d'un fichier. 
 * @param iNoeud : le noeud à ouvrir (in)
 * @param f : le fichier ouvert (out)
 *
 * On utilise la fonction d'ouverture du type de périphérique correspondant
 */
int ouvrirFichier(INoeud * iNoeud, Fichier * f);

#endif
