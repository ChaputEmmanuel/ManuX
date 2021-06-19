/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de gestion des appels système.       */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/appelsysteme.h>

#include <manux/errno.h>
#include <manux/printk.h>
#include <manux/debug.h>
#include <manux/scheduler.h>   // sys_basculerTache
#include <manux/memoire.h>     // AS_obtenirPages, à virer aprés dispatch  
#include <manux/console.h>     // Console
#include <manux/tache.h>       // sysFork

void * vecteurAppelsSysteme[NB_MAX_APPELS_SYSTEME];

/*
 * Implantation de l'appel système inutile
 */
int sys_dumbAS(ParametreAS as)
{
   printk("I am so useless, ...\n");
   return 0;
}

int definirAppelSysteme(int num, void * appel)
{
   if ((num < 0) || (num >= NB_MAX_APPELS_SYSTEME)) {
      return EINVAL;
   } else {
      printk_debug(DBG_KERNEL_AS, "Appel sys %d loaded\n", num);
      vecteurAppelsSysteme[num] = appel;
      return ESUCCES;
   }
}

void initialiserAppelsSysteme()
{
   printk_debug(DBG_KERNEL_AS, "vecteur des AS = 0x%x\n", vecteurAppelsSysteme);

   definirAppelSysteme(NBAS_DUMB, sys_dumbAS);
   
   /* Envoyer une chaîne de caractères sur la console */
   definirAppelSysteme(NBAS_ECRIRE_CONS, sys_ecrireConsole);

#ifdef MANUX_TACHES
   /* Création d'une nouvelle tâche */
   definirAppelSysteme(NBAS_CREER_TACHE, sys_creerTache);   

   /* Invocation explicite de l'ordonnanceur */
   definirAppelSysteme(NBAS_BASCULER_TACHE, sys_basculerTache);
#endif
   
   /* Les 4 suivants sont à vérifier */
   //definirAppelSysteme(NBAS_NUMERO_TACHE,   AS_numeroTache);
   //definirAppelSysteme(NBAS_CONSOLE,        AS_console);
   //definirAppelSysteme(NBAS_OBTENIR_PAGES,  AS_obtenirPages);
   //   definirAppelSysteme(NBAS_FORK,           sysFork);

}

void entrerAppelSysteme()
{
  printk_debug(DBG_KERNEL_AS, "Appel sys IN\n");
}

void sortirAppelSysteme()
{
  printk_debug(DBG_KERNEL_AS, "Appel sys OUT\n");
}

/*
 * Déclaration des appels système. WARNING à dispatcher dans usr ...
 */
/*
appelSysteme0(NBAS_NUMERO_TACHE,  int,       numeroTache);
appelSysteme0(NBAS_CONSOLE,       Console *, getConsole);
appelSysteme1(NBAS_OBTENIR_PAGES, int,       obtenirPages, int);
*/
