/*----------------------------------------------------------------------------*/
/*      Définition des fichiers de Manux.                                     */
/*                                                                            */
/* Pour le moment très simplifié : on ne se soucis pas d'ouvrir ou de fermer  */
/* par exemple                                                                */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#ifndef FICHIER_DEF
#define FICHIER_DEF

#include <manux/inoeud.h>
#include <manux/appelsysteme.h> // ParametreAS

typedef struct _Fichier Fichier;

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
   void            * prive;   //!< Caractérisation du fichier ouvert
   INoeud          * iNoeud;  //!< Caractérisation de la structure 
  //   MethodesFichier * methodes;
} Fichier;

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

#ifdef MANUX_KMALLOC
/**
 * @brief : création et ouverture d'un fichier
 */
Fichier * fichierCreer(INoeud * iNoeud);
#endif // MANUX_KMALLOC

#endif
