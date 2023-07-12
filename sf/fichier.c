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

   //   printk("C'est parti mon kiki  !\n");
   // On invoque la méthode associée
   result = f->iNoeud->methodesFichier->lire(f, buffer, nbOctets);

   return result;
}

int fichierEcrire(Fichier * f, void * buffer, int nbOctets)
{
   int result;

   //printk("C'est parti les aminches !\n");
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

   f = tacheEnCours->fichiers[fd];  // WARNING !!! Gestion erreur
   
   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : fd=%d, file=%x\n", fd, f);

   result = fichierEcrire(f, buffer, nbOctets);
   
   printk_debug(DBG_KERNEL_SYSFI, "sys_ecrire : res = %d\n", result);
   
   return result;
}

int sys_lire(ParametreAS as, int fd, void * buffer, int nbOctets)
{
   Fichier * f;
   int result;

   printk_debug(DBG_KERNEL_SYSFI, "sys_lire fd = %d, b = %d, nb = %d IN\n", fd, buffer, nbOctets);

   f = tacheEnCours->fichiers[fd];  // WARNING !!! Gestion erreur
   
   printk_debug(DBG_KERNEL_SYSFI, "sys_lire : fd=%d, file=%x\n", fd, f);

   result = fichierLire(f, buffer, nbOctets);
   
   printk_debug(DBG_KERNEL_SYSFI, "sys_lire : res = %d\n", result);
   
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
  
   printk_debug(DBG_KERNEL_SYSFI, "IN");

   // WARNING, plein de précautions à prendre !

   f->iNoeud = iNoeud;

   if (iNoeud->methodesFichier->ouvrir) {
      result = iNoeud->methodesFichier->ouvrir(iNoeud, f);
   }

   return result;
}

#ifdef MANUX_KMALLOC
/**
 * @brief : création et ouverture d'un fichier
 */
Fichier * fichierCreer(INoeud * iNoeud)
{
   Fichier * result = kmalloc(sizeof(Fichier));

   if (ouvrirFichier(iNoeud, result) == ESUCCES) {
      return result;
   } else {
      kfree(result);
      return NULL;
   }
}
#endif // MANUX_KMALLOC
