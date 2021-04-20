/*----------------------------------------------------------------------------*/
/*      Implémentation des sous-programmes de gestion des déroutements.       */
/*                                                                            */
/*                                     (C) Manu Chaput 2000, 2001, 2002, 2003 */
/*----------------------------------------------------------------------------*/
#include <manux/interruptions.h>

#include <manux/interBasNiveau.h>
#include <manux/io.h>
#include <manux/i386.h>           /* str */
#include <manux/scheduler.h>      /* basculerTache */
#include <manux/appelsysteme.h>   /* MANUX_AS_INT */
#include <manux/atomique.h>
#include <manux/printk.h>

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
   int i;

   /* Comportement par défaut : on ne fait rien ! */
   for (i = 0; i < 256; i++) {
      positionnerHandlerInterruption(idt, i, stubHandlerNop);
   }

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

void handlerWarning()
{
   printk("EXCEPTION received\n");
}
