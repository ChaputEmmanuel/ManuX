/**
 * @file include/manux/registre.h
 * @brief
 *
 *                                                            (C) E. Chaput 2024
 */

#ifndef REGISTRE
#define REGISTRE

#include <manux/stdarg.h>
#include <manux/types.h>   // uint32_t

/**
 * @brief Les différents types de donnée qu'on peut mettre dans un
 * registre 
 */
typedef enum {
   typeParametreRegistre,
   typeParametreChaine,
   typeParametreU32
} typeParametre;

typedef struct _registre registre;

/**
 * @brief Création d'un registre vide
 */
registre * registreCreer(char * nom);

/**
 * @brief Ajout d'un paramètre dans la base
 *
 * Exemples 
 *   registreAjouter(b, typeParametreU32, "0xFF", "systeme", "debug", "mask", NULL);
 *      b.systeme.debug.mask <- (uint32_t)0xFF
 *   registreAjouter(b, typeParametreBase, "", "net", "ip", NULL);
 *      création du sous système ip dans le système net
 */
void registreAjouter(registre * base,
                     typeParametre type,
                     char * valeur,
                     ...);


/**
 * @brief Lecture d'un paramètre dans la base
 */
char * registreLire(char * n, ...);

/**
 * @brief Initialisation du registre système
 */
void registreSystemeInitialiser();

/**
 * @brief Ajouter un paramètre au registre de ManuX
 */
void registreSystemeAjouterParametre(typeParametre type,
                                     char * valeur,
                                     ...);

/**
 * @brief Affichage du registre de ManuX
 */
void registreSystemeAfficher();

/**
 * @brief Lecture d'un paramètre de ManuX
 */
int registreSystemeLire(uint32_t * valeur, ...);

#endif   // REGISTRE
