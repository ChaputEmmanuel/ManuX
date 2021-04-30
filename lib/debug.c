/*----------------------------------------------------------------------------*/
/*      Quelques outils de debug pour ManuX.                                  */
/*                                                  (C) Manu Chaput 2000-2021 */
/*----------------------------------------------------------------------------*/

#include <manux/debug.h>

#include <manux/i386.h>

void paniqueNoyau(const char * funcName, const char * fileName, int lineNum, char * msg)
{
   printk("\n*** PANIQUE NOYAU ***\n");
   printk("%s (dans %s ligne %d)\n", funcName, fileName, lineNum);
   printk("%s\n", msg);
   halt();
}
