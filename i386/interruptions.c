/**
 * @file : interruptions.c
 * @brief: Implémentation des sous-programmes de gestion des déroutements
 *
 *                                                    (C) Manu Chaput 2000-2023
 */
#include <manux/config.h>
#include <manux/debug.h>          // assert
#include <manux/interruptions.h>
#if (MANUX_PIC == INTEL_8259A)
#   include "manux/intel-8259a.h"
#endif
#include <manux/memoire.h>        // NULL
#include <manux/interBasNiveau.h>
#include <manux/io.h>             // inb
#include <manux/i386.h>           // str
#include <manux/segment.h>        // gdtSysteme
#include <manux/scheduler.h>      // basculerTache
#include <manux/appelsysteme.h>   // MANUX_AS_INT 
#include <manux/atomique.h>
#include <manux/ecran.h>          // Caractéristiques de l'écran
#ifdef MANUX_CONSOLES_VIRTUELLES
#   include <manux/console.h>     // basculerVersConsoleSuivante
#endif
#include <manux/tache.h>          // IntelTSS 
#include <manux/string.h>         // memcpy 

#define MANUX_SELECTEUR_SEGMENT_CODE 0x08      /* WARNING a mettre ailleurs */

#ifdef MANUX_APPELS_SYSTEME
extern void handlerAppelSysteme();  /* WARNING à définir dans un .h */
#endif

/**
 * @brief Combien y a-t-il d'interruptions logicielles ?
 *
 * Il y a en tout MANUX_NB_INTERRUPTIONS interruptions, dont
 * MANUX_NB_EXCEPTIONS exceptions et MANUX_NB_IRQ IRQs.
 */
#define MANUX_NB_SOFT_INT \
  (MANUX_NB_INTERRUPTIONS - (MANUX_NB_EXCEPTIONS + MANUX_NB_IRQ))
void * handlerBasNiveauInterruption[MANUX_NB_SOFT_INT];

/**
 * @brief : la table de gestion des exceptions
 */
FonctionGestionException fonctionDeGestionException[MANUX_NB_EXCEPTIONS];

/**
 * @brief : Fonction de base de gestion d'une exception
 *
 */
void gestionException(uint32_t errCode, uint32_t numEx, TousRegistres registres,
                      uint32_t eip, uint32_t cs, uint32_t eFlags)
{
  fonctionDeGestionException[numEx](errCode, numEx, registres, eip, cs, eFlags);
}

void gestionExceptionPanique(uint32_t errCode, uint32_t itNum, TousRegistres registres,
				uint32_t eip, uint32_t cs, uint32_t eFlags);

/**
 * @frief La table des fonctions de gestion des interruptions
 */
FonctionGestionInterruption fonctionDeGestionInterruption[MANUX_NB_INTERRUPTIONS];

/**
 * @brief : Fonction de base de gestion d'une exception
 *
 */
void gestionInterruption(uint32_t numInt, TousRegistres registres,
                      uint32_t eip, uint32_t cs, uint32_t eFlags)
{
  int idx = numInt-(MANUX_NB_EXCEPTIONS + MANUX_NB_IRQ);
  fonctionDeGestionInterruption[idx](numInt, registres, eip, cs, eFlags);
}

/**
 * @brief Affectation du gestionnaire de l'interruption i
 */
void positionnerHandlerInterruption(IDT idt, int i, Handler handler)
{
   idt[i].itg.offsetFaible = ((uint32_t)handler & 0xFFFF);
   idt[i].itg.offsetFort = ((uint32_t)handler >> 16);
   idt[i].itg.parametres = 0x8E00;
   idt[i].itg.selSegment = MANUX_SELECTEUR_SEGMENT_CODE;
}

/**
 * @brief Chargement effectif de l'IDT
 *
 * lidt prend en paramètre l'adresse d'une zone contenant la taille puis
 * l'adresse de l'IDT.
 */
void chargerIDT(IDT idt)
{
   volatile uint8_t argument[6];

   argument[0] = 0xFF;
   argument[1] = 0x07;
   argument[2] = ( (uint32_t)idt      & 0xFF);
   argument[3] = (((uint32_t)idt>>8)  & 0xFF);
   argument[4] = (((uint32_t)idt>>16) & 0xFF); 
   argument[5] = (((uint32_t)idt>>24) & 0xFF);

   __asm__ __volatile__ ("lidt (%0)"
                         : /* Pas de sortie */
                         :"a" ((char *)argument)
                        );
   //return argument[0] +  argument[1]+argument[2]+argument[3]+ argument[4]+argument[5];

}

/**
 * Une fonction de gestion qui ne fait rien !
 */
void neRienFaire(uint32_t itNum, TousRegistres registres,
                                 uint32_t eip, uint32_t cs, uint32_t eFlags)
{
  printk("Pouet %d\n", itNum);
}

/**
 * Définition de la fonction de gestion d'une interruption
 * @return : 0 si c'est bon
 */
int definirFonctionGestionInterruption(int num,
   				FonctionGestionInterruption fg)
{
   fonctionDeGestionInterruption[num] = fg;

   return 0;
}

/**
 * @brief : Fonction de base de gestion d'un interruption.
 *
 * Elle va se charger de rediriger sur la bonne fonction de gestion
 * de l'interruption s'il en existe une, et sur la fonction de panique
 * sinon.
 */
void gestionGeneraleInterruption(uint32_t itNum, TousRegistres registres,
                                 uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   fonctionDeGestionInterruption[itNum](itNum, registres, eip, cs, eFlags);
}
 
/**
 * @brieg Initialisation de de la table des descripteurs d'interruption
 */
void initialiserIDT()
{
   int i;
   IDT idt = (IDT) MANUX_ADRESSE_IDT;

   // On commence par positionner les handlers "stub" des exceptions
   positionnerHandlerInterruption(idt, 0x00, stubHandlerExceptionDiv0);
   positionnerHandlerInterruption(idt, 0x01, stubHandlerExceptionDebug);
   positionnerHandlerInterruption(idt, 0x02, stubHandlerExceptionNMI);
   positionnerHandlerInterruption(idt, 0x03, stubHandlerExceptionBreakpoint);
   positionnerHandlerInterruption(idt, 0x04, stubHandlerExceptionOverflow);
   positionnerHandlerInterruption(idt, 0x05, stubHandlerExceptionBoundExceeded);
   positionnerHandlerInterruption(idt, 0x06, stubHandlerExceptionDeviceInvalidOpcode);
   positionnerHandlerInterruption(idt, 0x07, stubHandlerExceptionDeviceUnavailable);
   positionnerHandlerInterruption(idt, 0x08, stubHandlerExceptionDoubleFault);
   positionnerHandlerInterruption(idt, 0x09, stubHandlerExceptionCoproOverrun);
   positionnerHandlerInterruption(idt, 0x0a, stubHandlerExceptionInvalidTSS);
   positionnerHandlerInterruption(idt, 0x0b, stubHandlerExceptionSegmentNotPresent);
   positionnerHandlerInterruption(idt, 0x0c, stubHandlerExceptionStackSegmentFault);
   positionnerHandlerInterruption(idt, 0x0d, stubHandlerExceptionGeneralProtectionFault);
   positionnerHandlerInterruption(idt, 0x0e, stubHandlerExceptionPageFault);
   positionnerHandlerInterruption(idt, 0x0f, stubHandlerExceptionReserved);
   positionnerHandlerInterruption(idt, 0x10, stubHandlerExceptionFloatingPoint);
   positionnerHandlerInterruption(idt, 0x11, stubHandlerExceptionAlignmentCheck);
   positionnerHandlerInterruption(idt, 0x12, stubHandlerExceptionFloatingMachineCheck);
   positionnerHandlerInterruption(idt, 0x13, stubHandlerExceptionFloatingSIMDFPE);
   positionnerHandlerInterruption(idt, 0x14, stubHandlerExceptionFloatingVirtualization);
   positionnerHandlerInterruption(idt, 0x15, stubHandlerExceptionControlProtection);
   positionnerHandlerInterruption(idt, 0x16, stubHandlerExceptionReserved2);
   positionnerHandlerInterruption(idt, 0x17, stubHandlerExceptionReserved3);
   positionnerHandlerInterruption(idt, 0x18, stubHandlerExceptionReserved4);
   positionnerHandlerInterruption(idt, 0x19, stubHandlerExceptionReserved5);
   positionnerHandlerInterruption(idt, 0x1a, stubHandlerExceptionReserved6);
   positionnerHandlerInterruption(idt, 0x1b, stubHandlerExceptionReserved7);
   positionnerHandlerInterruption(idt, 0x1c, stubHandlerExceptionHypervisionInjection);
   positionnerHandlerInterruption(idt, 0x1d, stubHandlerExceptionVMMCommunication);
   positionnerHandlerInterruption(idt, 0x1e, stubHandlerExceptionSecurity);
   positionnerHandlerInterruption(idt, 0x1f, stubHandlerExceptionReserved8);

   // On configure l'aiguillage des exceptions. Par défaut, on panique !
   for (i = 0; i < MANUX_NB_EXCEPTIONS; i++) {
      fonctionDeGestionException[i] = gestionExceptionPanique;
   }

   // Maintenant, les exceptions que l'on veut gérer
   // (pour le moment aucune !)

   // Les IRQs sont gérées au travers du PIC. On positionne les
   // handlers bas niveau
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  0, stubHandlerIRQ0);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  1, stubHandlerIRQ1);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  2, stubHandlerIRQ2);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  3, stubHandlerIRQ3);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  4, stubHandlerIRQ4);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  5, stubHandlerIRQ5);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  6, stubHandlerIRQ6);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  7, stubHandlerIRQ7);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  8, stubHandlerIRQ8);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ +  9, stubHandlerIRQ9);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 10, stubHandlerIRQ10);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 11, stubHandlerIRQ11);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 12, stubHandlerIRQ12);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 13, stubHandlerIRQ13);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 14, stubHandlerIRQ14);
   positionnerHandlerInterruption(idt, MANUX_INT_BASE_IRQ + 15, stubHandlerIRQ15);

   // On vérifie qu'on est en cohérence avec le nombre d'IRQ défini
   // par le PIC
   assert(MANUX_NB_IRQ == 16);

   // L'aiguillage des IRQ est réalisé par le pilote du PIC que l'on
   // initialise maintenant
   MANUX_PIC_INIT(MANUX_INT_BASE_IRQ);

   // On va chercher les adresses des fonctions d'interruption
   initialiserHandlersInterruption(handlerBasNiveauInterruption, MANUX_NB_SOFT_INT);

   // On peuple l'IDT avec ces pointeurs
   for (i = 0; i < MANUX_NB_SOFT_INT; i++) {
      positionnerHandlerInterruption(idt,
				     MANUX_INT_BASE_IRQ + MANUX_NB_IRQ + i,
				     handlerBasNiveauInterruption[i]);
   }
   
   // On configure l'aiguillage des interruptions logicielles. Par
   // défaut, on ne fait rien
   for (i = 0; i < MANUX_NB_SOFT_INT; i++) {
      fonctionDeGestionInterruption[i] = neRienFaire;
   }

   printk("-> 0x%x <-\n", handlerBasNiveauInterruption[18]);
   printk(" ( 0x%x )\n",  stubHandlerInt66);
   
#ifdef MANUX_APPELS_SYSTEME
   /* Le handler de l'interruption utilisée pour les appels système */
   positionnerHandlerInterruption(idt, MANUX_AS_INT, handlerAppelSysteme);
#endif
   
   // On charge finalement l'IDT !
   chargerIDT(idt);
}


/*
 * Affichage de la valeur v en hexa sur n chiffres à la position l, c
 */
#define afficherHexa(v, n, l, c)		                        \
{                                                                       \
   uint32_t __val = (uint32_t) v;						\
   for (int __i = MANUX_CON_COLONNES*l+c+n-1; __i>=MANUX_CON_COLONNES*l+c; __i--) { \
      ecran[2*__i] = chiffre[__val&15] ;                                \
      __val /=16 ;			                                \
   }                                                                    \
}
#define afficherBin(v, n, l, c)		                                \
{                                                                       \
   uint32_t __val = (uint32_t) v;						\
   for (int __i = MANUX_CON_COLONNES*l+c+n-1; __i>=MANUX_CON_COLONNES*l+c; __i--) { \
      ecran[2*__i] = chiffre[__val&1] ;                                 \
      __val /=2 ;			                                \
   }                                                                    \
}

/*
 * Pour sauver l'ecran caché par l'écran bleu de la mort
 */
static char bufferEcran[4000];

void ecranDeLaMort(uint32_t errCode, uint32_t itNum, TousRegistres registres,
		    uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   /* A définir ailleurs lorsque ce sera au point */
   char *ecranPanique ="\
+-------------+-------------------+--------32--28--24--20--16--12---8---4---0--+\
| it : 0x--   | tss  = 0x-------- | eflags=  --------------------------------  |\
+-------------+-------------------+-------------+---+---+---+---+---+---+---+--+\
| cs : 0x---- | eip  = 0x-------- | cr3  = 0x                                  |\
+-------------+-------------------+--------------------------------------------+\
| ss : 0x---- | esp  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+\
| ds : 0x---- | esi  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+\
|             | edi  = 0x-------- | err = 0x                                   |\
+-------------+-----------------(ESC pour cacher)------------------------------+";
   
   char chiffre[16] = "0123456789ABCDEF";
   char * ecran = MANUX_CON_SCREEN;
   int i;
   uint32_t indice;

   /* Sauvegardons l'écran dans un buffer avant de le pourrir */
   memcpy(bufferEcran, ecran, 4000);

   // On fait un peu de place en haut de l'écran
   for (i=0; i<11*2*MANUX_CON_COLONNES; i+=2) {
      ecran[i]   = ecranPanique[i/2];
      ecran[i+1] = COUL_FOND_BLEU | COUL_TXT_JAUNE;
   }

   __asm__ __volatile__("str %%eax" : "=a" (indice));
   
   Descripteur desc = gdtSysteme->descripteur[indice>>3];
   IntelTSS * tssDuFautif = (IntelTSS *)(
         (((uint32_t)desc.dt.baseFort) << 24)
       + (((uint32_t)desc.dt.baseInter) << 16)
       + desc.dt.baseFaible
     );

   // Première ligne
   afficherHexa(itNum, 2, 1, 9);
   afficherHexa(tssDuFautif, 8, 1, 25);
   //afficherBin(tssDuFautif->EFLAGS, 32, 1, 45);
   afficherBin(eFlags, 32, 1, 45);
   afficherHexa(tssDuFautif->CR3, 8, 3, 45);

   // Les descripteurs de segment
   afficherHexa(cs, 4, 3, 9);
   afficherHexa(tssDuFautif->SS, 4, 5, 9);
   afficherHexa(tssDuFautif->DS, 4, 7, 9);

   // Les pointeurs
   //afficherHexa(tssDuFautif->EIP, 8, 3, 25);
   afficherHexa(eip, 8, 3, 25);
   afficherHexa(tssDuFautif->ESP, 8, 5, 25);
   afficherHexa(tssDuFautif->ESI, 8, 7, 25);
   afficherHexa(tssDuFautif->EDI, 8, 9, 25);

   afficherHexa(errCode, 8, 9, 45);
}

/**
 * @brief : Gestion (non) d'une exception non connue.
 *
 * On affichera ce que l'on peut à l'écran pour aider l'utilisateur, ...
 */
void gestionExceptionPanique(uint32_t errCode, uint32_t itNum, TousRegistres registres,
		    uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   char * ecran = MANUX_CON_SCREEN;
   int d = 1;
   char c;

   ecranDeLaMort(errCode, itNum, registres, eip, cs, eFlags);

   while (1) {
      inb(0x60, c);
      switch (c) {
         case 0x01: // ESC
#ifdef MANUX_CONSOLES_VIRTUELLES	   
	    basculerVersConsoleSuivante();
#endif
	    while (c == 0x81){
	       inb(0x60, c);
            };
         break;
         case 0x39: // SPACE On alterne entre l'écran bleu et l'écran au moment du drame 
            d = 1-d;
	    if (d) {
  	      ecranDeLaMort(errCode, itNum, registres, eip, cs, eFlags);
 	    }else{
	      // On restaure l'écran
              memcpy(ecran, bufferEcran, 4000);
	    };
            while (c == 0x39){
	      inb(0x60, c);
            };
         break;
      }
   }
   halt();
}
