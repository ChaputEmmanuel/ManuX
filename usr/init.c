/*
 * Attention, ici les include sont dans usr/include !!!
 */
//#include <manux/scheduler.h>  // basculerTache devrait être un AS !!
//#include <manux/printk.h>

#include <manux/types.h>
#include <stdio.h>

#include <manux/i386.h> // halt
#include <manux/appelsysteme.h> // basculerTache (bof !)

void calculerPremiers(int q)
/*
 * Calcul et affichage des nombres premiers. Le but est juste de faire
 * bosser le processeur, donc on ne cherche pas du tout à faire les
 * choses vite ou bien.
 */
{
   int n; /* Indice de la boucle de recherche d'un premier*/
   int d; /* Indice de la boucle de recherche de diviseur */
   booleen compose;
   int cpt = 1;
   
   while (TRUE) {
     printf("[%d] C'est la boucle numero %d : \n", q, cpt++);
      printf("%d", 2);
      for (n = 3; n < 1024; n += 2) {
         compose = FALSE;
         d = 3;
         while ((!compose) && (d*d <= n)) {
	    compose = compose || (n%d == 0);
            d += 2;
         }
         if (!compose) {
    	   printf(", %d", n); for (int i = 0; i<10000000; i+=2){asm("");};
	 }
      }
      printf("\n--------------------------------------------------------------------------------\n");
      //      if (cpt %3 == 0 ) basculerTache();
      basculerTache();
   }
}

void prout()
{
   int n;

   printf("Prout\n");
 
   for (n = 0; n < 10000000; n++) {
      calculerPremiers(2);
   }
}

void init()
{
   int n=2; // nombre de messages affichés

   printf("Greetings from userland !\n");

   n = creerNouvelleTache(prout, FALSE);

   for (n = 0; n < 10000000; n++) {
      calculerPremiers(1);
   }
}
