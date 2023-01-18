/*----------------------------------------------------------------------------*/
/*      Définition des éléments liés aux interruptions.                       */
/*                                     (C) Manu Chaput 2000, 2001, 2002, 2003 */
/*----------------------------------------------------------------------------*/
#ifndef INTERRUPTION_DEF
#define INTERRUPTION_DEF

#include <manux/types.h>

/*
 * Définition d'un Task Gate Descriptor ([1] p 6-9, 5-14)
 */
typedef struct {
   uint16_t reserve1;
   uint16_t tss;
   uint8_t  reserve2;
   uint8_t  parametres;
   uint16_t reserve3;
} TaskGateDescriptor;

/*
 * Définition d'une Interrupt Gate ([1] p 5-14)
 */
typedef struct {
   uint16_t offsetFaible;
   uint16_t selSegment;
   uint16_t parametres;
   uint16_t offsetFort;
} InterruptGate;

/*
 * Définition d'une Trap Gate ([1] p 5-14)
 */
typedef struct {
   uint16_t offsetFaible;
   uint16_t selSegment;
   uint16_t parametres;
   uint16_t offsetFort;
} TrapGate;

/*
 * Description des enregistrements de l'IDT
 */
typedef union {
   TaskGateDescriptor tsg;
   InterruptGate      itg;
   TrapGate           trg;
} IDTGate;

/*
 * L'IDT est un tableau de 256 (max) IDTGate
 */
typedef IDTGate * IDT;

/*
 * Type des fonctions servant de handler d'interruption
 */
typedef void (Handler());

void initialiserIDT();
/*
 * Initialiser l'IDT (Interrupt Description Table)
 */

typedef void (* FonctionGestionInteruption)(uint32_t itNum, TousRegistres registres,
         uint32_t eip, uint32_t cs, uint32_t eFlags);
/**
 * Le type d'une fonction de e gestion d'interruption
 */

int definirFonctionGestionInteruption(int num,
   				FonctionGestionInteruption fg);
/**
 * Définition de la fonction de gestion d'une interuption
 * @return : 0 si c'est bon
 */

#define cli() __asm__("cli"::);
/*
 * Interdire les interruptions
 * WARNING : ce n'est peut être pas le meilleur endroit où le définir
 */

#define sti() __asm__("sti"::);
/*
 * Autoriser les interruptions
 * WARNING : ce n'est peut être pas le meilleur endroit où le définir
 */

/*
 * Nombre de tops d'horloge depuis le boot
 */

void setFrequenceTimer(uint16_t freqHz);
/*
 * Positionne la fréquence du timer 0
 */

#endif
