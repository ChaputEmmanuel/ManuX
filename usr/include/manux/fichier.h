/*----------------------------------------------------------------------------*/
/*      Définition des fichiers de Manux.                                     */
/*                                                                            */
/*                                                       (C) Manu Chaput 2002 */
/*----------------------------------------------------------------------------*/
#ifndef FICHIER_DEF
#define FICHIER_DEF

#include <manux/appelsysteme.h> // ParametreAS

/*
 * Qu'est-ce qu'un fichier du point de vue du noyau ?
 */
typedef struct _Fichier {
   void                    * prive;   // Données privées au fichier
   struct _MethodesFichier * methodes;
} Fichier;

/*
 * Définition des opérations réalisables sur un fichier
 */
typedef struct _MethodesFichier {
   int (*ecrire) (Fichier * f, void * buffer, int nbOctets);
} MethodesFichier;

int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets);
/*
 * L'appel système write
 */

void sfInitialiser();
/* 
 * Initialisation de tout ce qui est lié au SF
 */

#endif
