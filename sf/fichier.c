/*----------------------------------------------------------------------------*/
/*      Implantation des fichiers de Manux.                                   */
/*                                                                            */
/*                                                  (C) Manu Chaput 2002-2023 */
/*----------------------------------------------------------------------------*/
#include <manux/fichier.h>
#include <manux/debug.h>
#include <manux/scheduler.h>    // tacheEnCours
#include <manux/errno.h>
#ifdef MANUX_KMALLOC
#   include <manux/kmalloc.h>
#endif

#define MANUX_DEBUG_FS_BASE

int fichierLire(Fichier * f, void * buffer, int nbOctets)
{
   int result;

   printk_debug(DBG_KERNEL_SYSFI, "on invoque 0x%x\n", f->iNoeud->methodesFichier->lire);
   
   // On invoque la méthode associée
   result = f->iNoeud->methodesFichier->lire(f, buffer, nbOctets);

   printk_debug(DBG_KERNEL_SYSFI, "on renvoie %d\n", result);

   return result;
}

int fichierEcrire(Fichier * f, void * buffer, int nbOctets)
{
   int result;

   // On invoque la méthode associée
   result = f->iNoeud->methodesFichier->ecrire(f, buffer, nbOctets);

   return result;
}

#ifdef MANUX_APPELS_SYSTEME
int sys_ecrire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;
   int result;

   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);

   if (tacheEnCours == NULL) {
      printk_debug(DBG_KERNEL_SYSFI, "pas de tache en cours !\n");
      result = -EINVAL;
   } else if (tacheEnCours->fichiers[fd] == NULL) {
      printk_debug(DBG_KERNEL_SYSFI, "pas de fichier %d !\n", fd);
      result = -EPASDEF;
   } else {
      f = tacheEnCours->fichiers[fd]; 
      printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : fd=%d, file=%x\n", fd, f);
      result = fichierEcrire(f, buffer, nbOctets);
      printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : res = %d\n", result);
   }
   
   return result;
}

int sys_lire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;
   int result;

   printk_debug(DBG_KERNEL_SYSFI, "sys_lire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);

   if (tacheEnCours == NULL) {
      printk_debug(DBG_KERNEL_SYSFI, "pas de tache en cours !\n");
      result = -EINVAL;
   } else if (tacheEnCours->fichiers[fd] == NULL) {
      printk_debug(DBG_KERNEL_SYSFI, "pas de fichier %d !\n", fd);
      result = -EPASDEF;
   } else {
      f = tacheEnCours->fichiers[fd]; 
      printk_debug(DBG_KERNEL_SYSFI, "sys_lire : fd=%d, file=%x\n", fd, f);
      result = fichierLire(f, buffer, nbOctets);
      printk_debug(DBG_KERNEL_SYSFI, "sys_lire : res = %d\n", result);
   }
   
   return result;
}
#endif //MANUX_APPELS_SYSTEME

void sfInitialiser()
{
#ifdef MANUX_APPELS_SYSTEME
   definirAppelSysteme(NBAS_ECRIRE, sys_ecrire);
   definirAppelSysteme(NBAS_LIRE, sys_lire);
#endif
}

/**
 * @brief : Ouverture d'un fichier. 
 * @param iNoeud : le noeud à ouvrir (in)
 * @param f : le fichier ouvert (out)
 *
 * On utilise la fonction d'ouverture du type de périphérique correspondant
 */
int ouvrirFichier(INoeud * iNoeud, Fichier * f)
{
   int result = ESUCCES;
  
   //   printk_debug(DBG_KERNEL_SYSFI, "IN");

   // WARNING, plein de précautions à prendre !

   f->iNoeud = iNoeud;

   if (iNoeud->methodesFichier->ouvrir) {
      result = iNoeud->methodesFichier->ouvrir(iNoeud, f);
   }

   //printk_debug(DBG_KERNEL_SYSFI, "OUT");
   return result;
}

#ifdef MANUX_KMALLOC
/**
 * @brief : création et ouverture d'un fichier
 */
Fichier * fichierCreer(INoeud * iNoeud)
{
   Fichier * result = kmalloc(sizeof(Fichier));

   //   printk_debug(DBG_KERNEL_SYSFI, "IN");
   if (ouvrirFichier(iNoeud, result) == ESUCCES) {
      return result;
   } else {
      kfree(result);
      return NULL;
   }
}
#endif // MANUX_KMALLOC
