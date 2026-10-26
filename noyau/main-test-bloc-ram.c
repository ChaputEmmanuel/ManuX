/**
 * @file: noyau/main-test-bloc-ram.c
 * @brief: Un noyau qui teste les supports bloc en RAM
 *                                                                           
 *                                                     (C) Manu Chaput 2000-2026
 */
#include <manux/config.h>
#include <manux/memoire.h>
#include <manux/support-bloc.h>
#include <manux/support-bloc-ram.h>

void startManuX(void);

void startManuX(void)
{
   SupportBloc sbRAM1, sbRAM2;
   int nbPages = 16;
   void * a1, * a2;

   a1 = allouerPages(nbPages); 
   a2 = allouerPages(nbPages);

   supportBlocRAMInitialiser(&sbRAM1, a1, nbPages*MANUX_TAILLE_PAGE);
   supportBlocRAMInitialiser(&sbRAM2, a2, nbPages*MANUX_TAILLE_PAGE);
   
}   // startManuX

