/**
 * @file multiconf/printk.h
 * @brief Exemple de configuration pour un test d'accès concurrents
 *
 *                                                     (C) Manu Chaput 2000-2023
 */

#ifdef MANUX_FICHIER_CONFIG
#   error "Cette chose ne devrait pas se produire !"
#   include MANUX_FICHIER_CONFIG
#else
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

#define MANUX_FICHIER_MAIN main-acces-concurrent

#define MANUX_USR_INIT init-acces-concurrent.o
#include <config/usr.h>

#include <config/base.h>
#include <config/console.h>
#include <config/printk.h>
#include <config/appels-systeme.h>  // pour le mode utilsiateur
//#include <config/systeme-fichiers.h>
#include <config/plan-memoire.h>

#include <config/verifications.h>

#endif  // MANUX_CONFIG
#endif  // MANUX_FICHIER_CONFIG
