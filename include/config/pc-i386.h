/*============================================================================*/
/*   Définitions liées au processeur.                                         */
/*============================================================================*/
/**
 * Utilise-t-on des spécificité i386 ?
 */
#define MANUX_LIBI386

/*----------------------------------------------------------------------------*/
/*   Configuration des interruptions                                          */ 
/*----------------------------------------------------------------------------*/

/**
 * Combien d'interuptions ?
 */
#ifndef MANUX_NB_INTERUPTIONS
#   define MANUX_NB_INTERUPTIONS 256
#endif

/*
 * On utilise ici des intel 8259a
 */
#ifndef MANUX_HANDLER_IRQ
#   define MANUX_HANDLER_IRQ i8259aGestionIRQ
#endif

/*
 * Combien de handlers peut-on greffer sur une interruption ?
 */
#ifndef MANUX_NB_HANDLER_PAR_IRQ
#   define MANUX_NB_HANDLER_PAR_IRQ 4
#endif

/*
 * Premier numéro d'interruption utilisé pour repositionner les IRQs
 */
#ifndef MANUX_INT_BASE_IRQ
#   define MANUX_INT_BASE_IRQ 0x20
#endif

/*
 * On a deux circuits et donc 16 IRQ potentielles
 */
#ifndef I8259A_NB_IRQ
#   define I8259A_NB_IRQ 16
#endif

/*
 * Les IRQ des matériels pris en charge
 */
#define IRQ_HORLOGE   0
#define IRQ_CLAVIER   1
