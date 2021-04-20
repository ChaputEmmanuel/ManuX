/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de base d'entrée-sortie du mode utilisateur. */
/*                                                                            */
/*                                                       (C) Manu Chaput 2002 */
/*----------------------------------------------------------------------------*/
#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

void printf(char * format, ...);
/*
 * Un grand classique ! Gérés pour le moment :
 *
 *    %[n]{dxo} %s \n
 */

#endif
