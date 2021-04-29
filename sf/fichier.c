/*----------------------------------------------------------------------------*/
/*      Implantation des fichiers de Manux.                                   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/fichier.h>
#include <manux/printk.h>
#include <manux/scheduler.h>    // tacheEnCours

#define MANUX_DEBUG_FS_BASE

int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;
   int result;
   
   printk("sys_ecrire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);

   f = &tacheEnCours->fichiers[fd];  // WARNING !!! Gestion erreur
   
#ifdef MANUX_DEBUG_FS_BASE
   printk("sys_ecrire : fd=%d, file=%x\n", fd, f);
#endif
   //   while (1){};		

   result = f->methodes->ecrire(f, buffer, nbOctets);
   
#ifdef MANUX_DEBUG_FS_BASE
   printk("sys_ecrire : res = %d\n", result);
#endif
   //   while (1){};
   
   return result;
}

void sfInitialiser()
{
  definirAppelSysteme(NBAS_ECRIRE, sys_ecrire);
}
