/*----------------------------------------------------------------------------*/
/*      Implantation des fichiers de Manux.                                   */
/*                                                                            */
/*                                                 (C) Manu Chaput 2002, 2003 */
/*----------------------------------------------------------------------------*/
#include <manux/fichier.h>
#include <manux/printk.h>
#include <manux/scheduler.h>    // tacheEnCours

int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;

   printk("sys_ecrire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);
   while (1){afficherJournal();};		

   f = &tacheEnCours->fichiers[fd];  // WARNING !!! Gestion erreur
   
#ifdef MANUX_DEBUG_FS_BASE
   printk("sys_ecrire : %d, %d\n", fd, f);
#endif

   return f->methodes->ecrire(f, buffer, nbOctets);
}

void sfInitialiser()
{
  definirAppelSysteme(NBAS_ECRIRE, sys_ecrire);
}
