/**
 * @file pilotes/support-bloc-ram.c
 * @brief Implantation des support bloc en RAM
 *                                                          (C) Manu Chaput 2026
 */
#include <manux/support-bloc-ram.h>

/**
 * @brief Lecture de blocs depuis un support
 * @parameter supportBloc le support depuis lequel on souhaite lire
 * @parameter indice le numéro du premier bloc lu (on commence à 0)
 * @parameter nombre le nomnre de blocs à lire
 * @parameter buffer la zeone mémoire où les données sont placées
 * @return le nombre de blocs lus
 */
uint32_t supportBlocRamLire(SupportBloc *supportBloc, uint32_t indice,
                            uint32_t nombre, void *buffer)
{
}

/**
 * @brief Écriture de blocs sur un support
 * @parameter supportBloc le support vers lequel on souhaite écrire
 * @parameter indice le numéro du premier blocé écrit (on commence à 0)
 * @parameter nombre le nomnre de blocs à lire
 * @parameter buffer la zeone mémoire où les données sont trouvés
 * @return le nombre de blocs écris
 */
uint32_t supportBlocRamEcrire(SupportBloc *supportBloc, uint32_t indice,
                              uint32_t nombre, void *buffer)
{
  
}

void supportBlocRAMInitialiser(SupportBloc * sb, void * memoire, uint32_t taille)
{
   sb->tailleBloc = 512;
}
