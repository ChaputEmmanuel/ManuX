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
#include <manux/kmalloc.h>  // NULL
#include <manux/string.h>   // memcpy

MethodesFichier tubeMethodesFichier;

/**
 * @brief Capacité mémoire d'un tube, en nombde pages
 */
#define MANUX_TUBE_NB_PAGES 1

#define MANUX_TUBE_CAPACITE (MANUX_TAILLE_PAGE * MANUX_TUBE_NB_PAGES)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct _tube {
   uint8_t * donnees;
   int taille; //< Nombre d'octets présents dans le tube
   int indiceProchain ; //< Position de la prochaine insertion
} Tube;

/**
 * @brief Ouverture d'un tube en tant que Fichier 
 * Ca existe ?
  */
int tubeOuvrir(INoeud * iNoeud, Fichier * f)
{
   f->prive = NULL;

   return ESUCCES;
}

/**
 * @brief Écriture dans un fichier
 */
size_t tubeEcrire(Fichier * f, void * buffer, size_t nbOctets)
{
   Tube * tube;
   int n = 0;
   int nbOctetsEcrits = 0; // Le nombre d'octets écrits
   

   if ((f == NULL) || (f->iNoeud == NULL) || (f->iNoeud->prive == NULL)) {
      printk("Dans le cul lulu !\n");
      return -EINVAL;
   }
   tube = f->iNoeud->prive;

   do {
      // On n'écrit ni plus que ce qui est demandé, ni plus que ce
      // qu'on peut
      n = MIN(nbOctets - nbOctetsEcrits, MANUX_TUBE_CAPACITE - tube->taille);

      // On ne doit pas aller écrire au delà du buffer
      n = MIN(n, (MANUX_TUBE_CAPACITE - tube->indiceProchain));

      // On peut donc copier n octets dans le buffer à partir de la
      // position courante, sans risque de déborder
      memcpy(tube->donnees + tube->indiceProchain, buffer, n);

      tube->indiceProchain = (tube->indiceProchain + n) % MANUX_TUBE_CAPACITE;
      tube->taille += n;
      
      buffer += n;

      nbOctetsEcrits += n;
   } while (n > 0);
   
   return nbOctetsEcrits;  
}

size_t tubeLire(Fichier * f, void * buffer, size_t nbOctets)
{
   Tube * tube;
   int n = 0;
   int nbOctetsLus = 0;
   int indicePremier;
   
   if ((f == NULL) || (f->iNoeud == NULL) || (f->iNoeud->prive == NULL)) {
      printk("Dans le cul lulu !\n");
      return -EINVAL;
   }
   tube = f->iNoeud->prive;

   do {
      indicePremier = (tube->indiceProchain + MANUX_TUBE_CAPACITE - tube->taille)%MANUX_TUBE_CAPACITE;

      // On ne lit ni plus que ce qui est demandé, ni plus que ce
      // qu'on a
      n = MIN(nbOctets - nbOctetsLus, tube->taille);
      
      // On ne doit pas aller lire au delà du buffer
      n = MIN(n, (MANUX_TUBE_CAPACITE - indicePremier));

      // On peut donc copier n octets dans le buffer à partir de la
      // position courante, sans risque de déborder
      memcpy(buffer, tube->donnees + indicePremier, n);

      indicePremier = (indicePremier + n) % MANUX_TUBE_CAPACITE;
      tube->taille -= n;
      
      buffer += n;

      nbOctetsLus += n;
   } while (n > 0);
   
   return nbOctetsLus;  
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
   Tube    * tube;

   printk("Coucou les tele tubes !\n");

   // Création de la structure
   tube = kmalloc(sizeof(Tube));
   if (tube == NULL) {
      return ENOMEM;
   }
     
   // Alocation de la mémoire tampon du tube
   if ((tube->donnees = allouerPage()) == NULL) {
      return ENOMEM;
   }

   tube->taille = 0;
   tube->indiceProchain = 0;
   
   // Création de l'iNoeud qui décrit le tube dans le système
   iNoeud = iNoeudCreer(tube, &tubeMethodesFichier);

   // Création du fichier d'entrée du tube (celui où on va écrire)
   fichiers[0] = fichierCreer(iNoeud);

   // Création du fichier de sortie du tube (celui où on va lire)
   fichiers[1] = fichierCreer(iNoeud);

   // On ajoute les fichiers à la tâche
   if (tacheAjouterFichiers(tacheEnCours, 2, fichiers, fds) != 2 ) {
      return ENOMEM;
   }

   printk("On va faire un tube entre %d et %d!\n", fds[0], fds[1]);

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
