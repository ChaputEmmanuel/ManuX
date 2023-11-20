/*----------------------------------------------------------------------------*/
/*      Implantion des fonctions de base d'entrée-sortie du mode utilisateur. */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>     // Suivant la config on passe par fs ou pas
#include <manux/appelsysteme.h>
#include <stdio.h>

/**
 * WARNING : On utilise ici cette fonction définie dans le noyau, il
 * faudra tôt ou tard changer ça, ...
 */
int vsnprintk(char * str, const size_t l, char * format, va_list argList);

#define MAX_PRINTF_LENGTH 64

#define NULL ((void *)0)
#define chiffre "0123456789abcdef"

appelSysteme3(NBAS_ECRIRE, int, ecrire, int, void *, int);

appelSysteme3(NBAS_LIRE, int, lire, int, void *, int);

appelSysteme1(NBAS_FERMER, int, fermer, int);

/*
 * ecrireConsole est un appel système. Son "implantation" côté
 * utilisateur passe donc par une macro.
 */
appelSysteme2(NBAS_ECRIRE_CONS, int, ecrireConsole, char *, int);

/**
 * @brief Un premier printf
 * WARNING on utlise une fonction qui n'est pas définie dans usr ! il
 * faudra changer ça un jour, ...
 */
void printf(char * format, ...)
{
   va_list argList;
   char      chaine[MAX_PRINTF_LENGTH];   // WARNING, il faut une gestion dynamique
                            // attention aux risques de telescopage avec la pile !
   int       result;

   va_start(argList, format);
   result = vsnprintk(chaine, MAX_PRINTF_LENGTH, format, argList);
   va_end(argList);

#ifdef MANU_FS
   ecrire(1, chaine, result); // WARNING : 1 à remplacer par stdout par exemple
#else
   // C'est exactement le but de l'AS ecrireConsole
   ecrireConsole(chaine, result); 
#endif

   va_end(argList);
}

