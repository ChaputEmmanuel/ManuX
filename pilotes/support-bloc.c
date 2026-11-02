/**
 * @file pilotes/support-bloc.c
 * @brief Implantation des support bloc 
 *                                                          (C) Manu Chaput 2026
 */
#include <manux/errno.h>
#include <manux/support-bloc.h>
#include <manux/fichier.h>
#include <manux/debug.h>           // assert, printk_debug
#include <manux/kmalloc.h>
#include <manux/string.h>          // memcpy

#define MIN(a, b) (((a) < (b)) ? (a) : (b))


/**
 * @brief Ouvrir un fichier de type support bloc dans le système de
 * fichiers 
 */
static int supportBlocOuvrir(INoeud *iNoeud, Fichier *f, uint16_t fanions,
                             uint16_t mode)
{
  [[maybe_unused]] SupportBloc *sb = (SupportBloc *)f->iNoeud->prive;
  
   (void)iNoeud;
   (void)mode;
   (void)fanions;

   return ESUCCES;  
}

/**
 * @brief Fermer un fichier de type support bloc
 */
static
int supportBlocFermer([[maybe_unused]] Fichier *f)
{
   return ESUCCES;  
}

/**
 * @brief Écriture dans un fichier
 */
static
size_t supportBlocEcrire(Fichier *f, void *buffer, size_t nbOctets)
{
   SupportBloc * sb = (SupportBloc *) f->iNoeud->prive;
   void *bufferIntermediaire = NULL;
   uint16_t tailleBloc = sb->tailleBloc;
   uint16_t nbBlocsEntiers;   //!< Combien de blocs pourrons-nous
			      //!copier intégralement ?
   uint32_t indiceBloc;       //!< Indice du prochain bloc à écrire
   uint32_t positionDansBloc; //!< Position dans son bloc du
                              //! premier octet à écrire
   uint32_t nbOctetsAEcrire;  //!< Lors d'une opération sur un bloc
   uint32_t result = 0;
   uint32_t position = f->position; //!< On ne doit pas la modifier

   // On n'écrit pas plus que ce qui est possible
   if (position + nbOctets > tailleBloc * sb->nombreBlocs) {
      nbOctets = tailleBloc * sb->nombreBlocs - position;
   }

   printk_debug(DBG_KERNEL_BLOC, "on va ecrire %d a partir de %d\n", nbOctets, position);
   // Si on ne commence pas sur une frontière de bloc, il faut une
   // copie de copie, ...
   if ((position % tailleBloc) != 0) {
      printk_debug(DBG_KERNEL_BLOC, "On commence dans un bloc\n");
      bufferIntermediaire = kmalloc(tailleBloc);
      if (bufferIntermediaire == NULL) {
         paniqueNoyau();
      }
      // Détermination du premier bloc à lire
      indiceBloc = position / tailleBloc;

      // On doir d'abord lire le contenu du bloc
      sb->methodes->lire(sb, indiceBloc, 1, bufferIntermediaire);

      // On copie dans ce bloc les octets à copier
      positionDansBloc = position % tailleBloc;
      nbOctetsAEcrire = MIN(nbOctets, tailleBloc - positionDansBloc);
      memcpy(bufferIntermediaire + positionDansBloc, buffer, nbOctetsAEcrire);
      
      // On écrit intégralement le bloc
      sb->methodes->ecrire(sb, indiceBloc, 1, bufferIntermediaire);

      // On décompte ce qui a été écrit
      nbOctets -= nbOctetsAEcrire;
      position += nbOctetsAEcrire;
      buffer += nbOctetsAEcrire;
      result += nbOctetsAEcrire;
   }

   // A partir de maintenant, on est sur une frontiêre de bloc
   assert((position % tailleBloc) == 0);

   // Pour tous les blocs complets, on peut faire une copie directe
   nbBlocsEntiers = nbOctets / tailleBloc;

   // Copions donc directement tous les blocs entiers
   if (nbBlocsEntiers > 0) {
      printk_debug(DBG_KERNEL_BLOC, "On va ecrire %d blocs entiers\n", nbBlocsEntiers);
      // Détermination du premier bloc à écrire
      indiceBloc = position / tailleBloc;
      sb->methodes->ecrire(sb, indiceBloc, nbBlocsEntiers, buffer);

      // On décompte ce qui a été écrit (nbBlocsEntiers de taille
      // tailleBloc)
      nbOctets -= nbBlocsEntiers * tailleBloc;
      position += nbBlocsEntiers * tailleBloc;
      buffer += nbBlocsEntiers * tailleBloc;
      result += nbBlocsEntiers * tailleBloc;

   }

   // A partir de maintenant, il nous reste moins d'un bloc à copier
   // et on est sur un début de bloc
   assert(nbOctets < tailleBloc);
   assert((position % tailleBloc) == 0);

   // On se positionne sur le bloc en question
   indiceBloc = position / tailleBloc;
   
   // Pour les derniers octets, on doit faire une copie de copie
   if (nbOctets > 0) {
      printk_debug(DBG_KERNEL_BLOC, "Il nous reste %d octets a ecrire\n", nbOctets);

      if (bufferIntermediaire == NULL) {
         bufferIntermediaire = kmalloc(tailleBloc);
      }
      if (bufferIntermediaire == NULL) {
         paniqueNoyau();
      }

      // On lit le bloc
      sb->methodes->lire(sb, indiceBloc, 1, bufferIntermediaire);

      // On écrit les octets à écrire
      memcpy(bufferIntermediaire, buffer, nbOctets);

      // On écrit le bloc
      sb->methodes->ecrire(sb, indiceBloc, 1, bufferIntermediaire);

      // On décompte ce qui a été écrit (nbBlocsEntiers de taille
      // tailleBloc)
      position += nbOctets;
      result += nbOctets;
   }

   if (bufferIntermediaire != NULL) {
      kfree(bufferIntermediaire);
   };
   
   return result;  
}

/**
 * @brief Lecture dans un fichier
 */
static
size_t supportBlocLire(Fichier *f, void *buffer, size_t nbOctets)
{
   SupportBloc * sb = (SupportBloc *) f->iNoeud->prive;
   void *bufferIntermediaire = NULL;
   uint16_t tailleBloc = sb->tailleBloc;
   uint16_t nbBlocsEntiers;   //!< Combien de blocs pourrons-nous
			      //!copier intégralement ?
   uint32_t indiceBloc;       //!< Indice du prochain bloc à lire
   uint32_t positionDansBloc; //!< Position dans son bloc du
                              //! premier octet à lire
   uint32_t nbOctetsALire;    //!< Lors d'une opération sur un bloc
   uint32_t result = 0;
   uint32_t position = f->position; //!< On ne doit pas la modifier
                                    // 
   // On ne lit pas plus que ce qui est disponible
   if (position + nbOctets > tailleBloc * sb->nombreBlocs) {
      nbOctets = tailleBloc * sb->nombreBlocs - position;
   }

   printk_debug(DBG_KERNEL_BLOC, "on va lire %d a partir de %d\n", nbOctets, position);
   // Si on ne commence pas sur une frontière de bloc, il faut une
   // copie de copie, ...
   if ((position % tailleBloc) != 0) {
      printk_debug(DBG_KERNEL_BLOC, "On commence dans un bloc\n");
      bufferIntermediaire = kmalloc(tailleBloc);
      if (bufferIntermediaire == NULL) {
         paniqueNoyau();
      }
      // Détermination du premier bloc à lire
      indiceBloc = position / tailleBloc;

      // On lit intégralement le bloc
      sb->methodes->lire(sb, indiceBloc, 1, bufferIntermediaire);

      // On copie ce dont on a besoin
      positionDansBloc = position % tailleBloc;
      nbOctetsALire = MIN(nbOctets, tailleBloc - positionDansBloc);
      memcpy(buffer, bufferIntermediaire + positionDansBloc, nbOctetsALire);

      // On décompte ce qu ia été lu
      nbOctets -= nbOctetsALire;
      position += nbOctetsALire;
      buffer += nbOctetsALire;
      result += nbOctetsALire;
   }

   // A partir de maintenant, on est sur une frontiêre de bloc
   assert((position % tailleBloc) == 0);

   // Pour tous les blocs complets, on peut faire une copie directe
   nbBlocsEntiers = nbOctets / tailleBloc;

   // Copions donc directement tous les blocs entiers
   if (nbBlocsEntiers > 0) {
      printk_debug(DBG_KERNEL_BLOC, "On va lire %d blocs entiers\n", nbBlocsEntiers);
      // Détermination du premier bloc à lire
      indiceBloc = position / tailleBloc;
      sb->methodes->lire(sb, indiceBloc, nbBlocsEntiers, buffer);

      // On décompte ce qui a été lu (nbBlocsEntiers de taille
      // tailleBloc)
      nbOctets -= nbBlocsEntiers * tailleBloc;
      position += nbBlocsEntiers * tailleBloc;
      buffer += nbBlocsEntiers * tailleBloc;
      result += nbBlocsEntiers * tailleBloc;

   }

   // A partir de maintenant, il nous reste moins d'un bloc à copier
   // et on est sur un début de bloc
   assert(nbOctets < tailleBloc);
   assert((position % tailleBloc) == 0);

   // On se positionne sur le bloc en question
   indiceBloc = position / tailleBloc;
   
   // Pour les derniers octets, on doit faire une copie de copie
   if (nbOctets > 0) {
      printk_debug(DBG_KERNEL_BLOC, "Il nous reste %d octets a lire\n", nbOctets);

      if (bufferIntermediaire == NULL) {
         bufferIntermediaire = kmalloc(tailleBloc);
      }
      if (bufferIntermediaire == NULL) {
         paniqueNoyau();
      }
      sb->methodes->lire(sb, indiceBloc, 1, bufferIntermediaire);      
      memcpy(buffer, bufferIntermediaire, nbOctets);

      // On décompte ce qui a été lu (nbBlocsEntiers de taille
      // tailleBloc)
      position += nbOctets;
      result += nbOctets;
   }

   if (bufferIntermediaire != NULL) {
      kfree(bufferIntermediaire);
   };
   
   return result;  
}

/**
 * @brief Les méthodes pour manipuler un support bloc comme un fichier
 */
MethodesFichier supportBlocMethodesFichier = {
   .ouvrir = supportBlocOuvrir,
   .fermer = supportBlocFermer,
   .ecrire = supportBlocEcrire,
   .lire = supportBlocLire
};

/**
 * @brief Lecture de blocs depuis un support bloc
 */
uint32_t supportBlocLireBlocs(SupportBloc *supportBloc, uint32_t indice,
                         uint32_t nombre, void *buffer)
{
   return supportBloc->methodes->lire(supportBloc, indice, nombre, buffer);  
}


/**
 * @brief Écriture de blocs vers un support bloc
 */
uint32_t supportBlocEcrireBlocs(SupportBloc *supportBloc, uint32_t indice,
                           uint32_t nombre, void *buffer)
{
   return supportBloc->methodes->ecrire(supportBloc, indice, nombre, buffer);  
}

/**
 * @brief Enregistrement dans le système d'un support bloc
 */
void supportBlocEnregistrer(SupportBloc *sb)
{
   sb->tailleBloc = 512; 
}

/**
 * @brief Une fonction d'ouverture qui ne passe pas par le système de
 * fichiers
 * WARNING : à supprimer dès que possible
 */
Fichier * supportBlocOuvrirHorsSF(SupportBloc *sb) {
   Fichier * resultat;
   INoeud *noeud;
   
   noeud = iNoeudCreer(sb, &supportBlocMethodesFichier);
   resultat = fichierCreer(noeud, O_RDONLY|O_WRONLY, 0);
   
   return resultat;
}
