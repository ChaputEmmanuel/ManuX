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
/*                                                (C) Manu Chaput 2000 - 2020 */
/*----------------------------------------------------------------------------*/

#ifndef MANUX_CONSOLE_DEF
#define MANUX_CONSOLE_DEF

#include <manux/config.h>
#include <manux/types.h>
#include <manux/atomique.h>     /* Accés unique à la console  */
#include <manux/fichier.h>      /* Une console est un fichier */

/*
 * Caractéristiques de l'écran physique
 */
#define CON_SCREEN   (char *)0x000B8000
#define CON_COLONNES 80
#define CON_LIGNES   25

/*
 * Structure d'une console. Attention, en cas de consoles virtuelles,
 * l'ordonnanceur stoque ça au début d'une page qui contient également
 * une copie de l'écran. Il faut donc que la somme des deux tailles
 * soit inférieure à la taille d'une page. Ca nous laisse 96 octets
 * pour cette structure.
 */
typedef struct _Console {
   char              * adresseEcran;      // Adresse à laquelle se trouve
                                          // le contenu affiché
   char              * adresseEcranCopie; // Une copie pour lorsque la
                                          // console est active
   int                 ligne, colonne ;
   unsigned char       attribut;
   uint8               nbLignes;
   uint8               nbColonnes;
   ExclusionMutuelle   scAcces;
#ifdef CONSOLES_VIRTUELLES
   struct _Console   * suivante;    // Les consoles virtuelles sont chaînées
   struct _Console   * precedente;  // doublement chaînées
#endif
} Console;

/*
 * Les méthodes permettant de traiter une console comme un fichier
 */
extern MethodesFichier consoleMethodesFichier;

/*
 * Définition des couleurs WARNING, c'est portnawak
 */
typedef enum {
   COUL_TXT_NOIR       = 0x00,
   COUL_TXT_BLEU       = 0x01,
   COUL_TXT_VERT       = 0x02,
   COUL_TXT_CYAN       = 0x03,
   COUL_TXT_ROUGE      = 0x04,
   COUL_TXT_MAGENTA    = 0x05,
   COUL_TXT_MARRON     = 0x06,
   COUL_TXT_GRIS_CLAIR = 0x07,
   COUL_TXT_GRIS       = 0x08,
   COUL_TXT_BLEU_CLAIR       = 0x09,
   COUL_TXT_VERT_CLAIR       = 0x0A,
   COUL_TXT_CYAN_CLAIR       = 0x0B,
   COUL_TXT_ROUGE_CLAIR      = 0x0C,
   COUL_TXT_MAGENTA_CLAIR    = 0x0D,
   COUL_TXT_JAUNE            = 0x0E,
   COUL_TXT_BLANC            = 0x0F,
   COUL_FOND_NOIR       = 0x00,
   COUL_FOND_BLEU       = 0x10,
   COUL_FOND_VERT       = 0x20,
   COUL_FOND_CYAN       = 0x30,
   COUL_FOND_ROUGE      = 0x40,
   COUL_FOND_MAGENTA    = 0x50,
   COUL_FOND_MARRON     = 0x60,
   COUL_FOND_GRIS_CLAIR = 0x70,
   COUL_CLIGNOTANT     = 0x80
} Couleur;

/*
 * Définition de certains caractères ASCII
 */
#define ASCII_ESC 27

/*
 * Initialisation de la console. Le pointeur retourné permet de
 * manipuler ensuite la console.
 */
Console * consoleInit();

/*
 * Choix des couleurs de texte et de fond (voir l'enum ci dessus)
 */
void affecterCouleurFond(Console * cons, Couleur coul);

void affecterCouleurTexte(Console * cons, Couleur coul);

/*
 * Affichage d'un message à l'écran. Attention, aucun formatage
 * n'est fait. En revanche, la chaine de caractères doit être terminée
 * par un zéro.
 */
void afficherConsole(Console * cons, char * msg);

/*
 * Affichage d'un message à l'écran. Attention, aucun formatage
 * n'est fait. Seuls les nbOctets premiers octets sont affichés,
 * indépemment de la présence d'un caractère nul.
 */
void afficherConsoleN(Console * cons, char * msg, int nbOctets);

/*
 * Effacement (avec la couleur courante) et positionnement du curseur en
 * haut à gauche.
 */
void effacerConsole(Console * cons);

/*
 * Affichage d'un entier sur la console
 */
void afficherConsoleEntier(Console * cons, int n);

/*
 * Affichage d'un entier sur la console. En hexa sur le nbre d'octets
 * voulu.
 */
void afficherConsoleRegistre(Console * cons, int nbOctets, int reg);

/*
 * La notion de console virtuelle permet de gérer plusieurs affichages
 * disjoints. Chaque console est donc gérée indépendemment des autres.
 * Une seule est affichée à l'écran à un instant t.
 * Les consoles sont stoquées dans un tableau et repérées par leur
 * indice dans ce tableau.
 */
#ifdef CONSOLES_VIRTUELLES

/*
 * Initialisation d'une console virtuelle. Nécessaire avant toute
 * autre opération. 
 * L'adresse de l'écran doit être fournie.
 */
void initialiserConsole(Console * cons, char * adresseEcran);

/*
 * Forcer l'apparition d'une console à l'écran
 */
void basculerVersConsole(Console * cons);

/*
 * Basculer vers la prochaine console virtuelle
 */
void basculerVersConsoleSuivante();

#endif  // CONSOLES_VIRTUELLES

#ifndef MANUX_JOURNAL
/*
 * Si l'on n'utilise pas le journal, printk() doit savoir sur quelle
 * console afficher.
 */
Console * consoleNoyau();

#endif // MANUX_JOURNAL

/*
 * Écriture sur une console
 */
int consoleEcrire(Fichier * f, void * buffer, int nbOctets);

#endif 
