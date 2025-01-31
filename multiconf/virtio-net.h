/**
 * @file multiconf/virtio-net.h
 * @brief Utilisation d'une interface réseau de type virtio
 *
 *                                                     (C) Manu Chaput 2000-2024
 */

#ifdef MANUX_FICHIER_CONFIG
#   error "Cette chose ne devrait pas se produire !"
#   include MANUX_FICHIER_CONFIG
#else
#ifndef MANUX_CONFIG
#define MANUX_CONFIG

#define MANUX_FICHIER_MAIN main-virtio-net

#include <config/base.h>
#include <config/bootloader.h>       // Nécessaire pour construire mon bootloader
#include <config/plan-memoire.h>
#include <config/gestion-memoire.h>
#include <config/kmalloc.h>
#include <config/pc-i386.h>
#include <config/synchronisation.h>
#include <config/console.h>
#include <config/printk.h>
#include <config/taches.h>
#include <config/stdlib.h>
#include <config/reseau.h>
#include <config/pci.h>
#include <config/virtio.h>
#undef MANUX_VIRTIO_CONSOLE

/***** PREVIOUS
       
#include <config/base.h>
#include <config/bootloader.h>
#include <config/plan-memoire.h>
#include <config/gestion-memoire.h>
#include <config/kmalloc.h>   // Parce que dans tache, on hérite des fichiers
#include <config/taches.h>
#include <config/synchronisation.h>
#undef MANUX_EXCLUSION_MUTUELLE_AUDIT
#undef MANUX_CONDITION_AUDIT
#include <config/console.h>
#include <config/printk.h>
#include <config/pc-i386.h>



*******/

#include <config/verifications.h>

#endif  // MANUX_CONFIG
#endif  // MANUX_FICHIER_CONFIG
