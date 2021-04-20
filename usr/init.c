#include <manux/scheduler.h>
#include <manux/printk.h>
#include <stdio.h>


void calculerPremiers()
/*
 * Calcul et affichage des nombres premiers. Le but est juste de faire
 * bosser le processeur, donc on ne cherche pas du tout à faire les
 * choses vite ou bien.
 */
{
   int n; /* Indice de la boucle de recherche d'un premier*/
   int d; /* Indice de la boucle de recherche de diviseur */
   booleen compose;

   while (TRUE) {
      printk("%d", 2);
      for (n = 3; n < 1073741820; n += 2) {
         compose = FALSE;
         d = 3;
         while ((!compose) && (d*d <= n)) {
	    compose = compose || (n%d == 0);
            d += 2;
         }
         if (!compose) {
	   printk(", %d", n); for (int i = 0; i<10000000; i+=2){asm("");};
	 }
      }
   }
}

void init()
{
   int n; // nombre de messages affichés
   int i,j;

   printk("Init est parti ...\n");

   printf("PRINTF PRINTF PRINTF !!!!\n");

   for (n = 0; n < 10000000; n++) {
      printk("Init is alive\n");
      calculerPremiers();
      basculerTache();
   }
}
