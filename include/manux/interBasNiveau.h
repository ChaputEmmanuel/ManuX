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

/**
 * Les gestionnaires bas niveau des exceptions
 */
void stubHandlerExceptionDiv0();
void stubHandlerExceptionDebug();
void stubHandlerExceptionNMI();
void stubHandlerExceptionBreakpoint();
void stubHandlerExceptionOverflow();
void stubHandlerExceptionBoundExceeded();
void stubHandlerExceptionDeviceInvalidOpcode();
void stubHandlerExceptionDeviceUnavailable();
void stubHandlerExceptionDoubleFault();
void stubHandlerExceptionCoproOverrun();
void stubHandlerExceptionInvalidTSS();
void stubHandlerExceptionSegmentNotPresent();
void stubHandlerExceptionStackSegmentFault();
void stubHandlerExceptionGeneralProtectionFault();
void stubHandlerExceptionPageFault();
void stubHandlerExceptionReserved();
void stubHandlerExceptionFloatingPoint();
void stubHandlerExceptionAlignmentCheck();
void stubHandlerExceptionFloatingMachineCheck();
void stubHandlerExceptionFloatingSIMDFPE();
void stubHandlerExceptionFloatingVirtualization();
void stubHandlerExceptionControlProtection();
void stubHandlerExceptionReserved2();
void stubHandlerExceptionReserved3();
void stubHandlerExceptionReserved4();
void stubHandlerExceptionReserved5();
void stubHandlerExceptionReserved6();
void stubHandlerExceptionReserved7();
void stubHandlerExceptionHypervisionInjection();
void stubHandlerExceptionVMMCommunication();
void stubHandlerExceptionSecurity();
void stubHandlerExceptionReserved8();


/**
 * Les gestionnaires bas niveau des IRQ
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

void stubHandlerInt66();

void initialiserHandlersInterruption(void * table, uint32_t taille);
#endif
