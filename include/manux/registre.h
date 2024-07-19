/**
 * @file include/manux/registre.h
 * @brief Gestion des registres (outils permettant de stocker des paramètres)
 *
 *   Un paramètre sera défini par un nom, un type et une valeur. Il peut lui
 * être associée une fonction qui sera invoquée lorsque la valeur présente dans
 * le registre est modifiée.
 *
 *   Un registre est une structure arborescente contenant des paramètres.
 *
 *   Il existe plusieurs familles de fonction de création/insertion car il y a
 * des besoins différents :
 * 
 *   . Le principe de base est que chaque sous-sytème crée les paramètres dans
 * un regsitre, avec pour chacun le type et la fonction de mise-à-jour
 * associées. On utilisera pour ça la fonction registreCreerParametre()
 * Le paramètre pourra alors être mis à jour à tout moment en utilisant la
 * fonction parametreMiseAJour() en lui fournissant le paramètre qui aura pu
 * être obtenu via la fonction registreObtenirParametre(). La fonction de mise
 * à jour sera alors invoquée, si elle existe, ce qui permet au sous-système
 * d'être prévenu et de prendre en compte la mise-à-jour.
 *
 *   . Il peut être également utile de renseigner un registre avec des valeurs
 * de paramètres avant que ceux-ci aient été créés et insérés dans le registre.
 * Des paramètres peuvent par exemple être chargés au démarage du système dont
 * la valeur pourra être utilisée lors du démarrage des sous-systèmes 
 * correspondants. Lors de leur première insertion dans le registre, la fonction
 * de mise-à-jour ne peut donc pas être invoqué et le type peut ne pas être
 * connu. On utilisera pour ça la fonction registreAjouterParametre() qui
 * ajoute un paramètre dans un registre et lui affecte une valeur, mais sans
 * invoquer la fonction de mise-à-jour puisqu'elle n'est pas encore définie.
 *
 *   . La fonction registreCreerParametre(), si elle trouve une valeur présente
 * lors de la création, pourra donc utiliser cette valeur pour l'initialisation
 * du sous-système correspondant.
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
 *
 * WARNING : renommer registreCreerParametre
 */
void registreAjouter(registre * base,
                     typeParametre type,
                     char * valeur,
                     ...);

/**
 * @brief Mise à jour de la valeur d'un paramètre dans un registre
 * Le paramètre doit exister. 
 * 
 * retour
 *  0 en cas de succes
 *  -ENOENT paramètre non présent
 *  -EINVAL échec de la conversion dans le type du paramètre
 */
int registreMettreAJourParametre(registre * base,
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
 * @brief Ajouter dans un registre système un paramètre décrit dans une chaîne
 * de caractères.
 *
 * La chaîne doit être sous la forme "a.b.c=v"
 * Attention, elle va être modifiée !
 */
void registreSystemeAjouterC(char * chaine);

/**
 * @brief Affichage du registre de ManuX
 */
void registreSystemeAfficher();

/**
 * @brief Lecture d'un paramètre de ManuX
 */
int registreSystemeLire(uint32_t * valeur, ...);

#endif   // REGISTRE
