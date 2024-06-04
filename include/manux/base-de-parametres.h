/**
 * @file include/manux/base-de-parametres.h
 * @brief
 *
 *                                                            (C) E. Chaput 2024
 */

#ifndef BASE_DE_PARAMETRES
#define BASE_DE_PARAMETRES

#include <manux/stdarg.h>

typedef enum {
   typeParametreBase,
   typeParametreChaine,
   typeParametreU32
} typeParametre;

typedef struct _baseDeParametres baseDeParametres;

/**
 * @brief Initialisation de la base, vide
 */
void baseDeParametresInitialiser();

/**
 * @brief Ajout d'un paramètre dans la base
 *
 * Exemples 
 *   baseDeParametresAjouter(b, typeParametreU32, "0xFF", "systeme", "debug", "mask", NULL);
 *      b.systeme.debug.mask <- (uint32_t)0xFF
 *   baseDeParametresAjouter(b, typeParametreBase, "", "net", "ip", NULL);
 *      création du sous système ip dans le système net
 */
void baseDeParametresAjouter(baseDeParametres * base,
			     typeParametre type,
			     char * valeur,
			     ...);

/**
 * AJouter un paramètre à ManuX
 */
void manuXAjouterParametre(typeParametre type,
			   char * valeur,
			   ...);

/**
 * @brief Lecture d'un paramètre dans la base
 */
char * baseDeParametresLire(char * n, ...);


#endif   // BASE_DE_PARAMETRES
