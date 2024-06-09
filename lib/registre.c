/**
 * @file lib/registre.c
 * @brief
 *
 *                                                            (C) E. Chaput 2024
 */
#include <manux/registre.h>
#include <manux/string.h>               // strlen, memcpy
#include <manux/printk.h>
#include <manux/kmalloc.h>
#include <manux/errno.h>                // ENOENT

/**
 * @brief Un paramètre est simplement une valeur (qui peut être typée)
 * associée à un nom
 */
typedef struct _parametre {
   char * nom;
   typeParametre type;
   union {
      char * chaine;
      struct _registre * registre;
      void * pointeur;
      uint32_t  u32;
   } v;
} parametre;

/**
 * @brief On va construire des listes de paramètres
 */
typedef struct _chainonParametre {
   parametre * parametre;
   struct _chainonParametre * suivant;
   struct _chainonParametre * precedent;   
} chainonParametre;

typedef struct _listeParametres {
   chainonParametre * premier;
   chainonParametre * dernier;   
} listeParametres;

/**
 * @brief Un registre est simplement une liste (nommée) de
 * paramètres. Les paramètres peuvent être eux-mêmes des registres
 */
typedef struct _registre {
   char * nom;
   listeParametres * parametres;
} registre;

static registre * registreSysteme = NULL;

/*----------------------------------------------------------------------------*/
/* Les paramètres                                                             */
/*----------------------------------------------------------------------------*/

/**
 * @brief Création d'un paramètre
 */
parametre * parametreCreer(char * nom, typeParametre type, void * valeur)
{
   parametre * result = (parametre *) kmalloc(sizeof(parametre));

   printk("Creation du param %s de type %d\n", nom, type);
   if (result != NULL) {
      result->nom = kmalloc(strlen(nom) +1);
      memcpy(result->nom, nom, strlen(nom) +1);
      result->type = type;
      result->v.pointeur = valeur;
   }
   
   return result;
}

/*----------------------------------------------------------------------------*/
/* Les listes de paramètres.                                                  */
/*----------------------------------------------------------------------------*/

/**
 * @brief Création d'une liste (vide) de paramètres
 */
listeParametres * listeParametresCreer()
{
   listeParametres * result = (listeParametres *)kmalloc(sizeof(listeParametres));

   if (result != NULL) {
      result->premier = NULL;
      result->dernier = NULL;
   }
   return result;
}

/**
 * @brief Insertion du paramètre p dans la liste lp
 *
 * Attention aucune vérification n'est faite, un doublon peut apparaître
 */
void listeParametresInserer(listeParametres * lp, parametre * p)
{
   chainonParametre * cp = (chainonParametre *) kmalloc(sizeof(chainonParametre));

   printk("Insertion dans 0x%x ...\n", lp);
   if (cp != NULL) {
      cp->parametre = p;
      cp->suivant = lp->premier;
      cp->precedent = NULL;
      if (lp->premier != NULL){
         lp->premier->precedent = cp;
      }
      lp->premier = cp;
      printk("  0x%x premier = 0x%x\n", lp, cp);
      if (lp->dernier == NULL) {
         lp->dernier = cp;
      }
   }
}

/*----------------------------------------------------------------------------*/
/* Les registres.                                                             */
/*----------------------------------------------------------------------------*/

/**
 * @brief Création d'un registre vide
 */
registre * registreCreer(char * nom)
{
   registre * result = (registre *)kmalloc(sizeof(registre));

   if (result != NULL) {
      result->nom = kmalloc(strlen(nom) +1);
      memcpy(result->nom, nom, strlen(nom) +1);
      result->parametres = listeParametresCreer();
   }
   
   return result;					 
}

/**
 * @brief Recherche d'un sous registe (direct) dans un registre. NULL si inexistant.
 */
registre * registreChercherSousRegistre(registre * b, char * nomSr)
{
   registre * result = NULL;
   chainonParametre * ch;

   if ((b != NULL) && (b->parametres != NULL)) {
      for (ch = b->parametres->premier ; (ch != NULL) && (result == NULL) ; ch = ch->suivant) {
         // Si on trouve une sous-base avec le bon nom, on la renvoie
	 if (   (ch->parametre->type == typeParametreRegistre)
	     && (strcmp(nomSr, ch->parametre->nom) == 0)){
            return ch->parametre->v.registre;
	 }
      }
   }
   
   return result;
}

/**
 * @brief Ajout d'un parametre dans un registre
 * 
 * Attention, aucune vérification n'est faite, si bien qu'on doublon
 * peut apparaître
 */
void registreAjouterParametre(registre * reg, parametre * param)
{
   listeParametresInserer(reg->parametres, param);
}

/**
 * @brief Création et ajout d'un sous registre fils dans un registre
 *
 * Un pointeur sur le registre ainsi créé est retourné. Si le sous
 * registre existait déjà, il n'est pas créé, mais le pointeur est
 * correctement retourné.
 */
registre * registreAjouterSousRegistre(registre * reg, char * nomSr)
{
   registre * result;
   parametre * sRParam;
   
   // Existe-t-il déjà ?
   result = registreChercherSousRegistre(reg, nomSr);

   // Si non, on le crée
   if (result == NULL) {
      // Création du (sous) reg
      result = registreCreer(nomSr);

      // On en fait un paramètre
      sRParam = parametreCreer(nomSr, typeParametreRegistre, (void *) result);

      // On l'insère dans le registre père
      registreAjouterParametre(reg, sRParam);	 
   }
   
   return result;
}

/**
 * @brief Ajout dans un reistre d'un paramètre dont le nom est défini
 * dans un tableau.
 */ 
void registreAjouterT(registre * reg,
                      typeParametre type,
                      char * valeur,
                      va_list argList)
{
   char * chaine, * prochaine;
   registre * regSS = reg; // La base du sous-système
   
   chaine = va_arg(argList, char *);
   prochaine = va_arg(argList, char *);

   while (chaine != NULL) {
      // Si on n'est pas à la dernière, on descend dans l'arborescence
      if (prochaine != NULL) {
         regSS = registreChercherSousRegistre(reg, chaine);
	 if (regSS == NULL) {
            regSS = registreAjouterSousRegistre(reg, chaine);
	 }
	 if (regSS != NULL) {
	   reg = regSS; // On travaille maintenant dans le sous reg
	 }
      // Si on est à la dernière, on peut placer la donnée
      } else {
	 registreAjouterParametre(regSS, parametreCreer(chaine, type, valeur));
      }
      // On avance
      chaine = prochaine;
      prochaine = va_arg(argList, char *);
   }
}

/**
 * @brief Ajout d'un paramètre dans un registre
 *
 * Exemples 
 *   registreAjouter(b, typeParametreU32, "0xFF", "systeme", "debug", "mask", NULL);
 *      b.systeme.debug.mask <- (uint32_t)0xFF
 *   registreAjouter(b, typeParametreReg, "", "net", "ip", NULL);
 *      création du sous système ip dans le système net
 * 
 * Toute l'arborescence menant au paramètre est créée.
 */
void registreAjouter(registre * reg,
                     typeParametre type,
                     char * valeur,
                     ...)
{
   va_list   argList;

   va_start(argList, valeur);
   registreAjouterT(reg, type, valeur, argList);
   va_end(argList);
}

/**
 * @brief Lecture dans un registre d'un paramètre dont le nom est donné
 * sous forme de tableau.
 */
int registreLireT(registre * reg, uint32_t * valeur, va_list argList)
{
   int result = -ENOENT;

   char * chaine, * prochaine;
   chainonParametre * ch;

   registre * regSS = reg; // Le registre du sous-système

   chaine = va_arg(argList, char *);
   prochaine = va_arg(argList, char *);

   while (chaine != NULL) {
      // Si on n'est pas à la dernière, on descend dans l'arborescence
      if (prochaine != NULL) {
         regSS = registreChercherSousRegistre(reg, chaine);
	 if (regSS == NULL) {
            return -ENOENT;
	 }
         reg = regSS;
      // Si on est à la dernière, on peut lire la donnée
      } else {
         for (ch = regSS->parametres->premier; ch != NULL; ch = ch->suivant){
            if (strcmp(ch->parametre->nom, chaine) == 0) {
               *valeur = ch->parametre->v.u32;
	       return 0;
            }
         }
      }
      // On avance
      chaine = prochaine;
      prochaine = va_arg(argList, char *);
   }
   
   return result;
}

/**
 * @brief Affichage (via printk) d'un registre
 */
void registreAfficher(registre * reg, int profondeur)
{
   chainonParametre * ch;
   int p;

   for (p = 0 ; p < profondeur; p++) {printk("   ");}
   printk("[%s]\n", reg->nom);

   for (ch = reg->parametres->premier; ch != NULL; ch = ch->suivant){
      for (p = 0 ; p < profondeur; p++) {printk("   ");}
      printk("%s : ", ch->parametre->nom);
      switch (ch->parametre->type){
         case typeParametreRegistre :
	   printk("(reg)\n");
	   registreAfficher(ch->parametre->v.registre, profondeur +1);
         break ;
         case typeParametreU32 :  
	   printk("0x%x\n", ch->parametre->v.u32);
         break;
         default :  
	   printk("%p\n", ch->parametre->v.pointeur);
         break;
      }
   }
   
}

/*----------------------------------------------------------------------------*/
/* Le registre système.                                                       */
/*----------------------------------------------------------------------------*/

/**
 * @brief Création du registre du système
 */
void registreSystemeInitialiser()
{
   registreSysteme = registreCreer("ManuX-Param");     
}

/**
 * @brief Ajouter au registre système un paramètre dont le nom est
 * donné comme une liste de paramètres.
 * 
 * Exemple
 *    registreSystemeAjouterParametre(typeParametreU32, 0xC0ffeeab,
 *    "net", "ip", "addr", NULL);
 */
void registreSystemeAjouterParametre(typeParametre type,
                                    char * valeur,
                                    ...)
{
   va_list   argList;

   va_start(argList, valeur);
   registreAjouterT(registreSysteme, type, valeur, argList);
   va_end(argList);    
}

/**
 * @brief Lecture d'un paramètre dans le registre du système
 */
int registreSystemeLire(uint32_t * valeur, ...)
{
   va_list   argList;
   int result;
   
   va_start(argList, valeur);
   result = registreLireT(registreSysteme, valeur, argList);
   va_end(argList);
   
   return result;
}

/**
 * @brief Affichage du registre système
 */
void registreSystemeAfficher()
{
    registreAfficher(registreSysteme, 0);   
}
