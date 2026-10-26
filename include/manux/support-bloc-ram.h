/**
 * @file include/manux/support-bloc.h
 * @brief Définition des supports orientés bloc
 *                                                          (C) Manu Chaput 2026
 */
#ifndef DEF_MANUX_SUPPORT_BLOC_RAM
#define DEF_MANUX_SUPPORT_BLOC_RAM

#include <manux/support-bloc.h>

/**
 * @bref Initialisation d'un support bloc en RAM
 * @param sb le support bloc à initialiser
 * @param memoire pointeur sur une zome mémoire allouées
 * @param taille en octets de la zone mémoire allouée
 */
void supportBlocRAMInitialiser(SupportBloc * sb, void * memoire, uint32_t taille);

#endif

