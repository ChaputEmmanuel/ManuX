/**
 * @file pilotes/support-bloc.c
 * @brief Implantation des support bloc 
 *                                                          (C) Manu Chaput 2026
 */
#include <manux/support-bloc.h>

/**
 * @brief Enregistrement dans le système d'un support bloc
 */
void supportBlocEnregistrer(SupportBloc *sb)
{
   sb->tailleBloc = 512; 
}
