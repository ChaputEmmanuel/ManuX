/**
 * @file sf/tube.c
 * @brief Une implantation des tubes de communication
 */
#include <manux/tubes.h>
#include <manux/tache.h>    // tacheAjouterFichiers
#include <manux/scheduler.h>// tacheEnCours
#include <manux/fichier.h>
#include <manux/errno.h>    // ESUCCES
#include <manux/memoire.h>  // NULL

MethodesFichier tubeMethodesFichier;

/**
 * @brief Ouverture d'un tube en tant que Fichier 
 * Ca existe ?
  */
int tubeOuvrir(INoeud * iNoeud, Fichier * f)
{
   f->prive = NULL;

   return ESUCCES;
}

size_t tubeEcrire(Fichier * f, void * buffer, size_t nbOctets)
{
   printk("Coucou les poto !\n");
   return -EINVAL;  
}

size_t tubeLire(Fichier * f, void * buffer, size_t nbOctets)
{
   printk("salut les amis !\n");
   return -EINVAL;  
}

/**
 * @brief Déclaration des méthodes permettant de traiter un tube
 * comme un fichier
 */
MethodesFichier tubeMethodesFichier = {
   .ouvrir = tubeOuvrir,
   .ecrire = tubeEcrire,
   .lire = tubeLire
};

#ifdef MANUX_APPELS_SYSTEME
/**
 * @brief Implantation de l'appel système tube()
 *
 * On va créer un iNoeud décrivant le tube (une structure en mémoire)
 * puis deux descripteurs de fichiers, l'un pour écrire, l'autre pour
 * lire. 
 */
int sys_tube(ParametreAS as, int * fds)
{
   INoeud  * iNoeud;
   Fichier * fichiers[2];
   void    * buffer;       // Le buffer qui contient les données

   printk("Coucou les tele tubes !\n");

   // Alocation de la mémoire tampon du tube
   if ((buffer = allouerPage()) == NULL) {
      return ENOMEM;
   }
   
   // Création de l'iNoeud qui décrit le tube dans le système
   iNoeud = iNoeudCreer(buffer, &tubeMethodesFichier);

   // Création du fichier d'entrée du tube (celui où on va écrire)
   fichiers[0] = fichierCreer(iNoeud);

   // Création du fichier de sortie du tube (celui où on va lire)
   fichiers[1] = fichierCreer(iNoeud);

   // On ajoute les fichiers à la tâche
   if (tacheAjouterFichiers(tacheEnCours, 2, fichiers, fds) != 2 ) {
      return ENOMEM;
   }

   // Si on est encore là, c'est que tout s'est déroulé comme prévu !
   return ESUCCES;
}
#endif


/**
 * @brief
 */
void tubesInitialisation()
{
}
