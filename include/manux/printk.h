/**
 * @file printk.h
 * @brief Définition des fonctions de base d'entrée-sortie du noyau.            
 *                                                                            
 *                                                  (C) Manu Chaput 2000-2023 
 *                                                                            */
#ifndef STD_IO_DEF
#define STD_IO_DEF

#include <manux/config.h>
#include <manux/stdarg.h>

/**
 * @brief Les différentes préfixes pour les niveaux de criticité
 */
#define PRINTK_PANIQUE      "{0}"
#define PRINTK_URGENCE      "{1}"
#define PRINTK_CRITIQUE     "{2}"
#define PRINTK_ERREUR       "{3}"
#define PRINTK_ATTENTION    "{4}"
#define PRINTK_NOTIFICATION "{5}"
#define PRINTK_INFORMATION  "{6}"
#define PRINTK_DEBUGAGE     "{7}"

void printk(char * format, ...);
/**
 * @brief Fonction principale d'affichage dans le noyau
 *
 * Un grand classique. Elle construit une chaîne de caractères puis
 * l'envoie au journal. Si le journal n'est pas actif (MANUX_JOURNAL
 * non défini) alors la chaîne est envoyée sur la console spécifique
 * au noyau.
 *
 * Les formats gérés pour le moment :
 *
 *    %[n][l[l]]{dxo} %s \n
 */

#endif
