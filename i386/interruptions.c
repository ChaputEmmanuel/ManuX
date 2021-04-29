/*----------------------------------------------------------------------------*/
/*      Implémentation des sous-programmes de gestion des déroutements.       */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/interruptions.h>

#include <manux/interBasNiveau.h>
#include <manux/io.h>
#include <manux/i386.h>           /* str */
#include <manux/segment.h>        /* gdtSysteme */
#include <manux/scheduler.h>      /* basculerTache */
#include <manux/appelsysteme.h>   /* MANUX_AS_INT */
#include <manux/atomique.h>
#include <manux/console.h>        /* Caractéristiques de l'écran */
#include <manux/tache.h>          /* IntelTSS */

#define SELECTEUR_SEGMENT_CODE 0x08      /* WARNING a mettre ailleurs */

extern void handlerAppelSysteme();  /* WARNING à définir dans un .h */

/*
 * Pour basculer vers le scheduler //WARNING beurk !
 */
extern Tache * tacheScheduler ;
extern Atomique schedulerEnCours;

uint32 nbTicks = 0;

void exDivisionParZero(TousRegistres registres,
                       uint32 eip, uint32 cs, uint32 eFlags)
{
}

/*
 * Le handler du timer (irq 8)
 */
void handlerTimer(TousRegistres registres,
                  uint32 eip, uint32 cs, uint32 eFlags)
{
   nbTicks++;

   outb(0x20, 0x20);

   /* Si le scheduler n'est pas déjà en cours, on l'active */
   /*   if (atomiqueTestInit(&schedulerEnCours, 1, 0)) {
      activerTache(tacheScheduler);
      }*/
}

void positionnerHandlerInterruption(IDT idt, int i, Handler handler)
/*
 * Affectation du handler de l'interruption i
 */
{
   idt[i].itg.offsetFaible = ((uint32)handler & 0xFFFF);
   idt[i].itg.offsetFort = ((uint32)handler >> 16);
   idt[i].itg.parametres = 0x8E00;
   idt[i].itg.selSegment = SELECTEUR_SEGMENT_CODE;
}

void chargerIDT(IDT idt)
/*
 * Chargement effectif de l'IDT ; lidt prend en paramètre l'adresse
 * d'une zone contenant la taille puis l'adresse de l'IDT.
 */
{
   volatile uint8 argument[6];

   argument[0] = 0xFF;
   argument[1] = 0x07;
   argument[2] = ( (uint32)idt      & 0xFF);
   argument[3] = (((uint32)idt>>8)  & 0xFF);
   argument[4] = (((uint32)idt>>16) & 0xFF); 
   argument[5] = (((uint32)idt>>24) & 0xFF);

   __asm__ __volatile__ ("lidt (%0)"
                         : /* Pas de sortie */
                         :"a" ((char *)argument)
                        );
   //return argument[0] +  argument[1]+argument[2]+argument[3]+ argument[4]+argument[5];

}

void initialiserIDT()
{
   IDT idt = (IDT) 0x30000; /* WARNING pas terrible ! */

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

   /* Le handler de l'interruption utilisée pour les appels système */
   positionnerHandlerInterruption(idt, MANUX_AS_INT, handlerAppelSysteme);

   /* Le handler du clavier */
   positionnerHandlerInterruption(idt, intClavier, stubHandlerClavier);

   /* On charge l'IDT */
   chargerIDT(idt);

   /* On autorise les IRQ WARNING pourquoi ? */
 //  sti();
}

/*
 * Affichage de la valeur v en hexa sur n chiffres à la position l, c
 */
#define afficherHexa(v, n, l, c)		                        \
{                                                                       \
   uint32 __val = (uint32) v;						\
   for (int __i = CON_COLONNES*l+c+n-1; __i>=CON_COLONNES*l+c; __i--) { \
      ecran[2*__i] = chiffre[__val&15] ;                                \
      __val /=16 ;			                                \
   }                                                                    \
}
#define afficherBin(v, n, l, c)		                        \
{                                                                       \
   uint32 __val = (uint32) v;						\
   for (int __i = CON_COLONNES*l+c+n-1; __i>=CON_COLONNES*l+c; __i--) { \
      ecran[2*__i] = chiffre[__val&1] ;                                \
      __val /=2 ;			                                \
   }                                                                    \
}

void handlerPanique(uint32 itNum, TousRegistres registres,
		    uint32 eip, uint32 cs, uint32 eFlags)
{
   /* A définir ailleurs lorsque ce sera au point */
   char *ecranPanique ="\
+-------------+-------------------+--------32--28--24--20--16--12---8---4---0--+\
| it : 0x--   | tss  = 0x-------- | eflags=  --------------------------------  |\
+-------------+-------------------+-------------+---+---+---+---+---+---+---+--+\
| cs : 0x---- | eip  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+\
| ss : 0x---- | esp  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+\
| ds : 0x---- | esi  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+\
|             | edi  = 0x-------- |                                            |\
+-------------+-------------------+--------------------------------------------+";
   
   char chiffre[16] = "0123456789ABCDEF";
   char * ecran = CON_SCREEN;
   int i;
   uint32 indice;
   
   // On fait un peu de place en haut de l'écran
   for (i=0; i<11*2*CON_COLONNES; i+=2) {
      ecran[i]   = ecranPanique[i/2];
      ecran[i+1] = COUL_FOND_GRIS_CLAIR | COUL_TXT_BLEU;
   }
   __asm__ __volatile__("str %%eax" : "=a" (indice));
   
   Descripteur desc = gdtSysteme->descripteur[indice>>3];
   IntelTSS * tssDuFautif = (IntelTSS *)(
         (((uint32)desc.dt.baseFort) << 24)
       + (((uint32)desc.dt.baseInter) << 16)
       + desc.dt.baseFaible
     );
   /*
   Descripteur descP = gdtSysteme->descripteur[tssDuFautif->TSSPrecedent >> 3];
   tssDuFautif = (IntelTSS *)(
         (((uint32)descP.dt.baseFort) << 24)
       + (((uint32)descP.dt.baseInter) << 16)
       + descP.dt.baseFaible
     );
   */
   // Première ligne
   afficherHexa(itNum, 2, 1, 9);
   afficherHexa(tssDuFautif, 8, 1, 25);
   //   afficherHexa(indice, 4, 1, 45);
   afficherBin(tssDuFautif->EFLAGS, 32, 1, 45);

   // Les descripteurs de segment
   afficherHexa(tssDuFautif->CS, 4, 3, 9);
   afficherHexa(tssDuFautif->SS, 4, 5, 9);
   afficherHexa(tssDuFautif->DS, 4, 7, 9);

   // Les pointeurs
   afficherHexa(tssDuFautif->EIP, 8, 3, 25);
   afficherHexa(tssDuFautif->ESP, 8, 5, 25);
   afficherHexa(tssDuFautif->ESI, 8, 7, 25);
   afficherHexa(tssDuFautif->EDI, 8, 9, 25);
   
   __asm__ __volatile__ ("hlt");
}
