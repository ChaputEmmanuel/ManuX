/**
 * @file multiconf/test-bloc-ram.h
 * @brief Exemple de configuration pour tester les supports bloc en RAM
 *
 *                                                     (C) Manu Chaput 2000-2026
 */

#ifdef MANUX_FICHIER_CONFIG
#   error "Cette chose ne devrait pas se produire !"
#   include MANUX_FICHIER_CONFIG
#else
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

#define MANUX_FICHIER_MAIN main-test-bloc-ram

#include <config/base.h>
#include <config/pc-i386.h>
#include <config/gestion-memoire.h>
#include <config/plan-memoire-pc.h>  // MANUX_KERNEL_START_ADDRESS nécessaire pour ldd
#include <config/pilotes.h>          // Les supports bloc sont des pilotes
#include <config/support-bloc.h>     // On veut des supports bloc
#include <config/support-bloc-ram.h> // C'est ce qu'on teste !

#include <config/verifications.h>

#endif  // MANUX_CONFIG
#endif  // MANUX_FICHIER_CONFIG
