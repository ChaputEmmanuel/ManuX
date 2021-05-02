/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de base d'entrée-sortie du noyau.            */
/*                                                                            */
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef STD_IO_DEF
#define STD_IO_DEF

#include <manux/config.h>
#include <manux/stdarg.h>

void printk(char * format, ...);
/*
 * Un grand classique ! Gérés pour le moment :
 *
 *    %[n]{dxo} %s \n
 */

#endif
