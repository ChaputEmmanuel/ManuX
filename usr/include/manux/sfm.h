/*----------------------------------------------------------------------------*/
/*      Définition de l'interface du Système de Fichier de Manux.             */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef SFM_DEF
#define SFM_DEF

#include <manux/types.h>

/*
 * La clef "magique" permettant de reconnaitre un SFM
 */
#define SFM_MAGIC 0x18273645

/*
 * Taille d'un bloc
 */
#define SFM_TAILLE_BLOC 512

/*
 * Nombre de blocs en accés direct
 */
#define SFM_NB_BLOCS_DIRECTS 8

/*
 * Longueur (maximale) d'un nom de fichier
 */
#define SFM_LONGUEUR_NOM 32

/*
 * Le superbloc
 */
typedef struct _SFMSuperBloc {
   uint32 magic;
   uint8  version;
   uint32 offsetPremierBlocLibre;
   uint32 offsetInodeRacine;
} SFMSuperBloc;

/*
 * Les différents types de fichier
 */
typedef enum _SFMTypeFichier {
   SFM_FICHIER_ELEMENTAIRE = 0,
   SFM_REPERTOIRE          = 1
} SFMTypeFichier;

/*
 * Structure d'un inode
 */
typedef struct _SFMInode {
   uint32 taille;
   SFMTypeFichier type;   
   uint32 blocsDirects[SFM_NB_BLOCS_DIRECTS];
} SFMInode;

/*
 * Structure d'une entrée dans un répertoire
 */
typedef struct _SFMEntreeRepertoire {
   uint32 offsetInode
   char nomFichier[SFM_LONGUEUR_NOM];
} SFMEntreeRepertoire;

/*
 * Structure d'un répertoire
 */
typedef struct _SFMRepertoire {
   uint32 nombreEntrees;
   SFMEntreeRepertoire entrees[0];
} SFMRepertoire;

#endif

