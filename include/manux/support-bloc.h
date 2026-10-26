/**
 * @file include/manux/support-bloc.h
 * @brief Définition des supoprts orientés bloc
 *                                                          (C) Manu Chaput 2026
 */
#ifndef DEF_MANUX_SUPPORT_BLOC
#define DEF_MANUX_SUPPORT_BLOC

#include <manux/types.h>

typedef struct _SupportBlocMethodes SupportBlocMethodes;

/**
 * @brief La structure définissant un support bloc
 */
typedef struct _SupportBloc {
   uint16_t tailleBloc ;              //< La taille du bloc
   uint32_t nombreBlocs ;             //< Le nombre total de blocs
   SupportBlocMethodes * methodes;
   void * prive;
} SupportBloc;

/**
 * @brief Définition des fonctions de manipulation d'un support bloc
 */
typedef struct _SupportBlocMethodes {
   uint32_t (*lire)(SupportBloc *supportBloc,
                    uint32_t indice, uint32_t nombre, void * buffer);
   uint32_t (*ecrire)(SupportBloc *supportBloc,
                      uint32_t indice, uint32_t nombre, void * buffer);
} SupportBlocMethodes;
   
/**
 * @brief Enregistrement dans le système d'un support bloc
 */
void supportBlocEnregistrer(SupportBloc *sb);

#endif
