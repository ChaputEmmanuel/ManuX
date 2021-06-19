/*----------------------------------------------------------------------------*/
/*      Implémentation des sous-programmes de gestion des déroutements.       */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/interruptions.h>

#include <manux/config.h>
#include <manux/interBasNiveau.h>
#include <manux/io.h>
#include <manux/i386.h>           /* str */
#include <manux/segment.h>        /* gdtSysteme */
#include <manux/scheduler.h>      /* basculerTache */
#include <manux/appelsysteme.h>   /* MANUX_AS_INT */
#include <manux/atomique.h>
#include <manux/console.h>        /* Caractéristiques de l'écran */
#include <manux/tache.h>          /* IntelTSS */
#include <manux/string.h>         /* memcpy */

#define MANUX_SELECTEUR_SEGMENT_CODE 0x08      /* WARNING a mettre ailleurs */

#ifdef MANUX_APPELS_SYSTEME
extern void handlerAppelSysteme();  /* WARNING à définir dans un .h */
#endif

/*
 * Nous allons décompter avec cette variable le nombre d'interruptions d'horloge
 */
Temps nbTopHorloge = 0;

void exDivisionParZero(TousRegistres registres,
                       uint32_t eip, uint32_t cs, uint32_t eFlags)
{
}

/*
 * Le handler du timer (irq 8)
 */
void handlerTimer(TousRegistres registres,
                  uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   nbTopHorloge++;

   outb(0x20, 0x20);

#ifdef MANUX_PREEMPTIF
   ordonnanceur();
#endif
}

void setFrequenceTimer(uint16_t freqHz)
{
   uint16_t decompte;

   decompte = 1193200 / freqHz;

   /* On initialise la fréquence du timer 0 WARNING a rendre plus propre */
   outb(0x43, 0x34); // wAS 36
   outb(0x40, decompte & 0xFF);
   outb(0x40, (decompte >> 8) & 0xFF);
}

void positionnerHandlerInterruption(IDT idt, int i, Handler handler)
/*
 * Affectation du handler de l'interruption i
 */
{
   idt[i].itg.offsetFaible = ((uint32_t)handler & 0xFFFF);
   idt[i].itg.offsetFort = ((uint32_t)handler >> 16);
   idt[i].itg.parametres = 0x8E00;
   idt[i].itg.selSegment = MANUX_SELECTEUR_SEGMENT_CODE;
}

void chargerIDT(IDT idt)
/*
 * Chargement effectif de l'IDT ; lidt prend en paramètre l'adresse
 * d'une zone contenant la taille puis l'adresse de l'IDT.
 */
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

void initialiserIDT()
{
   int i;
   IDT idt = (IDT) MANUX_ADRESSE_IDT;

   /* Comportement par défaut : on ne fait rien ! */
   for (i = 0; i < 256; i++) {
      positionnerHandlerInterruption(idt, i, stubHandlerNop);
   }
   
   positionnerHandlerInterruption(idt, 0, stubHandlerPanique_0);
   positionnerHandlerInterruption(idt, 1, stubHandlerPanique_1);
   positionnerHandlerInterruption(idt, 2, stubHandlerPanique_2);
   positionnerHandlerInterruption(idt, 3, stubHandlerPanique_3);
   positionnerHandlerInterruption(idt, 4, stubHandlerPanique_4);
   positionnerHandlerInterruption(idt, 5, stubHandlerPanique_5);
   positionnerHandlerInterruption(idt, 6, stubHandlerPanique_6);
   positionnerHandlerInterruption(idt, 7, stubHandlerPanique_7);
   positionnerHandlerInterruption(idt, 8, stubHandlerPanique_8);
   positionnerHandlerInterruption(idt, 9, stubHandlerPanique_9);
   positionnerHandlerInterruption(idt, 10, stubHandlerPanique_10);
   positionnerHandlerInterruption(idt, 11, stubHandlerPanique_11);
   positionnerHandlerInterruption(idt, 12, stubHandlerPanique_12);
   positionnerHandlerInterruption(idt, 13, stubHandlerPanique_13);
   positionnerHandlerInterruption(idt, 14, stubHandlerPanique_14);
   positionnerHandlerInterruption(idt, 15, stubHandlerPanique_15);
   positionnerHandlerInterruption(idt, 16, stubHandlerPanique_16);
   positionnerHandlerInterruption(idt, 17, stubHandlerPanique_17);
   positionnerHandlerInterruption(idt, 18, stubHandlerPanique_18);
   positionnerHandlerInterruption(idt, 19, stubHandlerPanique_19);
   positionnerHandlerInterruption(idt, 20, stubHandlerPanique_20);
   positionnerHandlerInterruption(idt, 21, stubHandlerPanique_21);
   positionnerHandlerInterruption(idt, 22, stubHandlerPanique_22);
   positionnerHandlerInterruption(idt, 23, stubHandlerPanique_23);
   positionnerHandlerInterruption(idt, 24, stubHandlerPanique_24);
   positionnerHandlerInterruption(idt, 25, stubHandlerPanique_25);
   positionnerHandlerInterruption(idt, 26, stubHandlerPanique_26);
   positionnerHandlerInterruption(idt, 27, stubHandlerPanique_27);
   positionnerHandlerInterruption(idt, 28, stubHandlerPanique_28);
   positionnerHandlerInterruption(idt, 29, stubHandlerPanique_29);
   positionnerHandlerInterruption(idt, 30, stubHandlerPanique_30);
   positionnerHandlerInterruption(idt, 31, stubHandlerPanique_31);

   /* Le handler du Timer */
   positionnerHandlerInterruption(idt, intTimer, stubHandlerTimer);

#ifdef MANUX_APPELS_SYSTEME
   /* Le handler de l'interruption utilisée pour les appels système */
   positionnerHandlerInterruption(idt, MANUX_AS_INT, handlerAppelSysteme);
#endif
   
#ifdef MANUX_CLAVIER
   /* Le handler du clavier */
   positionnerHandlerInterruption(idt, intClavier, stubHandlerClavier);
#endif
   
   /* On charge l'IDT */
   chargerIDT(idt);

   /* On autorise les IRQ WARNING pourquoi ? */
   //sti();
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

void ecranDeLaMort(uint32_t itNum, TousRegistres registres,
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
|             | edi  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+";
   
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
   /*
   Descripteur descP = gdtSysteme->descripteur[tssDuFautif->TSSPrecedent >> 3];
   tssDuFautif = (IntelTSS *)(
         (((uint32_t)descP.dt.baseFort) << 24)
       + (((uint32_t)descP.dt.baseInter) << 16)
       + descP.dt.baseFaible
     );
   */
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

}

void handlerPanique(uint32_t itNum, TousRegistres registres,
		    uint32_t eip, uint32_t cs, uint32_t eFlags)
{
   char * ecran = MANUX_CON_SCREEN;
   int d = 1;
   char c;

   ecranDeLaMort(itNum, registres, eip, cs, eFlags);

   while (1) {
      inb(0x60, c);
      switch (c) {
         case 0x81: // ESC
	      basculerVersConsoleSuivante();
            while (c == 0x81){
	      inb(0x60, c);
            };
         break;
         case 0x39: // SPACE On alterne entre l'écran bleu et l'écran au moment du drame 
            d = 1-d;
	    if (d) {
  	      ecranDeLaMort(itNum, registres, eip, cs, eFlags);
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
 
