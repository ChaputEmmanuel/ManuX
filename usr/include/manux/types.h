/*----------------------------------------------------------------------------*/
/*      Définition des types de base.                                         */
/*                                                                            */ 
/*                                                       (C) Manu Chaput 2000 */
/*----------------------------------------------------------------------------*/
#ifndef MANUX_TYPES
#define MANUX_TYPES

#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef uint32
typedef unsigned long uint32;
#endif

#ifndef booleen
typedef int booleen;
#endif

#ifndef TRUE
#   define TRUE 1
#endif

#ifndef FALSE
#   define FALSE 0
#endif

/*
 * Définition de la structure contenant tous les registres et sauvée lors d'un
 * pusha ([3] p 121). WARNING, ce n'est pas le meilleur endroit ...
 */
typedef struct _TousRegistres {
   uint32 edi;
   uint32 esi;
   uint32 ebp;
   uint32 esp;
   uint32 ebx;
   uint32 edx;
   uint32 ecx;
   uint32 eax;
} TousRegistres;

/*
 * Type des identificateurs de tâche
 */
typedef uint16 TacheID;

/*
 * Une page mémoire est simplement vue comme une adresse.
 */
typedef void * Page;

#endif
