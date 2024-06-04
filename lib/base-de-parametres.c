/**
 * @file lib/base-de-parametres.c
 * @brief
 *
 *                                                            (C) E. Chaput 2024
 */
#include <manux/base-de-parametres.h>
#include <manux/printk.h>
#include <manux/kmalloc.h>

typedef struct _parametre {
   char * nom;
   typeParametre type;
   union {
      char * chaine;
      struct _baseDeParametres * base;
   } v;
} parametre;

typedef struct _chainonParametre {
   parametre * parametre;
   struct _chainonParametre * suivant;
   struct _chainonParametre * precedent;   
} chainonParametre;

typedef struct _listeParametres {
   chainonParametre * premier;
   chainonParametre * dernier;   
} listeParametres;
  
typedef struct _baseDeParametres {
   char * nom;
   listeParametres parametres;
} baseDeParametres;

static baseDeParametres * BaseDeParametresManuX = NULL;

/**
 * @brief Initialisation de la base, vide
 */
void baseDeParametresInitialiser()
{
   BaseDeParametresManuX = (baseDeParametres *)kmalloc(sizeof(baseDeParametres));
}

void baseDeParametresAjouterTableau(baseDeParametres * base,
                                    typeParametre type,
                                    char * valeur,
				    va_list argList)
{
   char * chaine;

   chaine = va_arg(argList, char *);

   while (chaine != NULL) {
      printk(" Noeud '%s'\n", chaine);
   }
}

/**
 * @brief Ajout d'un paramètre dans la base
 *
 * Exemples 
 *   baseDeParametresAjouter(b, typeParametreU32, "0xFF", "systeme", "debug", "mask", NULL);
 *      b.systeme.debug.mask <- (uint32_t)0xFF
 *   baseDeParametresAjouter(b, typeParametreBase, "", "net", "ip", NULL);
 *      création du sous système ip dans le système net
 */
void baseDeParametresAjouter(baseDeParametres * base,
			     typeParametre type,
			     char * valeur,
			     ...)
{
   va_list   argList;

   va_start(argList, valeur);
   baseDeParametresAjouterTableau(base, type, valeur, argList);
   va_end(argList);
}

/**
 * AJouter un paramètre à ManuX
 */
void manuXAjouterParametre(typeParametre type,
			     char * valeur,
			     ...)
{
   va_list   argList;

   va_start(argList, valeur);
   baseDeParametresAjouterTableau(BaseDeParametresManuX, type, valeur, argList);
   va_end(argList);    
}

/**
 * @brief Lecture d'un paramètre dans la base
 */
char * baseDeParametresLire(char * n, ...)
{
   char * result = NULL;

   return result;
}
 
