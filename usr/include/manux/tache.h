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
 */
typedef struct _IntelTSS {
   uint16 TSSPrecedent;  /* Pour le chaînage */
   uint16 Reserve1;
   uint32 ESP0;
   uint16 SS0;
   uint16 Reserve2;
   uint32 ESP1;
   uint16 SS1;
   uint16 Reserve3;
   uint32 ESP2;
   uint16 SS2;
   uint16 Reserve4;
   uint32 CR3;           /* Page Directory Base Register */
   uint32 EIP;
   uint32 EFLAGS;
   uint32 EAX, ECX, EDX,
          EBX, ESP, EBP,
          ESI, EDI;      /* Les registres ! */
   uint16 ES;            /* Le sélecteur de segment */
   uint16 Reserve5;
   uint16 CS;            /* Le sélecteur de segment */ 
   uint16 Reserve6;
   uint16 SS;            /* Le sélecteur de segment */ 
   uint16 Reserve7;
   uint16 DS;            /* Le sélecteur de segment */ 
   uint16 Reserve8;
   uint16 FS;            /* Le sélecteur de segment */ 
   uint16 Reserve9;
   uint16 GS;            /* Le sélecteur de segment */ 
   uint16 Reserve10;
   uint16 LDT;           /* La Local Descriptor Table */
   uint16 Reserve11;
   uint16 Reserve12;
   uint16 IOMBA;
} IntelTSS;

/*
 * Définition du type décrivant une tache.
 */
typedef struct _Tache {
   IntelTSS           tss;
   uint16             tss_LDT;
   DescriptorTable *  ldt;
   uint16             indiceTSSDescriptor; /* Indice du TSS dans la GDT */
   TacheID            numero;
   EtatTache          etat;
   void             * tailleMemoire;       /* en octets */
   struct _Console  * console;                   /* WARNING à virer ? */
   Fichier            fichiers[NB_MAX_FICHIERS]; /* WARNING à généraliser */
} Tache;

/*
 * Chaque tâche pourra voir les infos la concernant à cette adresse
 */
extern Tache * tacheCourante;

Tache * creerTache(CorpsTache corpsTache, struct _Console * cons);
/*
 * Création d'une nouvelle tâche. Attention, elle doit ensuite être
 * insérée dans la liste des tâches en cours.
 */

void activerTache(Tache * tache);
/*
 * Activation d'une tâche. A l'utilisation exclusive du scheduler
 */

TacheID sysFork();
/*
 * Implantation de l'appel système fork
 */

#endif
