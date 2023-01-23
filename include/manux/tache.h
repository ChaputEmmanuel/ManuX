/*----------------------------------------------------------------------------*/
/*      Définition des taches de ManuX-32.                                    */
/*                                                                            */
/*                                     (C) Manu Chaput 2000, 2001, 2002, 2003 */
/*----------------------------------------------------------------------------*/
#ifndef TACHES_DEF
#define TACHES_DEF

#include <manux/config.h>     /* TAILLE_PAGE */

#include <manux/types.h>
#include <manux/interruptions.h>
#include <manux/segment.h>        /* Pour la descriptor table */
#include <manux/fichier.h>

/*
 * Taille de la Local Descriptor Table de chaque tâche
 */
#define LDT_NB_BYTES 1024 /* WARNING, pas top */

/*
 * Les différents états possibles pour une tâche
 */
typedef enum _EtatTache {
   Tache_En_Cours   = 1,    // la tâche courante
   Tache_Prete      = 2,    // les tâches en attente du processeur
   Tache_Bloquee    = 3     // sur un sémaphore par ex
} EtatTache;

/*
 * Définition du type du "main" d'une nouvelle tâche
 */
typedef void (CorpsTache());

/*
 * Définition de la structure TSS (Task State Segment)
 * WARNING : à mettre dans i386/processeur.h
 */
typedef struct _IntelTSS {
   uint16_t TSSPrecedent;  /* Pour le chaînage */
   uint16_t Reserve1;
   uint32_t ESP0;
   uint16_t SS0;
   uint16_t Reserve2;
   uint32_t ESP1;
   uint16_t SS1;
   uint16_t Reserve3;
   uint32_t ESP2;
   uint16_t SS2;
   uint16_t Reserve4;
   uint32_t CR3;           /* Page Directory Base Register */
   uint32_t EIP;
   uint32_t EFLAGS;
   uint32_t EAX, ECX, EDX,
          EBX, ESP, EBP,
          ESI, EDI;      /* Les registres ! */
   uint16_t ES;            /* Le sélecteur de segment */
   uint16_t Reserve5;
   uint16_t CS;            /* Le sélecteur de segment */ 
   uint16_t Reserve6;
   uint16_t SS;            /* Le sélecteur de segment */ 
   uint16_t Reserve7;
   uint16_t DS;            /* Le sélecteur de segment */ 
   uint16_t Reserve8;
   uint16_t FS;            /* Le sélecteur de segment */ 
   uint16_t Reserve9;
   uint16_t GS;            /* Le sélecteur de segment */ 
   uint16_t Reserve10;
   uint16_t LDT;           /* La Local Descriptor Table */
   uint16_t Reserve11;
   uint16_t Reserve12;
   uint16_t IOMBA;
} IntelTSS;

/*
 * Définition du type décrivant une tache.
 */
typedef struct _Tache {
   IntelTSS           tss;
   DescriptorTable *  ldt;
   uint16_t           indiceTSSDescriptor; /* Indice du TSS dans la GDT */

   TacheID            numero;
   EtatTache          etat;
   void             * tailleMemoire;             /* en octets */
  
#ifdef MANUX_TACHE_CONSOLE
   struct _Console  * console;
#endif
#ifdef MANUX_FS
  Fichier             fichiers[MANUX_NB_MAX_FICHIERS]; // WARNING à généraliser
#endif
   uint32_t           nbActivations;   // Décompte du nombre d'activations
   Temps              tempsExecution;  // Cumul du temps d'exécution
} Tache;

/*
 * Chaque tâche pourra voir les infos la concernant à cette adresse
 */
extern Tache * tacheCourante;

#ifdef MANUX_TACHE_CONSOLE
Tache * creerTache(CorpsTache corpsTache, struct _Console * cons);
#else
Tache * creerTache(CorpsTache corpsTache);
#endif

/*
 * Création d'une nouvelle tâche. Attention, elle doit ensuite être
 * insérée dans la liste des tâches en cours.
 */

void basculerVersTache(Tache * tache);
/*
 * Exécuter une tâche. A l'utilisation exclusive du scheduler
 */

TacheID sysFork();
/*
 * Implantation de l'appel système fork
 */

#endif
