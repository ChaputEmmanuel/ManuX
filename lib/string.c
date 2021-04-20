/*----------------------------------------------------------------------------*/
/*      Implantation des fonctions de manipulation des chaines.               */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/string.h>

void bcopy (const void *src, void *dest, int n)
/*
 * WARNING à réécrire en assembleur
 */
{
   char * d = dest;
   const char * s = src;
   while (n--)
      *d++ = *s++; 

   /*  
   int i;

   for (i = 0; i < n; i++) {
     ((char *)dest)[i] = ((char *)src)[i];
   }
  */
}
