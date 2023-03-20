/**
 * @file config/base.h
 * @brief Eléments de base de la configuration
 *
 * Il s'agit de quelques macros et constantes de base sans conséquence
 * majeure sur la compilation du noyau. Ce ne sont pas des paramètres
 * qui permettent de sélectionner ou même parametrer les
 * sous-systèmes.
 *
 *                                                    (C) E. Chaput 2022-2023 */

/**
 * @brief la signature d'un chargement par init-manux
 */
#ifndef MANUX_INIT_MAGIC
#   define MANUX_INIT_MAGIC 0x01c0ffee
#endif
