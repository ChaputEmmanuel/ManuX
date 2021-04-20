/*----------------------------------------------------------------------------*/
/*      Définition des éléments de configuration de ManuX.                    */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

/*
 * Taille d'une page mémoire (4 Ko)
 */
#define TAILLE_PAGE           0x1000

/*
 * Nombres de pages "système" c'est à dire communes à toutes les tâches.
 * WARNING, il serait bon de le calculer en fonction de la taille de la
 * mémoire physique. 
 */
#define NOMBRE_PAGES_SYSTEME 0x800   /* 8 Mo */

/*
 * Adresse de début de la zone gérée par malloc
 */
#ifndef ADRESSE_DEBUT_TAS
#   define ADRESSE_DEBUT_TAS 0x1000000
#endif

/*
 * Le nombre maximal de fichiers manipulés par un processus
 * WARNING : sans aucun intéret pour le moment !
 */
#ifndef NB_MAX_FICHIERS
#   define NB_MAX_FICHIERS  4
#endif

#endif
