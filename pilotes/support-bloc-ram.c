/**
 * @file pilotes/support-bloc-ram.c
 * @brief Implantation des support bloc en RAM
 *                                                     (C)      Manu Chaput 2026
 *                                                           Baptiste Gomez 2026
 */
#include "manux/support-bloc.h"
#include <manux/support-bloc-ram.h>

/**
 * @brief Lecture de blocs depuis un support
 * @parameter supportBloc le support depuis lequel on souhaite lire
 * @parameter indice le numéro du premier bloc lu (on commence à 0)
 * @parameter nombre le nomnre de blocs à lire
 * @parameter buffer la zeone mémoire où les données sont placées
 * @return le nombre de blocs lus
 */
static
uint32_t supportBlocRamLire(SupportBloc *supportBloc, uint32_t indice,
                            uint32_t nombre, void *buffer)
{
   int indiceDebut = indice * (supportBloc->tailleBloc);
   int tailleLecture = nombre * (supportBloc->tailleBloc);

   uint8_t * donnees = (uint8_t*) supportBloc->prive;
   uint8_t * buffer8 = (uint8_t*) buffer;

   for (int i = 0; i < tailleLecture; i++) {
        buffer8[i] = donnees[i+indiceDebut];
   }
   return nombre;
}

/**
 * @brief Écriture de blocs sur un support
 * @parameter supportBloc le support vers lequel on souhaite écrire
 * @parameter indice le numéro du premier blocé écrit (on commence à 0)
 * @parameter nombre le nomnre de blocs à lire
 * @parameter buffer la zeone mémoire où les données sont trouvés
 * @return le nombre de blocs écris
 */
static
uint32_t supportBlocRamEcrire(SupportBloc *supportBloc, uint32_t indice,
                              uint32_t nombre, void *buffer)
{
   int indiceDebut = indice * (supportBloc->tailleBloc);
   int tailleLecture = nombre * (supportBloc->tailleBloc);

   uint8_t * donnees = (uint8_t*) supportBloc->prive;
   uint8_t * buffer8 = (uint8_t*) buffer;

   for (int i = 0; i < tailleLecture; i++) {
        donnees[i+indiceDebut] = buffer8[i];
   }
   return nombre;
}

/**
 * @brief Les méthodes associées à un supportBlocRam
 */
SupportBlocMethodes supportBlocRamMethodes = {
   .lire = supportBlocRamLire,
   .ecrire = supportBlocRamEcrire
};

void supportBlocRAMInitialiser(SupportBloc * sb, void * memoire, uint32_t taille)
{
   // Fonctions par defaut
   sb->methodes = &supportBlocRamMethodes;

   // Initialisation SupportBlocRAM
   sb->prive = memoire;
   sb->tailleBloc = TAILLE_BLOC_RAM;
   sb->nombreBlocs = taille / sb->tailleBloc;

   return;
}
