/*----------------------------------------------------------------------------*/
/*      Implantation des sous-programmes de manipulation du clavier.          */
/*                                                                            */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/
#include <manux/clavier.h>

#include <manux/io.h>
#include <manux/irq.h>
#include <manux/printk.h>
#include <manux/interBasNiveau.h>

#include <manux/scheduler.h> /* basculeConsoleDemandee */

#ifdef MANUX_CLAVIER_CONSOLE
#   include <manux/console.h>
#endif

#include <keymaps/french.h>

int toucheTouche = 0;

void initialiserClavier()
{
   interdireIRQ(IRQClavier);

   toucheTouche = 0;

   /* Activation du clavier */
   outb(portDonneesClavier, 0xf4);
   outb(portCmdClavier, 0xae);

   autoriserIRQ(IRQClavier);
}

#define KEYCODE_ESC  0x01
#define KEYCODE_TAB  0x0F
#define KEYCODE_F1   0x3b
#define KEYCODE_F2   0x3c
#define KEYCODE_F3   0x3d
#define KEYCODE_F4   0x3e
#define KEYCODE_F5   0x3f
#define KEYCODE_F6   0x40
#define KEYCODE_F7   0x41
#define KEYCODE_F8   0x42
#define KEYCODE_F9   0x43
#define KEYCODE_F10  0x44


void handlerClavier()
{
   uint8_t etat;

   inb(0x64, etat);
   if (etat && 0x01) {
      inb(0x60, toucheTouche);
      toucheTouche &= 0xFF;
      //      printk("[KBD-0x%x]\n", toucheTouche);

#ifdef MANUX_CONSOLES_VIRTUELLES
      if (toucheTouche == KEYCODE_ESC) {
         basculeConsoleDemandee = TRUE;
	 return;
      }
#endif
      if (toucheTouche == KEYCODE_TAB) {
 	 afficheEtatSystemeDemande = TRUE;
	 return;
      }
#ifdef MANUX_TACHES
      if (toucheTouche == KEYCODE_F1) {
 	 basculerTacheDemande = TRUE;
	 return;
      }
#endif
#ifdef MANUX_CLAVIER_CONSOLE
      Console * cons;
#   ifdef MANUX_CONSOLES_VIRTUELLES
      cons = consoleActive;
#   else
      cons = _consoleNoyau;
#   endif
      if (toucheTouche & 0x80) {
      } else {
	//         printk(" !!! 0x%x !!!\n", toucheTouche);
         if (cons->bufferClavier){ 
            if (cons->nbCarAttente < 4096) {
               cons->bufferClavier[(cons->indiceProchainCar + cons->nbCarAttente)%4096] = keymap[toucheTouche];
	       //	       printk("b[%d] = 0x%x\n", (cons->indiceProchainCar + cons->nbCarAttente)%4096, keymap[toucheTouche]);
	       cons->nbCarAttente++;
             }
 	 }
      }
#endif
   }
}
