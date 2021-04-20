/*----------------------------------------------------------------------------*/
/*      Implantion des fonctions de base d'entrée-sortie.                     */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/printk.h>

#ifdef MANUX_JOURNAL
#   include <manux/journal.h>
#else
#   include <manux/console.h>
#endif
#include <manux/appelsysteme.h>
#include <manux/memoire.h>       /* NULL */

#define chiffre "0123456789abcdef"

void printk(char * format, ...)
{
   va_list   argList;
   char      chaine[4096];   // WARNING, c'est nul
   int       indice = 0;
   int       n;             // valeur associée à un %d
   char      nombre[10];    // chaîne du nombre
   char    * s;             // valeur associée à un %s
   int       in;            // indice pour les boucles internes
   int       nbChiffres;    // pour les %[n]d
   int       base;          // de l'affichage entier

   va_start(argList, format);

   while (*format) {
      switch (*format) {
         case '%' :
            format++;
            /* Lecture de la taille */
            nbChiffres = 0;
            while ((*format <= '9') && (*format >= '0')) {
               nbChiffres = nbChiffres * 10 + *format - '0';
               format++;
	    }

            switch (*format) {
	       case 'o' :
                  base = 8;
                  goto affent;
	       case 'x' :
                  base = 16;
                  goto affent;
	       case 'd' :
                  base = 10;
affent :          n = va_arg(argList, int);
                  if (n < 0) {
		     n = -n;
		     chaine[indice++] = '-';
		  }
                  in = 0;
                  do {
                     nombre[in++] = chiffre[n%base];
                     n = n/base;
		  } while (n != 0);
                  while (nbChiffres > in) {
                     nbChiffres--;
                     chaine[indice++] = ' ';
		  }
                  do {
                     chaine[indice++] = nombre[--in];
                  } while (in);
               break; 
               case 's' :
                  s = va_arg(argList, char *);
                  in = 0;
                  while (s[in]) {
                     chaine[indice++] = s[in++];
		  }
               break;
               default :
               break; 
	    }
         break;
	 default :
            chaine[indice++] = *format;
         break;
      }
      format++;
   }

   chaine[indice] = 0;

   /* On affiche et on déverouille */
#ifdef MANUX_JOURNAL
   journaliser(chaine);
#else
   afficherEcran(message);
#endif
   
   va_end(argList);
}
