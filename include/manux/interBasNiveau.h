/*----------------------------------------------------------------------------*/
/*      Définition des fonctions de bas-niveau permettant de manipuler les    */
/*   interruptions.                                                           */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#ifndef INTER_BAS_NIVEAU_DEF
#define INTER_BAS_NIVEAU_DEF

void stubHandlerNop();
/*
 * Un handler de bas niveau qui ne fait rien
 */

void stubHandlerIRQ0();
void stubHandlerIRQ1();
void stubHandlerIRQ2();
void stubHandlerIRQ3();
void stubHandlerIRQ4();
void stubHandlerIRQ5();
void stubHandlerIRQ6();
void stubHandlerIRQ7();
void stubHandlerIRQ8();
void stubHandlerIRQ9();
void stubHandlerIRQ10();
void stubHandlerIRQ11();
void stubHandlerIRQ12();
void stubHandlerIRQ13();
void stubHandlerIRQ14();
void stubHandlerIRQ15();

void stubHandlerPanique_0();
/*
 * Un handler de bas niveau qui affiche quelques informations
 */
void stubHandlerPanique_1();
void stubHandlerPanique_2();
void stubHandlerPanique_3();
void stubHandlerPanique_4();
void stubHandlerPanique_5();
void stubHandlerPanique_6();
void stubHandlerPanique_7();
void stubHandlerPanique_8();
void stubHandlerPanique_9();
void stubHandlerPanique_10();
void stubHandlerPanique_11();
void stubHandlerPanique_12();
void stubHandlerPanique_13();
void stubHandlerPanique_14();
void stubHandlerPanique_15();
void stubHandlerPanique_16();
void stubHandlerPanique_17();
void stubHandlerPanique_18();
void stubHandlerPanique_19();
void stubHandlerPanique_20();
void stubHandlerPanique_21();
void stubHandlerPanique_22();
void stubHandlerPanique_23();
void stubHandlerPanique_24();
void stubHandlerPanique_25();
void stubHandlerPanique_26();
void stubHandlerPanique_27();
void stubHandlerPanique_28();
void stubHandlerPanique_29();
void stubHandlerPanique_30();
void stubHandlerPanique_31();

#endif
