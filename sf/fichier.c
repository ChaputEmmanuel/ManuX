/*----------------------------------------------------------------------------*/
/*      Implantation des fichiers de Manux.                                   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/fichier.h>
#include <manux/debug.h>
#include <manux/scheduler.h>    // tacheEnCours

#define MANUX_DEBUG_FS_BASE

int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;
   int result;

   printk("Pouet\n");
#ifdef MANUX_DEBUG_FS_BASE
   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);
#endif

   f = &tacheEnCours->fichiers[fd];  // WARNING !!! Gestion erreur
   
#ifdef MANUX_DEBUG_FS_BASE
   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : fd=%d, file=%x\n", fd, f);
#endif

   result = f->methodes->ecrire(f, buffer, nbOctets);
   
#ifdef MANUX_DEBUG_FS_BASE
   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : res = %d\n", result);
#endif
   
   return result;
}

void sfInitialiser()
{
  definirAppelSysteme(NBAS_ECRIRE, sys_ecrire);
}
