/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de gestion des appels système.       */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/appelsysteme.h>

#include <manux/errno.h>
#include <manux/printk.h>
#include <manux/scheduler.h>   // sys_basculerTache
#include <manux/memoire.h>     // AS_obtenirPages, à virer aprés dispatch  
#include <manux/console.h>     // Console
#include <manux/tache.h>       // sysFork

void * vecteurAppelsSysteme[NB_MAX_APPELS_SYSTEME];

int definirAppelSysteme(int num, void * appel)
{
   if ((num < 0) || (num >= NB_MAX_APPELS_SYSTEME)) {
      return EINVAL;
   } else {
      //printk("Appel sys %d loaded\n", num);
      vecteurAppelsSysteme[num] = appel;
      return ESUCCES;
   }
}

void initialiserAppelsSysteme()
{
   definirAppelSysteme(NBAS_NUMERO_TACHE,   AS_numeroTache);
   definirAppelSysteme(NBAS_CONSOLE,        AS_console);
   definirAppelSysteme(NBAS_ECRIRE_CONS,    sys_ecrireConsole);
   definirAppelSysteme(NBAS_OBTENIR_PAGES,  AS_obtenirPages);
   definirAppelSysteme(NBAS_FORK,           sysFork);

   /* Invocation explicite de l'ordonnanceur */
   definirAppelSysteme(NBAS_BASCULER_TACHE, sys_basculerTache);   
}

void entrerAppelSysteme()
{
  printk("Appel sys IN\n");
}

void sortirAppelSysteme()
{
  printk("Appel sys OUT\n");
}

/*
 * Déclaration des appels système. WARNING à dispatcher dans usr ...
 */
/*
appelSysteme0(NBAS_NUMERO_TACHE,  int,       numeroTache);
appelSysteme0(NBAS_CONSOLE,       Console *, getConsole);
appelSysteme1(NBAS_OBTENIR_PAGES, int,       obtenirPages, int);
*/
