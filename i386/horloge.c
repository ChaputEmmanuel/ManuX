/*----------------------------------------------------------------------------*/
/*      Implémentation des sous-programmes de gestion de l'horloge matérielle.*/
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/config.h>
#include <manux/intel-8259a.h>
#include <manux/horloge.h>
#include <manux/io.h>           // outb
#include <manux/memoire.h>      // NULL
#include <manux/scheduler.h>    // ordonnanceur

/*
 * Nous allons décompter avec cette variable le nombre d'interruptions d'horloge
 */
Temps nbTopHorloge = 0;

/*
 * Le handler du timer
 */
void handlerHorloge(void * inutile)
{
   nbTopHorloge++;

#ifdef MANUX_PREEMPTIF
   ordonnanceur();
#endif
}

/*
 * Configuration de la fréquence du circuit
 */ 
void setFrequenceHorloge(uint16_t freqHz)
{
   uint16_t decompte;

   decompte = 1193200 / freqHz;

   // On initialise la fréquence du timer 0 WARNING a rendre plus propre
   outb(0x43, 0x34); // wAS 36
   outb(0x40, decompte & 0xFF);
   outb(0x40, (decompte >> 8) & 0xFF);
}

void initialiserHorloge()
{
   // Initialisation de la fréquence de l'horloge matérielle
   setFrequenceHorloge(MANUX_FREQUENCE_HORLOGE);

   // Enregistrement auprès du PIC
   i8259aAjouterHandler(IRQ_HORLOGE, handlerHorloge, NULL);

   // Autorisation de l'IRQ
   i8259aAutoriserIRQ(IRQ_HORLOGE);
}
