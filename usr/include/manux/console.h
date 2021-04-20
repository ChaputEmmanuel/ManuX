/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de base d'accés à la console.                */
/*                                                                            */
/*      Une console est protégée par un verrour de type ExclusionMutuelle.    */
/*   C'est à l'utilisateur de veiller à respecter les appels aux fonctions    */
/*   d'entrée et de sortie de la section critique avant et aprés chaque       */
/*   utilisation de la console. Bien sur le printf s'en occupe.               */
/*      La seule fonction dans laquelle ces appels sont effectués est celle   */
/*   permettant le basculement de console active.                             */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_CONSOLE_DEF
#define MANUX_CONSOLE_DEF

#include <manux/types.h>
#include <manux/atomique.h>     /* Accés unique à la console  */
#include <manux/fichier.h>      /* Une console est un fichier */
/*
 * Structure d'une console. Pour le moment c'est simple
 * mais il faudra étendre plus tard.
 */
typedef struct _Console {
   char              * adresseEcran;
   int                 ligne, colonne ;
   unsigned char       attribut;
   uint8               nbLignes;
   uint8               nbColonnes;
   ExclusionMutuelle   scAcces;
} Console;

extern Console ecranPhysique;

/*
 * Les méthodes permettant de traiter une console comme un fichier
 */
extern MethodesFichier consoleMethodesFichier;

/*
 * Définition des couleurs WARNING, c'est portnawak
 */
typedef enum {
   COUL_NOIR = 0,
   COUL_BLEU_CLAIR,
   COUL_VERT,
   COUL_CYAN,
   COUL_ROUGE,
   COUL_MAGENTA,
   COUL_JAUNE,
   COUL_BLANC,
   COUL_BLEU_FONCE,
   COUL_BLEU,
   COUL_MAUVE = 13
} Couleur;

/*
 * Définition de certains caractères ASCII
 */
#define ASCII_ESC 27

void initialiserConsole(Console * cons, char * adresseEcran);
/*
 * Initialisation de la console. Nécessaire avant toute autre opération.
 * L'adresse de l'écran doit être fournie. On passera NULL pour accéder
 * à l'écran physique.
 */

void affecterCouleurFond(Console * cons, Couleur coul);

void affecterCouleurTexte(Console * cons, Couleur coul);

void afficherConsole(Console * cons, char * msg);
/*
 * Affichage d'un message à l'écran. Attention, aucun formatage
 * n'est fait.
 * WARNING : doit disparaître au profit de la suivante
 */

int consoleEcrire(Fichier * f, void * buffer, int nbOctets);
/*
 * Écriture sur une console
 */

void effacerConsole(Console * cons);
/*
 * Effacement (avec la couleur courante) et positionnement du curseur en
 * haut à gauche.
 */

void afficherConsoleEntier(Console * cons, int n);
/*
 * Affichage d'un entier sur la console
 */

void afficherConsoleRegistre(Console * cons, int nbOctets, int reg);
/*
 * Affichage d'un entier sur la console. En hexa sur le nbre d'octets
 * voulu.
 */

void basculerConsole();
/*
 * Basculer vers la prochaine console virtuelle
 */


#endif
